/* Name: ambient
 * Creator: Pololu
 * Modified by: Pascal Urban
 * Last modified on Date: 11.01.2020
 * Description: Basic script to read the data of three VL53L1X sensors 
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
#define XSHUT3 D5

#define AMBIENT_THRESHOLD 1

// create instance of sensor class
VL53L1X sensor1;
VL53L1X sensor2;
VL53L1X sensor3;

// calculated average value
// used to smooth the values
float avgvalue1_low = 0;
float avgvalue2_low = 0;
float avgvalue3_low = 0;

float avgvalue1_high = 0;
float avgvalue2_high = 0;
float avgvalue3_high = 0;

float avg_ambient1 = 0;
float avg_ambient2 = 0;
float avg_ambient3 = 0;

// number of the iterations needed to calculate average
int n = 2^15-1;

int count = 0;


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
  pinMode(XSHUT3, OUTPUT);
  digitalWrite(XSHUT1, LOW);
  digitalWrite(XSHUT2, LOW);
  digitalWrite(XSHUT3, LOW);

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
  sensor1.setDistanceMode(VL53L1X::Medium);

  // setup Measurement Timing Budget
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes.
  sensor1.setMeasurementTimingBudget(50000);


  // Start continuous readings at a rate of one measurement every 500 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor1.startContinuous(50);

  // change the address of sensor 1
  // needed to use more then one sensor
  sensor1.setAddress(42);

  // -----------------------
  // start setup of sensor 2
  // -----------------------
  //similar to sensor 
  
  sensor2.setTimeout(500);
  digitalWrite(XSHUT2, HIGH);
  
  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor 2!");
    while (1);
  };
  sensor2.setDistanceMode(VL53L1X::Medium);
  sensor2.setMeasurementTimingBudget(50000);
  sensor2.setAddress(43);
  sensor2.startContinuous(50);

    // -----------------------
  // start setup of sensor 3
  // -----------------------
  //similar to sensor 
  
  sensor3.setTimeout(500);
  digitalWrite(XSHUT3, HIGH);
  
  if (!sensor3.init())
  {
    Serial.println("Failed to detect and initialize sensor 2!");
    while (1);
  };
  sensor3.setDistanceMode(VL53L1X::Long);
  sensor3.setMeasurementTimingBudget(50000);
  sensor3.setAddress(44);
  sensor3.startContinuous(50);
}

void loop()
{
  // start the reading process
  sensor1.read();
  sensor2.read();
  sensor3.read();
  
  avg_ambient1 = (avg_ambient1*(n-1) + float(sensor1.ranging_data.ambient_count_rate_MCPS)) / n;
  avg_ambient2 = (avg_ambient2*(n-1) + float(sensor2.ranging_data.ambient_count_rate_MCPS)) / n;
  avg_ambient3 = (avg_ambient3*(n-1) + float(sensor3.ranging_data.ambient_count_rate_MCPS)) / n;

  if (sensor1.ranging_data.ambient_count_rate_MCPS > AMBIENT_THRESHOLD){
    if (avgvalue1_high == 0)
      avgvalue1_high = sensor1.ranging_data.range_mm;
    avgvalue1_high = (avgvalue1_high*(n-1) + float(sensor1.ranging_data.range_mm)) / n;
  }
  else {
    if (avgvalue1_low == 0)
      avgvalue1_low = sensor1.ranging_data.range_mm;
    avgvalue1_low = (avgvalue1_low*(n-1) + float(sensor1.ranging_data.range_mm)) / n;
  }
    
  if (sensor2.ranging_data.ambient_count_rate_MCPS > AMBIENT_THRESHOLD){
    if (avgvalue2_high == 0)
      avgvalue2_high = sensor1.ranging_data.range_mm;
    avgvalue2_high = (avgvalue2_high*(n-1) + float(sensor2.ranging_data.range_mm)) / n;
  }
  else{
    if (avgvalue2_low == 0)
      avgvalue2_low = sensor1.ranging_data.range_mm;
    avgvalue2_low = (avgvalue2_low*(n-1) + float(sensor2.ranging_data.range_mm)) / n;
  }

  if (sensor3.ranging_data.ambient_count_rate_MCPS > AMBIENT_THRESHOLD){
    if (avgvalue3_high == 0)
      avgvalue3_high = sensor1.ranging_data.range_mm;
    avgvalue3_high = (avgvalue3_high*(n-1) + float(sensor3.ranging_data.range_mm)) / n;
    }
  else{
    if (avgvalue3_high == 0)
      avgvalue3_high = sensor1.ranging_data.range_mm;
    avgvalue3_low = (avgvalue3_low*(n-1) + float(sensor3.ranging_data.range_mm)) / n;
  }

  // just print every x * 50 ms
  if (count%10 == 0){
    // print the values on a serial console
    Serial.print("Sensor 1: ");
    Serial.print(" average low: ");
    Serial.print(avgvalue1_low);
    Serial.print(" - average high: ");
    Serial.print(avgvalue1_high);
    Serial.print(" - ambient avg: ");
    Serial.print(avg_ambient1);
    Serial.print(" - range: ");
    Serial.print(sensor1.ranging_data.range_mm);
    Serial.print(" - ambient: ");
    Serial.print(sensor1.ranging_data.ambient_count_rate_MCPS);
    Serial.println();
  
    Serial.print("Sensor 2: ");
    Serial.print(" average low: ");
    Serial.print(avgvalue2_low);
    Serial.print(" - average high: ");
    Serial.print(avgvalue2_high);
    Serial.print(" - ambient avg: ");
    Serial.print(avg_ambient2);
    Serial.print(" - range: ");
    Serial.print(sensor2.ranging_data.range_mm);
    Serial.print(" - ambient: ");
    Serial.print(sensor2.ranging_data.ambient_count_rate_MCPS);
    Serial.println();

  
    Serial.print("Sensor 3: ");
    Serial.print(" average low: ");
    Serial.print(avgvalue3_low);
    Serial.print(" - average high: ");
    Serial.print(avgvalue3_high);
    Serial.print(" - ambient avg: ");
    Serial.print(avg_ambient3);
    Serial.print(" - range: ");
    Serial.print(sensor3.ranging_data.range_mm);
    Serial.print(" - ambient: ");
    Serial.print(sensor3.ranging_data.ambient_count_rate_MCPS);
    Serial.println();
  }
  count++;
}
