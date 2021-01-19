/*
  Radio    Arduino
  CE    -> 7
  CSN   -> 8 (Hardware SPI SS)
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

  VCC to BME and nRF given from the DO of the AT328P
*/

#include "watchdogHandler.h"
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <SPI.h>
#include <NRFLite.h>

volatile unsigned long secondsLastUpdate = 0;
unsigned long reqlogintervals = 30; // enter value in Sec - interval between two log entires
unsigned long secondsUpdateFrequency = ((reqlogintervals / 8) * 8); // - 5;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

const static uint8_t RADIO_ID = 11;
const static uint8_t DESTINATION_RADIO_ID = 10;
const static uint8_t PIN_RADIO_CE = 7;
const static uint8_t PIN_RADIO_CSN = 8;

int nRFvcc = 2;     // DO2 for VCC for the nRF
int BMEvcc = 3;     // DO3 for VCC for the BME
int bootT = 00;    // Delay for Boot up of NRF and BME
bool initreq = 0;   // If this is true the BME and nRF need to be Initialized

NRFLite radio;

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





void setup()
{ 
  wdt_disable(); //  Disable the watchdog timer first thing, in case it is misconfigured
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(nRFvcc, OUTPUT); // VCC to NRF controlled using the D2 pin
  pinMode(BMEvcc, OUTPUT); // VCC to BME controlled using the D3 pin
  digitalWrite (nRFvcc, HIGH);  // Turn on the PS to the NRF for Initialization
  digitalWrite (BMEvcc, HIGH);  // Turn on the PS to the BME for Initialization
  delay (bootT); // BootUp Delay for NRF & BME
  Serial.begin(115200);
  Serial.println("");
  Serial.print ("Sensor ID : ");
  Serial.println(RADIO_ID);
  Serial.println("");
  Serial.print ("Required Logging Interval (sec) : ");
  Serial.println(reqlogintervals);
  Serial.println("");
  Serial.print ("Update Frequency (sec) : ");
  Serial.println(secondsUpdateFrequency);
  Serial.println("");
  if (!radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) // check RF Module Connection
  {
    Serial.println("Cannot communicate with RF");
    while (1);
  }
  if (!bme.begin(0x76)) // check Sensor Module Connection
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  delay (500);
  setup_watchdog(WDTO_8S); //  Run ISR(WDT_vect) after configured delay
  // Valid delays:
  //   WDTO_15MS  //  WDTO_30MS  //  WDTO_60MS  //  WDTO_120MS //  WDTO_250MS  //  WDTO_500MS
  //  WDTO_1S  //  WDTO_2S  //  WDTO_4S  //  WDTO_8S

  sendBME280data();  // Send first Update
  secondsLastUpdate = 0;  // Update the time at which Data was sent
}

void loop()
{
  // Send a heartbeat once every second.
  //if (millis() - _lastpacketSendTime > _packetSendIntvl)
  if ( secondsLastUpdate >= secondsUpdateFrequency)
  {
    // digitalWrite (nRFvcc, HIGH);  // Turn on the PS to the NRF for Operation
    // digitalWrite (BMEvcc, HIGH);  // Turn on the PS to the BME for Operation
    digitalWrite(LED_BUILTIN, HIGH);
    //delay (bootT); // BootUp Delay for NRF & BME
    sendBME280data();
    secondsLastUpdate = 0;
  }

  sleeping();  //  Go to sleep forever... or until the watchdog timer fires!

  // //     Show any received data.
  //        if (millis() - _lastpacketSendTime > 1000)
  //    {
  //        _lastpacketSendTime = millis();
  //        getBME280data();
  //    }
  //    delay (50);
}

ISR(WDT_vect)
{
  //  Anything you use in here needs to be declared with "volatile" keyword
  //  Track the passage of time.
  secondsLastUpdate = secondsLastUpdate + 8;
  // Serial.println("awake now");
  Serial.print("Sec since last update : ");
  Serial.println(secondsLastUpdate);
  delay (5);
}

void sleeping()
{
  // Serial.println("Going to sleep");
  delay (10);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  waking();
}

void waking()
{
  //  You could wake up your external sensors & hardware here
}


void sendBME280data()
{
  //if (initreq){ // need to initialize the BME and nRF
  //radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN);
  //bme.begin(0x76); }
  Serial.print("Sending BME280 : ");
  RadioPacket radioData;
  radioData.PacketType = BME280data;
  radioData.SensorID = RADIO_ID;
  radioData.T = bme.readTemperature();
  radioData.H = bme.readHumidity();
  String msg = "Sensor ID : ";
  msg += radioData.SensorID;
  msg += ", ";
  msg += "T = ";
  msg += radioData.T;
  msg += ", ";
  msg += "H = ";
  msg += radioData.H;
  Serial.print (msg);
  if (!radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData))) // 'send' puts the radio into Tx mode.
  {
    Serial.println("...Sent");
  }
      else
      {
          Serial.println("...Failed");
      }
  digitalWrite(LED_BUILTIN, LOW);
  // delay (bootT); // BootUp Delay for NRF & BME
  // digitalWrite (nRFvcc, LOW);  // Turn oFF the PS to the NRF for Power Save
  // digitalWrite (BMEvcc, LOW);  // Turn oFF the PS to the BME for Power Save
  initreq = 0;   // If this is true the BME and nRF need to be Initialized 
}

void getBME280data()
{
  if (radio.hasData()) // 'hasData' puts the radio into Rx mode.
  {
    RadioPacket radioData;
    radio.readData(&radioData);
    if (radioData.PacketType == BME280data)
    {
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
