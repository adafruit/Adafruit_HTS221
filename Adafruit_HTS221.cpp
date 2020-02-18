
/*!
 *  @file Adafruit_HTS221.cpp
 *
 *  @mainpage Adafruit HTS221 Humidity and Temperature Sensor library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit HTS221 Humidity and Temperature Sensor
 * library
 *
 * 	This is a library for the Adafruit HTS221 breakout:
 * 	https://www.adafruit.com/product/453X
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_HTS221.h"

/*!
 *    @brief  Instantiates a new HTS221 class
 */
Adafruit_HTS221::Adafruit_HTS221(void) {}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_HTS221::begin_I2C(uint8_t i2c_address, TwoWire *wire,
                                int32_t sensor_id) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init();
}

bool Adafruit_HTS221::_init(void) {
  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, HTS221_WHOAMI, 1);

  // make sure we're talking to the right chip
  if (chip_id.read() != HTS221_CHIP_ID) {
    Serial.print("Got id: 0x");
    Serial.println(chip_id.read(), HEX);
    return false;
  }

  return true;
}
