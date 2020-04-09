#include <XBee.h>
#include <Wire.h>

//Xbee struct
typedef struct XBeeStruct {
  long seq; 
  float temp;
  float pulse;
  int rssi;
} XBeeDataStruct;
static XBeeDataStruct XBeeData;


//Ack packet and ecieve variables
typedef struct AckStruct {
  boolean flag = false;
} AckPacketStruct;
static AckPacketStruct AckData;
Rx16Response rx16 = Rx16Response();
uint8_t* data = 0;

int tempPin = 0;
int pulsePin = 1;
XBee xbee = XBee();
long sequence=1;
int lastRSSI=0;
int len = 0;



void setup()
{
    Serial.begin(9600);

    //To initialize Xbee Modules
    delay(5000);

    while(1){
    //Send init packets to initialize connection
   
    XBeeAddress64 addr64 = XBeeAddress64(0x0000, 0xFFFF);
    Tx16Request tx16 = Tx16Request(addr64, (uint8_t *)&AckData, sizeof(AckPacketStruct));
    xbee.send( tx16 );

    xbee.readPacket(10);
    if (xbee.getResponse().isAvailable())
    {
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
        {
            xbee.getResponse().getRx16Response(rx16);
            data = rx16.getData();
            len = rx16.getDataLength();
            AckData = (AckPacketStruct &)*data;
            if(AckData.flag == true)
            {
              int lastRSSI = -1*rx16.getRssi();
              break;
            }
        }
    }
  }
}

void loop()
{
   // read analog volt from sensor and save to variable temp
    
    XBeeData.temp = analogRead(tempPin) * 0.48828125;
    XBeeData.pulse = analogRead(pulsePin) ;
    
    XBeeData.seq = sequence;
    sequence++;
    XBeeData.rssi = lastRSSI;

    
    XBeeAddress64 addr64 = XBeeAddress64(0x0000, 0xFFFF);
    Tx16Request tx16 = Tx16Request(addr64, (uint8_t *)&XBeeData, sizeof(XBeeDataStruct));
    xbee.send( tx16 );

    //Recieving ACK Packet
    xbee.readPacket(10);
    if (xbee.getResponse().isAvailable())
    {
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE)
        {
            xbee.getResponse().getRx16Response(rx16);
            data = rx16.getData();
            len = rx16.getDataLength();
            AckData = (AckPacketStruct &)*data;
            if(AckData.flag == true)
            {
              int lastRSSI = -1*rx16.getRssi();
            }
        }
    }
}