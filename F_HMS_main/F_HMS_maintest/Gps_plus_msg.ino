
void serial_setup(){
  while (!Serial);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing...."));
  fonaSerial->begin(4800);
  if(! fona.begin(*fonaSerial)) {
  Serial.println(F("Couldn't find FONA"));
  while (1);
  }
}

void flushSerial() {
  while (Serial.available())
  Serial.read();
}

    
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

  while (Serial.available()) {
    char c =  Serial.read();

    //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

    if (c == '\r') continue;
    if (c == 0xA) {
      if (buffidx == 0)   // the first 0x0A is ignored
        continue;

      timeout = 0;         // the second 0x0A is the end of the line
      timeoutvalid = true;
      break;
    }
    buff[buffidx] = c;
    buffidx++;
  }

    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
}
  buff[buffidx] = 0;  // null term
  return buffidx;
}

void sendSos(){
  String message = "EMERGENCY ..... IS IN NEED OF ATTENTION, http://maps.google.com/maps?q=";
  flushSerial();
  Serial.print(F("Send to #"));
  //readline(sendto, 20);
  Serial.println(sendto);
  Serial.print(F("Type out one-line message (140 char): "));
  //readline(message, 140);
  message = message + slatitude + "," + slongitude;
  char sosMessage[141];
  strcpy(sosMessage,message.c_str());
  Serial.println(sosMessage);
  if (fona.sendSMS(sendto,sosMessage)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Sent!"));
    }
}

void gps_location(){
  delay(2000);
  float latitude = 0, longitude = 0, speed_kph, heading, speed_mph, altitude;

  // if you ask for an altitude reading, getGPS will return false if there isn't a 3D fix
  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

  if (gps_success) {

     Serial.print("GPS lat:");
     Serial.println(latitude, 6);
     Serial.print("GPS long:");
     Serial.println(longitude, 6);
     Serial.print("GPS speed KPH:");
     Serial.println(speed_kph);
     Serial.print("GPS speed MPH:");
     speed_mph = speed_kph * 0.621371192;
     Serial.println(speed_mph);
     Serial.print("GPS heading:");
     Serial.println(heading);
     Serial.print("GPS altitude:");
     Serial.println(altitude);
     slatitude = latitude/1.00000f;
     slongitude = longitude/1.00000f;
     sendSos();
     sentSMS = true;
  } 
  else {
     Serial.println("Waiting for FONA GPS 3D fix..."); 
  }  
 }

 void gps_fix(){
   int8_t stat;
   // check GPS fix
   stat = fona.GPSstatus();
   if (stat < 0)
   Serial.println(F("Failed to query"));
   if (stat == 0) Serial.println(F("GPS off"));
   if (stat == 1) Serial.println(F("No fix"));
   if (stat == 2) Serial.println(F("2D fix"));
   if (stat == 3) Serial.println(F("3D fix"));
 }

 void turn_gps_on(){
  // turn GPS on
  if (!fona.enableGPS(true))
    Serial.println(F("Failed to turn on"));
  }

 void network_status(){
    uint8_t n = fona.getNetworkStatus();
    Serial.print(F("Network status "));
    Serial.print(n);
    Serial.print(F(": "));
    if (n == 0) Serial.println(F("Not registered"));
    if (n == 1) Serial.println(F("Registered (home)"));
    if (n == 2) Serial.println(F("Not registered (searching)"));
    if (n == 3) Serial.println(F("Denied"));
    if (n == 4) Serial.println(F("Unknown"));
    if (n == 5) Serial.println(F("Registered roaming"));
 }

 void readAllSMSreq(){
        int8_t smsnum = fona.getNumSMS();
        uint16_t smslen;
        int8_t smsn;
        
        float latitude = 0, longitude = 0, speed_kph, heading, speed_mph, altitude;
      
        // if you ask for an altitude reading, getGPS will return false if there isn't a 3D fix
        boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);

        String message = "http://maps.google.com/maps?q=";
        
        if ( (type == FONA3G_A) || (type == FONA3G_E) ) {
          smsn = 0; // zero indexed
          smsnum--;
        } else {
          smsn = 1;  // 1 indexed
        }
       
        
        for ( ; smsn <= smsnum; smsn++) {
          Serial.print(F("\n\rReading SMS #")); Serial.println(smsn);
          if (!fona.readSMS(smsn, replybuffer, 250, &smslen)) {  // pass in buffer and max len!
            Serial.println(F("Failed!"));
            break;
          }
           if (smslen == 0) {
            Serial.println(F("[empty slot]"));
            smsnum++;
            continue;
          }

          Serial.print(F("***** SMS #")); Serial.print(smsn);
          Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****"));
          Serial.println(replybuffer);
          Serial.println(F("*****"));
        }
         String compare[15] = replybuffer;
        String location[15] = "location";
        int result;
         result = checker(compare,location);
          if(result == 0)
          {
            gps_fix();
            delay(2000);
            if (gps_success) {
          
              Serial.print("GPS lat:");
              Serial.println(latitude, 6);
              Serial.print("GPS long:");
              Serial.println(longitude, 6);
              Serial.print("GPS speed KPH:");
              Serial.println(speed_kph);
              Serial.print("GPS speed MPH:");
              speed_mph = speed_kph * 0.621371192;
              Serial.println(speed_mph);
              Serial.print("GPS heading:");
              Serial.println(heading);
              Serial.print("GPS altitude:");
              Serial.println(altitude);
              slatitude = latitude/1.00000f;
              slongitude = longitude/1.00000f;
              flushSerial();
              Serial.print(F("Send to #"));
              //readline(sendto, 20);
              Serial.println(smsn);
              Serial.print(F("Type out one-line message (140 char): "));
              //readline(message, 140);
              message = message + slatitude + "," + slongitude;
              char sosMessage[141];
              strcpy(sosMessage,message.c_str());
              Serial.println(sosMessage);
              if (fona.sendSMS(sendto,sosMessage)) {
              Serial.println(F("Failed"));
              } else {
              Serial.println(F("Sent!"));
              flushSerial();
              Serial.print(F("Delete #"));
              Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
              if (fona.deleteSMS(smsn)) {
                Serial.println(F("OK!"));
              } else {
                Serial.println(F("Couldn't delete"));
              }
            }
            } else {
              Serial.println("Waiting for FONA GPS 3D fix...");
            } 
          }
          else{
            flushSerial();
            Serial.print(F("Delete #"));
            Serial.print(F("\n\rDeleting SMS #")); Serial.println(smsn);
            if (fona.deleteSMS(smsn)) {
              Serial.println(F("OK!"));
            } else {
              Serial.println(F("Couldn't delete"));
        }
          }  
     }

 int checker(String compare[],String location[])
{
    int i,result=1;
    for(i=0; compare[i]!='\0' || location[i]!='\0'; i++) {
        if(compare[i] != location[i]) {
            result=0;
            break;
        }
    }
    return result;
}
