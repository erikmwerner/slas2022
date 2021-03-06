/*
  Sliding Window Filter

  Reads an analog input on pin 0, stores data in an array, and computes the average of
  all the data points. Results are printed to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  
  Attach the center node of a voltage divider circuit to pin A0, and the outside nodes to +3.3V and ground.
  
  This example adds a sliding window filter
  
  Updated: 2020 01 26

  This example sketch has been modified from the example code at:
  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

//< the width of the sliding window
const uint8_t window_size = 16;

//< a buffer to hold values for filtering
int buffer[window_size] = {0};

//< an integer to remember the index of the buffer
uint8_t buffer_index = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  delay(100);
  
  Serial.print("Sample [lux]"); // print out the value just read:
  Serial.print(','); // separate fields with a comma
  Serial.println("Average [lux]");
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0
  int sensor_value = analogRead(A0);
  
  // store the new reading in the buffer
  buffer[buffer_index] = sensor_value;
  buffer_index++;
  
  // buffer_index now points to the next open slot in the array
  // reset to zero if we have filled the array
  // index is offset by one because array is zero-indexed
  if(buffer_index - 1  > window_size) {
    buffer_index = 0;
  }

  // compute the average of the values stored in the buffer
  double average_value = average(buffer, window_size);

  // scale the values to lux (0-400)
  double current_lux = mapDouble(sensor_value, 11, 926, 0.0, 400.0); 
  double average_lux = mapDouble(average_value, 11, 926, 0.0, 400.0);
  
  Serial.print(current_lux); // print out the value just read:
  Serial.print(','); // separate fields with a comma
  Serial.println(average_lux);
  
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
