
/*  LCD RS to digital pin 7
 *  LCD Enable pin to digital pin 8
 *  LCD D4 pin to digital pin 9
 *  LCD D5 pin to digital pin 10
 *  LCD D6 pin to digital pin 11
 *  LCD D7 pin to digital pin 12
 *  LCD R/W pin to ground
 *  LCD VSS pin to ground
 *  LCD VCC pin to 5V
 *  10K resistor:
 *  ends to +5V and ground
 *  wiper to LCD V0 pin (pin 3)  
 */

# include <LiquidCrystal.h>

LiquidCrystal lcd(7,8,9,10,11,12);

float A = 0.7; // global variables for the low pass filter. A + B = 1. When A is higher the filter is more restrictive. 
float B = 0.3;
int dir_pot_pin = A4;         //assigns the direction potentiometer to analog pin A4
int ang_pot_pin = A5;         //assigns the angle potentiometer to analog pin A5
int sample_size = 10;
int sum_vals = 0;
int avg_val = 0;
int old_dir_mapped = 0;
int old_ang_mapped = 0;

int dir_read = 0;         //filtered reading for direction potentiometer

int ang_read = 0;         //filtered reading for angle potentiometer
int ang_max = 16;         //maximum value possible for the angle off vertical
int dir_max = 360;         //maximum value possible for the direction off North
int dir_mapped;         //mapped value for direction, ready for output
int ang_mapped;         //mapped value for angle, ready for output
int dir_output = 0;
int ang_output = 0;


void setup() {
  //setup the LCD's number of columns and rows:
  lcd.begin(16,2);
  Serial.begin(9600); //used for debugging
}

// This function makes an array of readings from the sensor, then returns the average.
int pot_rf(int pot_pin){
  int i = 0;
  sum_vals = 0;
  avg_val = 0;
  while (i <= sample_size -1) {          //Loop while you could up 0-sample_size
    sum_vals = sum_vals + analogRead(pot_pin);
    i = i + 1;
  }
  avg_val = sum_vals/sample_size;
  return avg_val;
}

// This funciton maps the potentiometer readings to the desired range.
int pot_map(int pot_val, int max_val){
  int mapped_val = map(pot_val, 0, 1021, 0, max_val);
  return mapped_val;
}

//This function steadies the values. It returns a new reading ONLY when the new reading has 
//changed by more than +/1 from the old reading.
int steady_val(int new_reading, int old_reading){
  if ((new_reading > old_reading + 1) || (new_reading < old_reading - 1)){
    old_reading = new_reading;
  }
  return old_reading;
}

//This function prints out processed values for desired direction and angle to the LCD screen.
void printVals(int pdir_read, int pang_read){

  lcd.clear();          //clears the lcd screen of any previous printouts
  lcd.setCursor(0,0);         //starts the cursor line 1, space 1
  lcd.print("Deg off N   ");          //next two lines, prints the degrees direction off of North, ccw
  lcd.print(pdir_read);
  lcd.setCursor(0,1);         //moves the cursor to line 2, space 1
  lcd.print("Angle   ");          //next two lines, prints the degrees off vertical
  lcd.print(pang_read);
}

void loop() {

  dir_read = pot_rf(dir_pot_pin);
  ang_read = pot_rf(ang_pot_pin);  
  dir_mapped = pot_map(dir_read, dir_max);
  ang_mapped = pot_map(ang_read, ang_max);

  dir_output = steady_val(dir_mapped, old_dir_mapped);
  ang_output = steady_val(ang_mapped, old_ang_mapped);

  old_dir_mapped = dir_output;
  
  printVals(dir_output, ang_output);
  delay(200);         //delay of 200 msec, to allow for steadier printouts

}
