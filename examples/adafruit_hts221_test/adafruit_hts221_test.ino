// Basic demo for reading Humidity and Temperature
#include <Wire.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

Adafruit_HTS221 hts;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit HTS221 test!");

 // Try to initialize!
  if (!hts.begin()) {
    Serial.println("Failed to find HTS221 chip");
    while (1) { delay(10); }
  }
  Serial.println("HTS221 Found!");

  // hts.setDataRate(HTS221_RATE_7_HZ);
  Serial.print("Data rate set to: ");
  switch (hts.getDataRate()) {
   case HTS221_RATE_ONE_SHOT: Serial.println("One Shot"); break;
   case HTS221_RATE_1_HZ: Serial.println("1 Hz"); break;
   case HTS221_RATE_7_HZ: Serial.println("7 Hz"); break;
   case HTS221_RATE_12_5_HZ: Serial.println("12.5 Hz"); break;
  }
}

void loop() {
  sensors_event_t temp;
  sensors_event_t humidity;
  hts.getEvent(&humidity, &temp);// get pressure
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");

  delay(500);
}
