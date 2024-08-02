#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

// Undefine the conflicting macro 'ENABLE' to avoid conflict with Adafruit library
#undef ENABLE

#include <Adafruit_CircuitPlayground.h> // Include the library for Circuit Playground

// Re-define the ENABLE macro as needed for SparkFun_Bio_Sensor_Hub_Library
#define ENABLE 0x01

// No other Address options.
#define DEF_ADDR 0x55
// Reset pin, MFIO pin
const int resPin = 4; // RST connected to A0
const int mfioPin = 5; // MFIO connected to A2

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin);

bioData body;  

// Connect the S tab of the Temperature Sensor to A3
int sensorPin = A3;

// Set temperature threshold variable to check against. If the temperature reading is above
// this number in degrees Fahrenheit, the buzzer will sound
int threshold = 110; // 110 degrees Fahrenheit

void setup() {
  Serial.begin(115200); // Initialize Serial at 115200 bps for bio sensor data
  Wire.begin();

  // Initialize the Bio Sensor
  int result = bioHub.begin();
  if (!result) {
    Serial.println("Sensor started!");
  } else {
    Serial.println("Could not communicate with the sensor!!!");
  }

  Serial.println("Configuring Sensor....");
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings.
  if (!error) {
    Serial.println("Sensor configured.");
  } else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: ");
    Serial.println(error);
  }

  // Initialize the Circuit Playground library
  CircuitPlayground.begin();

  // Set the temperature sensor pin as an INPUT:
  pinMode(sensorPin, INPUT);

  // Data lags a bit behind the sensor; delay for data to catch up
  delay(4000);
  Serial.println("data loaded");
}

void loop() {
  Serial.println("begin loop");
  // Read Bio Sensor Data
  body = bioHub.readBpm();
  Serial.print("Heartrate: ");
  Serial.println(body.heartRate);
  Serial.print("Confidence: ");
  Serial.println(body.confidence);
  Serial.print("Oxygen: ");
  Serial.println(body.oxygen);
  Serial.print("Status: ");
  Serial.println(body.status);

  // Check for errors or issues
  if (body.heartRate == 0) {
    Serial.println("No valid heart rate detected.");
  }
  if (body.confidence == 0) {
    Serial.println("No confidence value detected.");
  }
  if (body.oxygen == 0) {
    Serial.println("No oxygen level detected.");
  }
  if (body.status == 0) {
    Serial.println("Sensor status is 0, indicating potential issues.");
  }

  // Temperature Monitoring
  long rawTemp = analogRead(sensorPin); // Read the raw 0-1023 value of temperature
  float voltage = rawTemp * (3.3 / 1023.0); // Calculate the voltage
  float celsius = (voltage - 0.5) * 100; // Calculate the celsius temperature
  float fahrenheit = (celsius * 9.0 / 5.0) + 32.0; // Convert Celsius to Fahrenheit

  // Print temperature values to serial monitor
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Celsius: ");
  Serial.println(celsius);
  Serial.print("Fahrenheit: ");
  Serial.println(fahrenheit);
  Serial.println();

  // Check the temperature, and make a beep if the temperature rises above the threshold
  if (fahrenheit >= threshold) {
    CircuitPlayground.playTone(1000, 500); // Play a tone at 1000 Hz for 500 ms
    Serial.println("Thermal alert!");
  }

  delay(1000); // Wait 1 second between readings
}
