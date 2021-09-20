
// PIC16F1615 Configuration Bit Settings

// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switch Over (Internal External Switch Over mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCD = OFF        // Zero Cross Detect Disable Bit (ZCD disable.  ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PLLEN = OFF       // PLL Enable Bit (4x PLL is always enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF    // Low-Voltage Programming Enable (Low-voltage programming enabled)

// CONFIG3
#pragma config WDTCPS = WDTCPS1F// WDT Period Select (Software Control (WDTPS))
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT enabled)
#pragma config WDTCWS = WDTCWSSW// WDT Window Select (Software WDT window size control (WDTWS bits))
#pragma config WDTCCS = SWC     // WDT Input Clock Selector (Software control, controlled by WDTCS bits)

#include "main.h"
#include "RFM69HCW.h"
#include <xc.h>
#include <math.h>

//Pin Definitions
#define RST PORTCbits.RC2
#define SS PORTAbits.RA2
#define TX_TRIGGER PORTAbits.RA5    
#define THERMISTOR PORTAbits.RA4
#define TH_ENABLE PORTCbits.RC5

//Programmable Constants
#define NODE_ADDR 0x30
#define _XTAL_FREQ 16000000
#define PAYLOAD_LENGTH 4 
#define T0 25.0 //room temp celsius
#define B_COEFF 3950.0 //thermistor coefficient
#define TH_NOM 10000.0 //10k NTC thermistor
#define SERIES_RESISTOR 10000.0 //divider resistor

void spi_init_master();
void resetRadio();
void rfmSetup();

byte rssiThresh = 0;
byte rssi = 0;
byte fifoFlags = 0;
byte modeFlags = 0;
byte powerSettings = 0;
byte operatingMode = 0;
float thermistorTemp;

void TRIS_ANS_Setup() {
    ANSELA = 0; //majority digital connections
    ANSELC = 0; // ^
    TRISC0 = 0; //SPI clock
    TRISC1 = 1; //SPI data input
    TRISC2 = 0; //RESET output to radio
    TRISC3 = 0; //SPI data output
    TRISC5 = 0; //thermistor enable 
    TRISA5 = 1; //tx trigger input
    TRISA2 = 0; // slave select output (unnecessary but still connected for potential future devices outside of PCB))
    TRISA4 = 1; //thermistor divider input
    ANSA4 = 1; // analog input on RA4
}

void analogSetup() {
    //analog reference defaults to VDD as positive reference
    ADFM = 0; //left justified readings (only use ADRESH for 0-255)
    WPUA4 = 0;
}

void resetRadio() {
    RST = 1;
    __delay_ms(1);
    RST = 0;
    __delay_ms(20);
}

int init() {
    TRIS_ANS_Setup();
    analogSetup();
    rfmSetup();
    return 0;
}

void rfmSetup() {
    SSP1BUF;
    SSP1STATbits.BF = 0;
    spi_init_master();
    resetRadio();
    __delay_ms(20);
}

float thermistor_temp() {
    ADCON0bits.CHS = 0b00011; //thermistor A4 on AN3 input
    ADON = 1; //turn ADC on
    TH_ENABLE = 1;
    GO = 1;
    while (GO);
    TH_ENABLE = 0;
    ADON = 0;
    float resistance = TH_NOM / ((255.0 / ADRESH) - 1);
    float temperature = resistance / TH_NOM;
    temperature = log(temperature);
    temperature /= B_COEFF;
    temperature += 1.0 / (T0 + 273.15);
    temperature = 1.0 / temperature;
    temperature -= 273.15;
    return temperature;
}

int main() {
    init();
    RFM69 radio = {.SSport = &PORTA, .SSbit = 2, .node_address = 0x30, .packet_length = PAYLOAD_LENGTH,
        .RSSI_threshold = 0xE4, .sync_words =
        {0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67}};
    initRadio(&radio);
    byte *payload = &thermistorTemp;
    setPowerOutput(&radio, POWER_DEFAULT);
    setModulationThreshold(&radio);
    setOperatingMode(&radio, SLEEP_MODE);
    setAutoMode(&radio, 0b01011011); //tx auto modes, fifo level is trigger, packet sent is return to previous state
    while (1) {
        thermistorTemp = thermistor_temp();
        fifoFlags = getFifoFlags(&radio);
        modeFlags = getModeFlags(&radio);
        if (TX_TRIGGER) {
            sendPayload(&radio, payload);
        }
        fifoFlags = getFifoFlags(&radio);
        modeFlags = getModeFlags(&radio);
        __delay_ms(10); //delay for pseudo-hysteresis, implement button press later as an interrupt pin-change
        operatingMode = getOperatingMode(&radio);
        payload[PAYLOAD_LENGTH - 1]++; //increment ascii value by one to discern different messages
    }
    return 0;
}

void spi_init_master() {
    SSP1CON1bits.SSPM = 0b0001;
    SSP1CONbits.SSPEN = 1;
    SSP1CONbits.CKP = 0; //check rfm
    SSP1STATbits.CKE = 1; //check rfm
    SSP1STATbits.SMP = 0; //check rfm (sampling at end of clock)
    //SPI clock select
    RC0PPS = 0b10000;
    //SPI SDI select
    SSPDATPPS = 0b10001;
    //SPI SDO select
    RC3PPS = 0b10001;
}
