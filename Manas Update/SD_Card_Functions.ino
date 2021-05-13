void logstring()
{
  dataString = "";
  dataString += timevalue + ',' + TVal + ',' + HVal;
  Serial.println ( dataString);
}

void get_timestring()
{
  rtc.update();
  timevalue = "";
  timevalue += (String)rtc.date() + '-';
  timevalue += (String)rtc.month(); timevalue += '-';
  timevalue += (String)rtc.year(); timevalue += '-';
  timevalue += (String)rtc.hour(); timevalue += ':';
  if (rtc.minute() < 10) {
    timevalue += '0';
  }
  timevalue += (String)rtc.minute(); timevalue += ':';
  if (rtc.second() < 10) {
    timevalue += '0';
  }
  timevalue += (String)rtc.second();
  if (rtc.is12Hour()) // If RTC is in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) timevalue += (":PM"); // Returns true if PM
    else timevalue += (":AM");
  }
  return timevalue;
}

void add2log()
{
  get_timestring();
  logstring();
  dataFile = SD.open(FName, FILE_WRITE);
  Serial.print("Datafile : "); Serial.println(dataFile);
  Serial.print("Vals: "); Serial.print(temp); Serial.print("  "); Serial.println(humidity);
  if (dataFile) // record if data file exists
  {
    dataFile.println(dataString);
    dataFile.close();
    // Serial.println(dataString);
    logfail = 0;
  }
  else // error if data file has issue
  {
    logfail = 1;
    //Serial.println("DataFile Error !!!");
  }
}

void get_FileName()
{
  int temp1 = 0;

  String tempname = "";
  tempname += (String)rtc.date(); //max 2 digits
  //tempname += '-';
  tempname += (String)rtc.month(); //max 2 digits
  //tempname += '-';
  tempname += (String)rtc.year();  // mac 2 digits
  //tempname += '-';

  for (int i = 0; i < tempname.length(); i++)
  {
    FileName [i] = tempname[i];
    temp1 = i;
  }
  Serial.println(temp1);
  FileName[temp1 + 1] = '0';
  FileName[temp1 + 2] = '0';
  FileName[temp1 + 3] = '.';
  FileName[temp1 + 4] = 'c';
  FileName[temp1 + 5] = 's';
  FileName[temp1 + 6] = 'v';

  for (int i = 0; i < 100; i++)
  {
    FileName[temp1 + 1] = (i / 10) % 10 + '0';
    FileName[temp1 + 2] = i % 10 + '0';

    Serial.print("Searching for ");
    Serial.println(FileName);

    if (!SD.exists(FileName))
    {
      Serial.print("Chosen Name: ");
      Serial.println(FileName);

      dataFile = SD.open(FileName, FILE_WRITE);

      if (dataFile) {
        Serial.println("File Created");
        FName = FileName;
      }
      else {
        Serial.println("File Create Error");
      }

      dataFile.close();
      return;
    }
    else
    {
      Serial.print(FileName);
      Serial.println(" already exists!");
    }
  }
}

void new_set()
{
  dataFile = SD.open(FileName, FILE_WRITE);
  if (dataFile) {
    Serial.println("File Opened");
  }
  else {
    Serial.println("File Open Error");
  }
  dataFile.println("-----------New Data Starts Here-------------");
  dataFile.print("System Reference ID - ");
  dataFile.println(SerialNo);
  for (int i = 0; i < 3; i++)
  {
    dataFile.print(Header[i]);
    dataFile.print(",");
  }
  dataFile.println();
  dataFile.close();
  Serial.println("Header added to file");
}

void wipe_sd()
{
  int temp1 = 0;

  String tempname = "";
  tempname += (String)rtc.date(); //max 2 digits
  //tempname += '-';
  tempname += (String)rtc.month(); //max 2 digits
  //tempname += '-';
  tempname += (String)rtc.year();  // mac 2 digits
  //tempname += '-';

  for (int i = 0; i < tempname.length(); i++)
  {
    FileName [i] = tempname[i];
    temp1 = i;
  }
  Serial.println(temp1);
  FileName[temp1 + 1] = '0';
  FileName[temp1 + 2] = '0';
  FileName[temp1 + 3] = '.';
  FileName[temp1 + 4] = 'c';
  FileName[temp1 + 5] = 's';
  FileName[temp1 + 6] = 'v';

  for (int i = 0; i < 100; i++)
  {
    FileName[temp1 + 1] = (i / 10) % 10 + '0';
    FileName[temp1 + 2] = i % 10 + '0';

    Serial.print("Searching for ");
    Serial.println(FileName);

    if (SD.exists(FileName))
    {
      Serial.print("Found :- ");
      Serial.println(FileName);
      Serial.println("Removing NOW");
      SD.remove(FileName);
    }
    else
    {
      Serial.print(FileName);
      Serial.println(" already removed!");
    }
  }
}

void rm(File dir, String tempPath)
{
  while (true) {
    File entry =  dir.openNextFile();
    String localPath;

    Serial.println("");
    if (entry) {
      if ( entry.isDirectory() )
      {
        localPath = tempPath + entry.name() + rootpath + '\0';
        char folderBuf[localPath.length()];
        localPath.toCharArray(folderBuf, localPath.length() );
        rm(entry, folderBuf);

        if ( SD.rmdir( folderBuf ) )
        {
          Serial.print("Deleted folder ");
          Serial.println(folderBuf);
          FolderDeleteCount++;
        }
        else
        {
          Serial.print("Unable to delete folder ");
          Serial.println(folderBuf);
          FailCount++;
        }
      }
      else
      {
        localPath = tempPath + entry.name() + '\0';
        char charBuf[localPath.length()];
        localPath.toCharArray(charBuf, localPath.length() );

        if ( SD.remove( charBuf ) )
        {
          Serial.print("Deleted ");
          Serial.println(localPath);
          DeletedCount++;
        }
        else
        {
          Serial.print("Failed to delete ");
          Serial.println(localPath);
          FailCount++;
        }

      }
    }
    else {
      // break out of recursion
      break;
    }
  }
}
