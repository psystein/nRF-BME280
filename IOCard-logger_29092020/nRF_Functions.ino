
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
      TVal = radioData.T;
      msg += ", ";
      msg += "H = ";
      msg += radioData.H;
      HVal = radioData.H;
      Enter2Log = 1; // data Entry to be made into the Log
    }
    delay (5);
  }
  else
  {
    Serial.println("No RF Rx");
    Enter2Log = 0;  // NO Data Entry to be made into the Log
  }
}
