/*
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

#include <xc.h>
#include "RFM69HCW.h"

//Pin Definitions
#define RST PORTCbits.RC2
#define SS PORTAbits.RA2
#define TX_TRIGGER PORTAbits.RA4    //check later if this is an open pin

//Programmable Constants
#define NODE_ADDR 0x30
#define _XTAL_FREQ 16000000
#define PAYLOAD_LENGTH 0x08 //8 byte payload

void spi_init_master();
void resetRadio();
void rfmSetup();

byte rssiThresh=0;
byte rssi=0;
byte fifoFlags=0;
byte modeFlags=0;
byte powerSettings=0;
byte operatingMode=0;

void resetRadio(){
    RST = 1;
    __delay_ms(1);
    RST = 0;
    __delay_ms(20);
}

int init(){
    rfmSetup();
    return 0;
}


void rfmSetup(){
    TRISCbits.TRISC2=0;
    TRISCbits.TRISC3=0;
    TRISCbits.TRISC5=0;
    SSP1BUF;
    SSP1STATbits.BF=0;
    OSCCONbits.IRCF=0b1111; //internal CLK
    spi_init_master();
    TRISCbits.TRISC5=0;
    resetRadio();
    TRISAbits.TRISA2=0;
    __delay_ms(20);
}



int main() {
    TRISAbits.TRISA4=1;
    TRISCbits.TRISC5=0;
    TRISCbits.TRISC4=0;
    init();
    PORTCbits.RC5=1; //set RC5 to high to use for setting RA4 trigger switch (missing extra VCC source)
    PORTCbits.RC4=0; //same as above but for low signal
    RFM69 radio = {.SSport=&PORTA, .SSbit=2, .node_address=0x30,.packet_length=PAYLOAD_LENGTH,
            .RSSI_threshold=0xE4, .sync_words = {0x67,0x67,0x67,0x67,0x67,0x67,0x67,0x67}};
    initRadio(&radio);
    //button configs
    byte payload[PAYLOAD_LENGTH] = {0x48, 0x45, 0x4C, 0x4C, 0x4F, 0x20, 0x20, '0'};
    setPowerOutput(&radio, HW_POWER_MIN);
    setModulationThreshold(&radio);
    setAutoMode(&radio, 0b01011011); //tx auto modes, fifo level is trigger, packet sent is return to sleep
    while(1){
            fifoFlags = getFifoFlags(&radio);
            modeFlags = getModeFlags(&radio);
            sendPayload(&radio, payload);
            fifoFlags = getFifoFlags(&radio);
            modeFlags = getModeFlags(&radio);
            __delay_ms(100); //delay for pseudo-hysteresis, implement button press later as an interrupt pin-change
            operatingMode = getOperatingMode(&radio);
            payload[PAYLOAD_LENGTH-1]++; //increment ascii value by one to discern different messages
    }
    return 0;
}

void spi_init_master(){
    ANSELA=0;
    ANSELC=0;
    SSP1CON1bits.SSPM=0b0001;
    SSP1CONbits.SSPEN=1;
    SSP1CONbits.CKP=0; //check rfm
    SSP1STATbits.CKE=1; //check rfm
    SSP1STATbits.SMP=0; //check rfm (sampling at end of clock)
    //SPI clock select
    RC0PPS=0b10000;
    TRISCbits.TRISC0=0;

    //SPI SDI select
    SSPDATPPS=0b10001;
    TRISCbits.TRISC1=1;
    
    //SPI SDO select
    RC3PPS=0b10001;
    TRISCbits.TRISC3=0;
}
*/