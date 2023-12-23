
#ifndef LM35_h
#define LM35_h

#include <Arduino.h>
#include <math.h>

class LM35
{
public:
	LM35();
	LM35(int);
	int getTemp();
	int TempToCelsius(int temp);
	int getTempCelsius();
	void Print();
private:
	const float toDegrees = 0.48828125;//Value to change Temp adc to degrees
	int value;
	float temp;
	int tCelsius = 0;
};

#endif 
