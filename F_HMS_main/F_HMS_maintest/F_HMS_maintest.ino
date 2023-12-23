#include <TimerOne.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "LM35.h"
#include "Adafruit_FONA.h"

#define FONA_RX 50
#define FONA_TX 51
#define FONA_RST 20
#define Led 42
#define redLed 31

volatile boolean ledOn = false; 
volatile int cycle = 0;

boolean sentSMS = false;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
const int PIN = A1;
const byte RATE_SIZE = 5;
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
int i = 0;
int sosCounter=0;
int cCelsius;
char c;
char sendto[21] = "0852830482";
String slatitude,slongitude;
char replybuffer[255];
uint8_t type;


LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
LM35 finalLM35(PIN);
MAX30105 particleSensor;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  pinMode(Led,OUTPUT);
  Timer1.initialize(500000000);
  Timer1.attachInterrupt(Timer);
  if(!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  serial_setup();
  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case FONA800L:
      Serial.println(F("FONA 800L")); break;
    case FONA800H:
      Serial.println(F("FONA 800H")); break;
    case FONA808_V1:
      Serial.println(F("FONA 808 (v1)")); break;
    case FONA808_V2:
      Serial.println(F("FONA 808 (v2)")); break;
    case FONA3G_A:
      Serial.println(F("FONA 3G (American)")); break;
    case FONA3G_E:
      Serial.println(F("FONA 3G (European)")); break;
    default: 
      Serial.println(F("???")); break;
  }
  network_status();
  turn_gps_on();
}

void loop() {
  // put your main code here, to run repeatedly
 while(beatAvg<120&&cCelsius<38){
  if(cycle == 0){
    instructions();
    delay(150);
  }

  finalLM35.getTemp();
  cCelsius = finalLM35.getTempCelsius();
  if(cycle == 1){
  finalLM35.Print();
  }

 
  const unsigned long eventTime_1_LCD = 10000;
  unsigned long previousTime_1 = 0;
  long irValue = particleSensor.getIR();
  
  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  unsigned long currentTime = millis();
   if(cycle == 2){
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  if((currentTime - previousTime_1) >= eventTime_1_LCD ){
  LCDprintbpm();
  previousTime_1 = currentTime;
  }
  if (irValue < 50000){
    delay(200);
    Serial.print(" No finger?");
    }
  Serial.println();
  Serial.println();
  }
 }
 while(sosCounter<=20)
 {
 digitalWrite(Led,LOW);
 lcd.begin(16,2);
 lcd.clear();
 lcd.print("Attention needed");
 lcd.setCursor(0,1);
 lcd.print("SOS been sent ");
 while(sentSMS == false)
 {
 gps_fix();
 gps_location();
 }
 sosCounter++;
 digitalWrite(redLed,HIGH);
 if(sosCounter >= 15){
 digitalWrite(redLed,LOW);
 }
 }
 cCelsius = 0;
 beatAvg = 0;
 sosCounter = 0;
}
