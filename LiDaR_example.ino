/* Name: LiDar example
 * Creator: Pololu
 * Modified by: Pascal Urban
 * Last modified on Date: 11.01.2020
 * Description: Basic script to read the data of two VL53L1X sensors 
 *              and calculate the average value. 
 *              It's designed to run a NodeMCU ESP32.
*/

// needed libraries
#include <Wire.h>
#include <VL53L1X.h>

// Ports connected to XSHUT pin of LiDaR
// used to reset the sensors
#define XSHUT1 D3
#define XSHUT2 D8

// create instance of sensor class
VL53L1X sensor1;
VL53L1X sensor2;

// calculated average value
// used to smooth the values
float avgvalue1 = 0;
float avgvalue2 = 0;

// number of the iterations needed to calculate average
int n = 2^15-1;


void setup()
{
  // setting up serial connection for debugging
  Serial.begin(115200);
  // I2C to LiDaRs
  Wire.begin(D1, D2);
  Wire.setClock(400000); // use 400 kHz I2C

  // pull down XSHUT to reset the sensor
  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);
  digitalWrite(XSHUT1, LOW);
  digitalWrite(XSHUT2, LOW);

  // -----------------------
  // start setup of sensor 1
  // -----------------------

  sensor1.setTimeout(500);
  
  // deactivate XSHUT to enable sensor 1
  digitalWrite(XSHUT1, HIGH);

  // initialize
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor 1!");
    while (1);
  }

  // setup distance mode
  sensor1.setDistanceMode(VL53L1X::Long);

  // setup Measurement Timing Budget
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes.
  sensor1.setMeasurementTimingBudget(50000);


  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor1.startContinuous(500);

  // change the address of sensor 1
  // needed to use more then one sensor
  sensor1.setAddress(42);

  // -----------------------
  // start setup of sensor 2
  // -----------------------
  //simular to sensor 
  
  sensor2.setTimeout(500);
  digitalWrite(XSHUT2, HIGH);
  
  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor 2!");
    while (1);
  };
  sensor2.setDistanceMode(VL53L1X::Long);
  sensor2.setMeasurementTimingBudget(50000);
  sensor1.setAddress(43);
  sensor2.startContinuous(500);
}

void loop()
{
  // start the reading process
  sensor1.read();
  sensor2.read();

  // initial value
  if(avgvalue1 == 0){
    avgvalue1 = float(sensor1.ranging_data.range_mm);
  }

  // smooth the values
  avgvalue1 = (avgvalue1*(n-1) + float(sensor1.ranging_data.range_mm)) / n;
  avgvalue2 = (avgvalue2*(n-1) + float(sensor2.ranging_data.range_mm)) / n;


  // print the values on a serial console
  Serial.print("Sensor 2: ");
  Serial.print("range: ");
  Serial.print(sensor1.ranging_data.range_mm);
  Serial.print(" - ");
  Serial.print(" average: ");
  Serial.print(avgvalue1);
  Serial.print(" - ");
  Serial.print(" ambient: ");
  Serial.print(sensor1.ranging_data.ambient_count_rate_MCPS);
  Serial.println();
  
  Serial.print("Sensor 2: ");
  Serial.print("range: ");
  Serial.print(sensor2.ranging_data.range_mm);
  Serial.print(" - ");
  Serial.print(" average: ");
  Serial.print(avgvalue2);
  Serial.print(" - ");
  Serial.print(" ambient: ");
  Serial.print(sensor2.ranging_data.ambient_count_rate_MCPS);
  Serial.println();
}
