#include <xc.h>
#include "RFM69HCW.h"
byte recommended_params[10] = {0x18|WRITE,0x08,0x19|WRITE,0x55,0x1A|WRITE,0x8B,0x6F|WRITE,0x00};



void sendPairs(RFM69* radio, byte msgs[], const int len){
    byte msg[2] ={0};
    for(int i=0;i<len;i+=2){
        msg[0]=msgs[i];msg[1]=msgs[i+1];
        sendBulk(radio, msg,2);
    }
}

void sendBulk(RFM69* radio, byte msgs[], const byte len){
    setPinWithMask(radio->SSport, radio->SSbit, 0);
    for(int i=0; i<len; i++){
        msgs[i]=send(radio, msgs[i]);
    }
    setPinWithMask(radio->SSport, radio->SSbit,1);
}


byte send(RFM69* radio, byte msg){
    SSP1BUF=msg;
    while(SSP1CONbits.WCOL){  //this condition should never return true, but test it to make sure
            SSP1CONbits.WCOL=0;
            SSP1BUF=msg;
    }
    while(!SSP1STATbits.BF); //wait for serial buffer to exchange
    radio->sent[radio->sendReceiveIndex]=msg;
    radio->sendReceiveIndex++;
    if(radio->sendReceiveIndex>(LEN_MESSAGE_BOOK-1)){
        radio->sendReceiveIndex=0;
    }
    radio->received[radio->sendReceiveIndex]=SSP1BUF;
    return radio->received[radio->sendReceiveIndex];
}

void setPinWithMask(byte* pPort, byte iPin, byte value) { 
    if(value==0){
       *pPort &= ~(1<<iPin);
    }else{
       *pPort |= 1 << iPin;    
    }
} 

void setSyncWords(RFM69* radio){
    unsigned char msg[10];
    msg[0]=SYNC_CONFIG|WRITE; msg[1]=0b10011000;
    for(int i=2;i<10;i++){    //index at 2 because offset by sending first 2 messages 
        msg[i]=(radio->sync_words[i]);
    }
    sendBulk(radio,msg,10);
}

void setPacketLength(RFM69* radio){
    byte msg[] = {PACKET_LEN|WRITE, radio->packet_length};
    sendBulk(radio, msg, 2);
}

void setNodeAddress(RFM69* radio){
    byte msg[] = {NODE_ADDR|WRITE, radio->node_address};
    sendBulk(radio, msg, 2);
}

void setRSSIThreshold(RFM69* radio){
    byte msg[] = {RSSI_THRESH|WRITE, radio->RSSI_threshold};
    sendBulk(radio, msg, 2);
}

void setPacketFormat(RFM69* radio){
    byte msg[] = {PACKET_FORMAT|WRITE, 0b00010000};
    sendBulk(radio, msg, 2);
}

void setOperatingMode(RFM69* radio, byte operatingMode){
    byte msg[] = {OPERATING_MODE|WRITE, operatingMode};
    sendBulk(radio, msg, 2);
}

void setRecommendedParams(RFM69* radio){
    sendPairs(radio, recommended_params, 8);
}

void setModulationThreshold(RFM69* radio){
    byte msg[] = {FIFO_THRESH|WRITE, 0x07}; //packet length -1 and set MSb to 0
    sendBulk(radio, msg, 2);
}

byte checkForPayload(RFM69* radio){
    byte flags[] = {FIFO_FLAGS, 0}; 
    sendBulk(radio, flags, 2);
    return (flags[1] & (1<<2));
}

void receivePayload(RFM69* radio){
    setPinWithMask(radio->SSport, radio->SSbit,0);
    send(radio, FIFO);
    for(int i=0;i<(*radio).packet_length;i++){
        radio->payload[i]=send(radio,FIFO);
    }
    setPinWithMask(radio->SSport, radio->SSbit,1);
}

void setPreambleLength(RFM69* radio){
    byte msg[] = {PREAMBLE_LEN_LSB|WRITE, 0x03};
    sendBulk(radio, msg, 2);
}

void initRadio(RFM69* radio){
    setSyncWords(radio);
    setPacketLength(radio);
    setNodeAddress(radio);
    setRSSIThreshold(radio);
    setPacketFormat(radio);
    setRecommendedParams(radio);
    setModulationThreshold(radio);
    setPreambleLength(radio);
}

void setPowerOutput(RFM69* radio, byte powerSettings){
    byte msg[] = {POWER_REG|WRITE, powerSettings};
    sendBulk(radio,msg,2);
}

void sendPayload(RFM69* radio, byte payload[]){
    setPinWithMask(radio->SSport, radio->SSbit,0);
    send(radio, FIFO|WRITE);
    for(int i=0;i<radio->packet_length;i++){
        send(radio,payload[i]);
    }
    setPinWithMask(radio->SSport, radio->SSbit,1);
}

byte getRSSI(RFM69* radio){
    byte msg[]={RSSI_VALUE, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

byte getFifoFlags(RFM69* radio){
    byte msg[]={FIFO_FLAGS, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

byte getRSSIThreshold(RFM69* radio){
    byte msg[]={RSSI_THRESH, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

byte getOperatingMode(RFM69* radio){
    byte msg[]={OPERATING_MODE, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

void setDiodeMapping(RFM69* radio, byte mapping){
    byte msg[]={0x25, mapping};
    sendBulk(radio, msg, 2);
    return;
}

byte getModeFlags(RFM69* radio){
    byte msg[]={MODE_FLAGS, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

byte checkRSSISampling(RFM69* radio){
    byte msg[]={RSSI_CONFIG, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

void startRSSISampling(RFM69* radio){
    byte msg[]={RSSI_CONFIG|WRITE, 0x1};
    sendBulk(radio, msg, 2);
    return;
}

void restartRx(RFM69* radio){
    byte msg[]={RX_CONFIG|WRITE, 0b00000110};
    sendBulk(radio, msg, 2);
    return;
}

byte getPowerSettings(RFM69* radio){
    byte msg[]={POWER_REG, 0x0};
    sendBulk(radio, msg, 2);
    return msg[1];
}

void setAutoMode(RFM69* radio, byte autoModes){
    byte msg[]={AUTO_MODE|WRITE, autoModes};
    sendBulk(radio, msg, 2);
    return;
}
