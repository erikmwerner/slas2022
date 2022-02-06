/*
  Rolling Stats

  Reads an analog input on pin 0 and computes stats using Welford's method

  This example sketch has been modified from the example code at:
  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/
#include <SLAS2022.h>

WelfordStats stats;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0
  int sensor_value = analogRead(A0);

  stats.update(sensor_value);

  // scale the values to lux (0-400)
  //double current_lux = mapDouble(sensor_value, 11, 926, 0.0, 400.0); 
  //double average_lux = mapDouble(average_value, 11, 926, 0.0, 400.0);
  Serial.print(sensor_value); // print out the value just read:
  Serial.print(','); // separate fields with a comma
  Serial.print(stats.mean()); // print out the value just read:
  Serial.print(','); // separate fields with a comma
  Serial.print(stats.mean() + stats.variance());
  Serial.print(','); // separate fields with a comma
  Serial.println(stats.mean() - stats.variance());
  
  int delayTimeMs = 10; // 10ms delay ~= 100 Hz sampling
  delay(delayTimeMs);   // delay in between reads for stability
}

// a function to compute the average of the values in an array
// note: count must be <= the number of elements in the array
double average(int buffer[], const uint8_t count) {
  double average = 0;
  for(uint8_t i = 0; i < count; ++i) {
    average += buffer[i];
  }
  average = average / (double)count;
  return average;
}

// a function to map values with double precision (the standard map uses integers)
double mapDouble(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}