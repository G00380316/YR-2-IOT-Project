#include <LiquidCrystal.h>
#include <Arduino.h>
#include <math.h>
#include "LM35.h"

LM35::LM35()
{
	temp = 20;
	value = A1;
}

LM35::LM35(int val)
{
	temp = 20;
	value = val;
}

int LM35::getTemp()
{
	int cdegrees = analogRead(value);
	tCelsius = TempToCelsius(cdegrees);
}

int LM35::TempToCelsius(int cdegrees)
{
	int output = cdegrees * toDegrees;
	return output;
}

void LM35::Print()
{
  const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
  LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
	Serial.print("Temperature = ");
	Serial.print(tCelsius);
	Serial.print("\n");
  lcd.begin(16, 2);
  lcd.print("Temperature= ");
  lcd.print(tCelsius);
  lcd.setCursor(0,1);
  lcd.print("Wait for Green");
  delay(150);
}

int LM35::getTempCelsius()
{
	return tCelsius;
}
