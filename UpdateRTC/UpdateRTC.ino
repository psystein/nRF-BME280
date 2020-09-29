#include <SparkFunDS3234RTC.h>

#include <SPI.h>

void setup() {
 Serial.begin(9600);
  rtc.begin(39);
  rtc.autoTime();
  delay(1000);
  Serial.println ("DONE");
  rtc.update();
  String tempname = String(rtc.date());
    tempname += "_";
    tempname += String(rtc.month());
    tempname += "_";
    tempname += String(rtc.year());
    tempname += "_";
    tempname += String(rtc.hour());
    tempname += "_";
    tempname += String(rtc.minute());
    tempname += "_";
    tempname += String(rtc.second());

    Serial.println (tempname);
    
}

void loop() {
  // put your main code here, to run repeatedly:

}
