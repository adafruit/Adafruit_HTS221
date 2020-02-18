/*!
 *  @file Adafruit_HTS221.h
 *
 * 	I2C Driver for the Adafruit HTS221 Humidity and Temperature Sensor
 *library
 *
 * 	This is a library for the Adafruit HTS221 breakout:
 * 	https://www.adafruit.com/products/453X
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_HTS221_H
#define _ADAFRUIT_HTS221_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>

#define HTS221_I2CADDR_DEFAULT 0x5F ///< HTS221 default i2c address
#define HTS221_CHIP_ID 0xBC         ///< HTS221 default device id from WHOAMI

#define HTS221_EXAMPLE_REG 0x00 ///< Example register
#define HTS221_WHOAMI 0x0F      ///< Chip ID register

/**
 * @brief Example enum values
 *
 * Allowed values for `setProximityLEDCurrent`.
 */
typedef enum led_current {
  HTS221_EXAMPLE_50MA,
  HTS221_EXAMPLE_75MA,
} HTS221_example_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the HTS221 I2C Digital Potentiometer
 */
class Adafruit_HTS221 {
public:
  Adafruit_HTS221();

  bool begin_I2C(uint8_t i2c_address = HTS221_I2CADDR_DEFAULT,
                 TwoWire *wire = &Wire, int32_t sensor_id = 0);

  // bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI,
  //                int32_t sensor_id = 0);
  // bool begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
  //                int8_t mosi_pin, int32_t sensor_id = 0);

private:
  bool _init(void);

  Adafruit_I2CDevice *i2c_dev;
};

#endif
