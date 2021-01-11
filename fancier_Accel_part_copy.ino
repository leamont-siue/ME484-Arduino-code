/*
    Arduino and ADXL345 Accelerometer Tutorial
     by Dejan, https://howtomechatronics.com
*/
// Rewritten by Leanne Montgomery for Self-leveling rocket

#include <Wire.h>  // Wire library - used for I2C communication
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Stepper.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
int x_read, y_read, z_read;  // Readings from the accelerometer in x, y, z
int low_x = -270;         // Lowest static value output for x (ACC1)
int high_x = 310;         // Highest static value output for x (ACC1)
//  int low_y = -269.0;         // Lowest static value output for y (ACC1), y not needed in this app
//  int high_y = 310.0;         // Highest static value output for y (ACC1), y not needed in this app
int low_z = -254;         // Lowest static value output for z (ACC1)
int high_z = 264;         // Highest static value output for z (ACC1)

int x_range = (high_x - low_x)/2;         // Range is the distance from "zero" to the maximum values.
//  int y_range = (high_y - low_y)/2;         // y not needed in this app
int z_range = (high_z - low_z)/2;
int x_zero = x_range + low_x;         // Zeroes are at the center of the range.
//  int y_zero = y_range + low_y;         // y not needed in this app
int z_zero = z_range + low_z;
int x_out, y_out, z_out;          // Adjusted values for accelerometer readings, on a scale of -1 to 1.

int steps_per_rev = 2048;         //Total steps in one revolution for this motor.
int rev_per_min = 15;         // (Adjustable) speed for stepper motor. Range is particular to motor.
int xloc_in_range = 0;
int xsteps_needed = 0;
Stepper myStepper(steps_per_rev, 3, 5, 4, 6);

void setup() {
 
  Serial.begin(9600); // Initiate serial communication for printing on Serial monitor
  
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
  myStepper.setSpeed(rev_per_min);

  if(!accel.begin()){
    Serial.println("No accelerometer detected.");
    while(1);
  }
}

int read_steps_necessary(int axis, int steps_rev, int axis_range){
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);           // Read 6 registers total, each axis value is stored in 2 registers
  int x_read = ( Wire.read()| Wire.read() << 8);          // Raw x-value
  int x_loc_in_range = (x_zero-x_read);          // distance (to travel) to get to "0" of the x-range
  int y_read = ( Wire.read()| Wire.read() << 8);          // Raw y-value
//  y_out = (y_read - y_zero)/y_range;          // distance (to travel) to get to center of the x-range; y not necessary in this app
  int z_read = ( Wire.read()| Wire.read() << 8);          // Raw z-value
  int z_loc_in_range = (z_zero-z_read);          // distance (to travel) to get to center of the x-range
  
  Serial.print("X= ");
  Serial.print(x_read);
  Serial.print("   Y= ");
  Serial.print(y_read);
  Serial.print("   Z= ");
  Serial.println(z_read);
  
  if (axis == 1){
    Serial.print("X distance from zero: ");
    Serial.println(x_loc_in_range);
    int steps_needed = x_loc_in_range * (steps_rev/axis_range);
    return steps_needed;
  }
  else if (axis == 3){
    Serial.print("Z distance from zero: ");
    Serial.println(z_loc_in_range);
    int steps_needed = z_loc_in_range * (steps_rev/axis_range);
    return steps_needed;
  }  
}


void loop() {
  int x_steps_needed = read_steps_necessary(1, steps_per_rev, x_range);
  Serial.print("Taking ");
  Serial.print(x_steps_needed);
  Serial.println(" steps now.");
  myStepper.step(x_steps_needed);
  delay(1000);
  
  
}
