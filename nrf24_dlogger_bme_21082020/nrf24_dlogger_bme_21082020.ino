/*
Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

BME280 
VCC   -> 3V3
SDA   -> A4
SCL   -> A5
GND   -> GND
*/

#include <SPI.h>
#include <NRFLite.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
const static uint8_t RF_RX_LED = 3;

enum RadioPacketType
{
  //  Heartbeat
  BME280data
};

struct RadioPacket
{
    RadioPacketType PacketType;
    uint8_t SensorID;
    float T;
    float H;
};

NRFLite _radio;
uint32_t _lastpacketSendTime;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

void setup()
{
    pinMode(RF_RX_LED, OUTPUT);
    Serial.begin(115200);
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
      Serial.println("Cannot communicate with RF");
      while (1); 
    }
//    if (!bme.begin(0x76)) 
//    { 
//      Serial.println("Could not find a valid BME280 sensor, check wiring!"); 
//      while (1);
//    }
}

void loop()
{
//    // Send a heartbeat once every second.
//    if (millis() - _lastpacketSendTime > 995)
//    {
//        _lastpacketSendTime = millis();
//        sendBME280data();        
//    }

      // Show any received data.
    if (millis() - _lastpacketSendTime > 500)
    {
        _lastpacketSendTime = millis();
        getBME280data();      
    }
    delay (50);
    digitalWrite(RF_RX_LED, LOW);
}

void sendBME280data()
{
//    Serial.print("Sending BME280 : ");
//    RadioPacket radioData;
//    radioData.PacketType = BME280data;
//    radioData.T = bme.readTemperature();
//    radioData.H = bme.readHumidity();
//            String msg = "T = ";
//            msg += radioData.T;
//            msg += ", ";
//            msg += "H = ";
//            msg += radioData.H;
//    Serial.println (msg);
//    if (_radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData))) // 'send' puts the radio into Tx mode.
//    {
//        Serial.println("...Success");
//    }
////    else
////    {
////        Serial.println("...Failed");
////    }
}


void getBME280data()
{
    if (_radio.hasData()) // 'hasData' puts the radio into Rx mode.
    {   
        RadioPacket radioData;
        _radio.readData(&radioData);
        if (radioData.PacketType == BME280data)
        {   
            digitalWrite(RF_RX_LED, HIGH);
            String msg = "BME280data : ";
            msg += "Sensor ID : ";
            msg += radioData.SensorID;
            msg += ", ";
            msg += "T = ";
            msg += radioData.T;
            msg += ", ";
            msg += "H = ";
            msg += radioData.H;
            Serial.println(msg);
        }
        delay (5); 
    }
    else
    {
       Serial.println("No RF Rx");
    }
}
