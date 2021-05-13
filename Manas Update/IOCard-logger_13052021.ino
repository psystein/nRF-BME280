

/*
  Radio    Arduino
  CE    -> 38
  CSN   -> 53 (Hardware SPI SS)
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
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
#include <SparkFunDS3234RTC.h>
#include <SD.h>
#include <Nextion.h>

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;
const static uint8_t PIN_RADIO_CE = 38;    // For CDSPL IO CARD
const static uint8_t PIN_RADIO_CSN = 53;   // FOR CDSPL IO CARD
const static uint8_t RF_RX_LED = 13;       // User LED on IO Card

char FileName[13];
String FName;
File dataFile;
File root;
String SerialNo = "RF_Sensor_Logger : 001";
String Header[] = {"Date and Time" , "TEMPERATURE" , "HUMIDITY",};
String rootpath = "/";
int DeletedCount = 0; int FolderDeleteCount = 0; int FailCount = 0; // Variable for Deleting from Logging Disk
String timevalue = ""; String TVal = ""; String HVal = ""; String dataString = ""; // Variable for Datalogging
bool Enter2Log = 0; int logfail = 0;
int temp = 0, humidity = 0;

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

// #define SEALEVELPRESSURE_HPA (1013.25)
// Adafruit_BME280 bme;

void setup()
{
  Serial.begin(115200);
  Serial.print("Initializing SD card...");
  if (!SD.begin(12)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  nexInit();
  rtc.begin(39);
  delay(1000);
  rtc.autoTime();
  //delay(1000);
  rtc.update();

  pinMode(RF_RX_LED, OUTPUT);

  get_FileName();
  new_set();
  Serial.println("Datalogging Card initialized.");

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
    Serial.println("Cannot communicate with RF");
    while (1);
  }
  Serial.println("RF Comm. Listener Initialized");
}
unsigned long lastGraph = 0;
bool rxed = 0, graphed = 0;
uint8_t tempG = 0, humidityG = 0;
void loop()
{
  //    // Send a heartbeat once every second.
  //    if (millis() - _lastpacketSendTime > 995)
  //    {
  //        _lastpacketSendTime = millis();
  //        sendBME280data();
  //    }

  // Show any received data.
  if (millis() - lastGraph >= 288000) {
    if (rxed && !graphed) {
      tempG = map(temp, 0, 50000, 0, 255);
      humidityG = map(humidity, 0, 10000, 0, 255);
      char buf[15] = {0};
      lastGraph = millis();
      sprintf(buf, "add 1,0,%u", tempG);
      sendCommand(buf);
      buf[15] = {0};
      sprintf(buf, "add 2,0,%u", humidityG);
      sendCommand(buf);
      buf[15] = {0};
      graphed = 1;
      rxed = 0;
    }
  }
  if (millis() - _lastpacketSendTime > 500)
  {
    _lastpacketSendTime = millis();
    getBME280data();
    if (Enter2Log)
    {
      add2log();
      Enter2Log = 0;
    }
  }
  delay (50);
  digitalWrite(RF_RX_LED, LOW);
}
