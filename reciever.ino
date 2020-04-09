#include <XBee.h>
#include <SD.h>
#include <SPI.h>
//SD card


//Xbee

int count=1;
int start = 1;
XBee xbee = XBee();

typedef struct XBeeStruct {
  long seq;
  float temp;
  float pulse;
  int rssi;
} XBeeDataStruct;
static XBeeDataStruct XBeeData;


//Ack packet
typedef struct AckStruct {
  boolean flag = true;
} AckPacketStruct;
static AckPacketStruct AckData;



//Recieve Variables
Rx16Response rx16 = Rx16Response();
uint8_t* data = 0;
int len = 0;
long sequence = 1;

void setup()
{
    
    xbee.setSerial(Serial2);
    Serial2.begin(9600);


    while(1){
    xbee.readPacket(10);
    if (xbee.getResponse().isAvailable())
    {
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
        {
            xbee.getResponse().getRx16Response(rx16);
            data = rx16.getData();
            len = rx16.getDataLength();
            AckData = (AckPacketStruct &)*data;
            
            if(AckData.flag == false)
            {
            XBeeAddress64 addr64 = XBeeAddress64(0x0000, 0xFFFF);
            Tx16Request tx16 = Tx16Request(addr64, (uint8_t *)&AckData, sizeof(AckPacketStruct));
            xbee.send( tx16 );
            }
        }
    }  
    }

    //Time to initialize XBee 
    delay(5000);
}


void loop()
{
    
   
    xbee.readPacket(10);
    if (xbee.getResponse().isAvailable())
    {
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
        {
            xbee.getResponse().getRx16Response(rx16);
            data = rx16.getData();
            len = rx16.getDataLength();
            XBeeData = (XBeeDataStruct &)*data;
            int RSSI_VALUE = -1*rx16.getRssi();
            //Print.
            /*
            Serial.println("Data Obtained:");
            Serial.print("\tRSSI = ");
             
            Serial.print(RSSI_VALUE);
            Serial.print("\ttemp = ");
            Serial.print(XBeeData.temp);
            Serial.print("\tpulse = ");
            Serial.print(XBeeData.pulse);
            
            */

            count++;

            //Sending ACK Packets
            XBeeAddress64 addr64 = XBeeAddress64(0x0000, 0xFFFF);
            Tx16Request tx16 = Tx16Request(addr64, (uint8_t *)&AckData, sizeof(AckPacketStruct));
            xbee.send( tx16 );
            
            //Serial.println(count-1);
            //Serial.println("Close");
            
        }
    }
   
}