/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#define FIFO 0x00
#define OPERATING_MODE 0x01
#define DATA_MODE 0x02
#define BITRATE_MSB 0x03
#define BITRATE_LSB 0x04
#define DIO1_MAP 0x25
#define MODE_FLAGS 0x27
#define FIFO_FLAGS 0x28
#define PREAMBLE_LEN_MSB 0x2c
#define PREAMBLE_LEN_LSB 0x2d
#define PACKET_MODE 0x17
#define PACKET_FORMAT 0x37
#define RX_CONFIG 0x3D
#define RSSI_CONFIG 0x23
#define RSSI_VALUE 0x24
#define PACKET_LEN 0x38
#define NODE_ADDR 0x39
#define BROADCAST_ADDR 0x40
#define RSSI_THRESH 0x29
#define FIFO_THRESH 0x3C
#define SYNC_CONFIG 0x2e
#define SYNC_START 0x2f
#define RECEIVING 0xB0
#define TRANSMITTING 0xAC
#define STANDBY 0xA4 //0xA4 is forced mode by user version
#define POWER_REG 0x11
#define LEN_MESSAGE_BOOK 20
#define POWER_DEFAULT 0x9F
#define HW_POWER_MIN 0x5F
#define HW_POWER_MED 0x7F
#define HW_POWER_MAX 0x7F
#define WRITE 0x80
#define AUTO_MODE 0x3B

#include <xc.h> // include processor files - each processor file is guarded.  

typedef unsigned char byte;

typedef struct RFM69HCW { 
   byte received[LEN_MESSAGE_BOOK];
   byte sent[LEN_MESSAGE_BOOK];
   byte *SSport;
   byte SSbit;
   byte packet_length;
   byte payload[LEN_MESSAGE_BOOK];
   byte node_address;
   byte RSSI_threshold;
   byte sync_words[8];
   byte sendReceiveIndex;
} RFM69;

byte SYNC_WORDS[8] = {0x67,0x67,0x67,0x67,0x67,0x67,0x67,0x67};

void sendBulk(RFM69* radio, byte msgs[], const byte len);
byte send(RFM69* radio, byte msg);
void setPinWithMask(byte* pPort, byte iPin, byte value);
void receive(RFM69* radio, byte msg);
byte checkForPayload(RFM69* radio);
void receivePayload(RFM69* radio);
void setPreambleLength(RFM69* radio);
void initRadio(RFM69* radio);
void setModulationThreshold(RFM69* radio);
void sendPayload(RFM69* radio, byte payload[]);
void setOperatingMode(RFM69* radio, byte operatingMode);
byte getRSSI(RFM69* radio);
void setPowerOutput(RFM69* radio, byte powerSettings);
byte getRSSIThreshold(RFM69* radio);
byte getFifoFlags(RFM69* radio);
byte getOperatingMode(RFM69* radio);
void setDiodeMapping(RFM69* radio, byte mapping);
byte getModeFlags(RFM69* radio);
byte checkRSSISampling(RFM69* radio);
void startRSSISampling(RFM69* radio);
void restartRx(RFM69* radio);
byte getPowerSettings(RFM69* radio);
void setAutoMode(RFM69* radio, byte autoModes);
// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

