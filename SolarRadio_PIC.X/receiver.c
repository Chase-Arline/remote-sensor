
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
#pragma config PLLEN = OFF       // PLL Enable Bit (4x PLL)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF    // Low-Voltage Programming Enable (Low-voltage programming enabled)

// CONFIG3
#pragma config WDTCPS = WDTCPS1F// WDT Period Select (Software Control (WDTPS))
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT enabled)
#pragma config WDTCWS = WDTCWSSW// WDT Window Select (Software WDT window size control (WDTWS bits))
#pragma config WDTCCS = SWC     // WDT Input Clock Selector (Software control, controlled by WDTCS bits)

#include <xc.h>
#include "RFM69HCW.h"

//Pin Definitions
#define RST PORTCbits.RC2
#define SS PORTAbits.RA2
#define RX_TRIGGER PORTAbits.RA5    //check later if this is an open pin

//Programmable Constants
#define _XTAL_FREQ 16000000

#define PAYLOAD_LENGTH 4 

void spi_init_master();
void resetRadio();
void rfmSetup();

byte operatingMode;
byte RSSI;
byte fifoFlags;
byte modeFlags;
byte RSSISampling;
float thermistorTemp;

void TRIS_ANS_Setup() {
    ANSELA = 0; //majority digital connections
    ANSELC = 0; // ^
    TRISC0 = 0; //SPI clock output
    TRISC1 = 1; //SPI data input
    TRISC2 = 0; //RESET output to radio
    TRISC3 = 0; //SPI data output
    TRISC5 = 1; //thermistor divider input
    TRISA2 = 0; // slave select output (unnecessary but still connected for potential future devices outside of PCB))
    TRISA5 = 1; //RX trigger input, determines if radio is in receiving mode
}

void resetRadio() {
    RST = 1;
    __delay_ms(1);
    RST = 0;
    __delay_ms(20);
}

int init() {
    TRIS_ANS_Setup();
    rfmSetup();
    return 0;
}

void rfmSetup() {
    SSP1BUF;
    SSP1STATbits.BF = 0;
    spi_init_master();
    resetRadio();
}

float bytesToFloat(byte *bytes){
    return (float)(bytes[0] << 24 | bytes[1] << 16 | bytes[2] <<8 | bytes[3]);
}

int main() {
    init();
    RFM69 radio = {.SSport = &PORTA, .SSbit = 2, .node_address = 0x20, .packet_length = PAYLOAD_LENGTH,
        .RSSI_threshold = 0xE4, .sync_words =
        {0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67}};
    initRadio(&radio);
    setPowerOutput(&radio, HW_POWER_MIN);
    
    setDiodeMapping(&radio, 0x40); //set D0 to payload ready, leave rest as default
    setAutoMode(&radio, 0b01100100); //enter sleep on rising edge of CrcOk, exit sleep mode on falling edge of fifoNotEmpty (fifo is now empty), 
    // to use auto mode: put radio in receiving mode and it will perform these actions: go into sleep mode when packet is ready, go back to receiving when packet is transferred to PIC
    setOperatingMode(&radio, RECEIVING);
    byte reversedPayload[4];
    while (1) {
        RSSI = getRSSI(&radio);
        fifoFlags = getFifoFlags(&radio);
        modeFlags = getModeFlags(&radio);
        operatingMode = getOperatingMode(&radio);
        if ((((operatingMode & 0x7F) == 0))) { //check if payload is ready
            fifoFlags = getFifoFlags(&radio);
            __delay_ms(30);
            receivePayload(&radio);
            for(int i=0; i<4;i++){
                reversedPayload[i] = radio.payload[3-i];
            }
            thermistorTemp = bytesToFloat(reversedPayload);
        }
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
