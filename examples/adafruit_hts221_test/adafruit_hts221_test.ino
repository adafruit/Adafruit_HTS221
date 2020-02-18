// Basic demo for reading Humidity and Temperature
#include <Wire.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

Adafruit_HTS221 hts; // TODO FIX NAME

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit HTS221 test!");

 // Try to initialize!
  if (!hts.begin_I2C()) {
  //if (!hts.begin_SPI(LPS_CS)) {
  //if (!hts.begin_SPI(LPS_CS, LPS_SCK, LPS_MISO, LPS_MOSI)) {
    Serial.println("Failed to find HTS221 chip");
    while (1) { delay(10); }
  }

  Serial.println("HTS221 Found!");

  //msa.setDataRate(MSA301_DATARATE_31_25_HZ);
  //Serial.print("Data rate set to: ");
  //switch (msa.getDataRate()) {
  //  case HTS221_DATARATE_1_HZ: Serial.println("1 Hz"); break;
  //  case HTS221_DATARATE_1_95_HZ: Serial.println("1.95 Hz"); break;
  //  case HTS221_DATARATE_3_9_HZ: Serial.println("3.9 Hz"); break;
  //}
}

void loop() {
  // sensors_event_t temp;
  // sensors_event_t humidity;
  // hts.getEvent(&humidity, &temp);// get pressure

  delay(100);
}
