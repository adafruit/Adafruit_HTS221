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
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define HTS221_I2CADDR_DEFAULT 0x5F ///< HTS221 default i2c address
#define HTS221_CHIP_ID 0xBC         ///< HTS221 default device id from WHOAMI
#define HTS221_CTRL_REG_1 0x20      ///< First control regsiter; PD, OBDU, ODR
#define HTS221_CTRL_REG_2                                                      \
  0x21 ///< Second control regsiter; BOOT, Heater, ONE_SHOT
#define HTS221_CTRL_REG_3 0x22     ///< Third control regsiter; DRDY_H_L, DRDY
#define HTS221_HUMIDITY_OUT_L 0x28 ///< Humidity output register (LSByte)
#define HTS221_TEMP_OUT_L 0x2A     ///< Temperature output register (LSByte)

#define HTS221_WHOAMI 0x0F ///< Chip ID register
/**
 * @brief
 *
 * Allowed values for `setDataRate`.
 */
typedef enum {
  HTS221_RATE_ONE_SHOT,
  HTS221_RATE_1_HZ,
  HTS221_RATE_7_HZ,
  HTS221_RATE_12_5_HZ,
} hts221_rate_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the HTS221 I2C Digital Potentiometer
 */
class Adafruit_HTS221 {
public:
  Adafruit_HTS221();
  ~Adafruit_HTS221();

  bool begin(uint8_t i2c_address = HTS221_I2CADDR_DEFAULT,
             TwoWire *wire = &Wire, int32_t sensor_id = 0);

  // bool getEvent(sensors_event_t *humidity, sensors_event_t *temp);
  void boot(void);

  void setActive(bool active);
  hts221_rate_t getDataRate(void);
  void setDataRate(hts221_rate_t data_rate);
  bool getEvent(sensors_event_t *humidity, sensors_event_t *temp);
  // Adafruit_Sensor *getTemperatureSensor(void);
  // Adafruit_Sensor *getPressureSensor(void);

protected:
  void _read(void);
  virtual bool _init(int32_t sensor_id);

  float unscaled_temp,   ///< Last reading's temperature (C) before scaling
      unscaled_humidity; ///< Last reading's humidity (percent) before scaling

  uint16_t _sensorid_humidity, ///< ID number for humidity
      _sensorid_temp;          ///< ID number for temperature

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface

  //   Adafruit_HTS221_Temp *temp_sensor = NULL; ///< Temp sensor data object
  //   Adafruit_HTS221_Pressure *humidity_sensor =
  //       NULL; ///< Pressure sensor data object

private:
  //   friend class Adafruit_HTS221_Temp;     ///< Gives access to private
  //   members to
  //                                         ///< Temp data object
  //   friend class Adafruit_HTS221_Pressure; ///< Gives access to private
  //                                         ///< members to Pressure data
  //                                         ///< object
  //   void fillPressureEvent(sensors_event_t *humidity, uint32_t timestamp);
  void fillTempEvent(sensors_event_t *temp, uint32_t timestamp);
};

#endif
