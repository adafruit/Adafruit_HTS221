
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
Adafruit_HTS221::~Adafruit_HTS221(void) {}

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
bool Adafruit_HTS221::begin(uint8_t i2c_address, TwoWire *wire,
                            int32_t sensor_id) {
  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}
/*!  @brief Initializer for post i2c/spi init
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_HTS221::_init(int32_t sensor_id) {
  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, HTS221_WHOAMI, 1);

  // make sure we're talking to the right chip
  if (chip_id.read() != HTS221_CHIP_ID) {
    return false;
  }

  _sensorid_humidity = sensor_id;
  _sensorid_temp = sensor_id + 1;
  boot();
  setActive(true); // arise!
  setDataRate(
      HTS221_RATE_12_5_HZ); // set to max data rate (default is one shot)

  _fetchTempCalibrationValues();
  Serial.print("T0: "); Serial.println(T0);
  Serial.print("T1: "); Serial.println(T1);
  Serial.print("T0_OUT: "); Serial.println(T0_OUT);
  Serial.print("T1_OUT: "); Serial.println(T1_OUT);
  return true;
}

/**
 * @brief Restores the trimming function values into registers from flash
 *
 */
void Adafruit_HTS221::boot(void) {
  Adafruit_BusIO_Register ctrl_2 =
      Adafruit_BusIO_Register(i2c_dev, HTS221_CTRL_REG_2, 1);
  Adafruit_BusIO_RegisterBits boot = Adafruit_BusIO_RegisterBits(&ctrl_2, 1, 7);

  boot.write(1);
  while (boot.read()) {
    delay(1);
  }
}

/**
 * @brief Sets the sensor to active or inactive
 *
 * @param active Set to true to enable the sensor, false to disable
 */
void Adafruit_HTS221::setActive(bool active) {
  Adafruit_BusIO_Register ctrl_1 =
      Adafruit_BusIO_Register(i2c_dev, HTS221_CTRL_REG_1, 1);
  Adafruit_BusIO_RegisterBits pd_bit =
      Adafruit_BusIO_RegisterBits(&ctrl_1, 1, 7);

  pd_bit.write(active);
}

/**
 * @brief Returns the current measurement rate
 *
 * @return hts221_rate_t the current measurement rate
 */
hts221_rate_t Adafruit_HTS221::getDataRate(void) {
  Adafruit_BusIO_Register ctrl_1 =
      Adafruit_BusIO_Register(i2c_dev, HTS221_CTRL_REG_1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits =
      Adafruit_BusIO_RegisterBits(&ctrl_1, 2, 0);

  return (hts221_rate_t)data_rate_bits.read();
}

/**
 * @brief Sets the rate at which measurements are taken
 *
 * @param data_rate The new measurement rate. Must be a `hts221_rate_t`
 */
void Adafruit_HTS221::setDataRate(hts221_rate_t data_rate) {
  Adafruit_BusIO_Register ctrl_1 =
      Adafruit_BusIO_Register(i2c_dev, HTS221_CTRL_REG_1, 1);
  Adafruit_BusIO_RegisterBits data_rate_bits =
      Adafruit_BusIO_RegisterBits(&ctrl_1, 2, 0);

  data_rate_bits.write(data_rate);
}

/**************************************************************************/
/*!
    @brief  Gets the pressure sensor and temperature values as sensor events
    @param  humidity Sensor event object that will be populated with humidity
   data
    @param  temp Sensor event object that will be populated with temp data
    @returns True
*/
/**************************************************************************/
bool Adafruit_HTS221::getEvent(sensors_event_t *humidity,
                               sensors_event_t *temp) {
  uint32_t t = millis();
  _read();

  // use helpers to fill in the events
  fillTempEvent(temp, t);
  return true;
}

/******************* Adafruit_Sensor functions *****************/
/*!
 *     @brief  Updates the measurement data for all sensors simultaneously
 */
/**************************************************************************/
void Adafruit_HTS221::_read(void) {

  Adafruit_BusIO_Register temp_data =
      Adafruit_BusIO_Register(i2c_dev, HTS221_TEMP_OUT_L, 2);

  uint8_t buffer[2];

  temp_data.read(buffer, 2);
  raw_temperature = 0;

  raw_temperature |= (int16_t)(buffer[1]);
  raw_temperature <<= 8;
  raw_temperature |= (int16_t)(buffer[0]);

  if (raw_temperature & 0x8000) {
    raw_temperature = raw_temperature - 0xFFFF;
  }

  _applyTemperatureCorrection();

}

void Adafruit_HTS221::fillTempEvent(sensors_event_t *temp, uint32_t timestamp) {
  memset(temp, 0, sizeof(sensors_event_t));
  temp->version = sizeof(sensors_event_t);
  temp->sensor_id = _sensorid_temp;
  temp->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  temp->timestamp = timestamp;
  temp->temperature = unscaled_temp; // will need to be corrected
}


void Adafruit_HTS221::_fetchTempCalibrationValues(void){
  Adafruit_BusIO_Register t0_degc_x8_l = Adafruit_BusIO_Register(i2c_dev, HTS221_T0_DEGC_X8, 2);
  Adafruit_BusIO_Register t1_t0_msb = Adafruit_BusIO_Register(i2c_dev, HTS221_T1_T0_MSB, 1);
  Adafruit_BusIO_Register  to_out = Adafruit_BusIO_Register(i2c_dev, HTS221_T0_OUT, 4);

  uint8_t buffer[4];
  // Get bytes for and assemble T0 and T1
  t1_t0_msb.read(buffer, 1);

  T1 |= (int16_t)(buffer[0] & 0b1100);
  T1 <<= 6;
  T0 |= (int16_t)(buffer[0] & 0b0011);
  T0 <<= 8;

  t0_degc_x8_l.read(buffer, 2);

  T0 |= (int16_t)buffer[0];
  T1 |= (int16_t)buffer[1];

  to_out.read(buffer, 4);

  T0_OUT |= (int16_t)(buffer[1]);
  T0_OUT <<= 8;
  T0_OUT |= (int16_t)(buffer[0]);

  T1_OUT |= (int16_t)(buffer[3]);
  T1_OUT <<= 8;
  T1_OUT |= (int16_t)(buffer[2]);
}

/**
 * @brief Use the temperature calibration values to correct the raw value
 *
 */
void Adafruit_HTS221::_applyTemperatureCorrection(void){
  Serial.println("applying correction *waves wand*");

}

// SENSOR_TYPE_RELATIVE_HUMIDITY
/*
  //       TEMPERATURE MATH

  int16_t T0_out, T1_out, T_out, T0_degC_x8_u16, T1_degC_x8_u16;
  int16_t T0_degC, T1_degC;
  uint8_t buffer[4], tmp;
  float   tmp_f;

  if(HTS221_ReadReg(handle, HTS221_T0_DEGC_X8, 2, buffer))
    return HTS221_ERROR;
  if(HTS221_ReadReg(handle, HTS221_T0_T1_DEGC_H2, 1, &tmp))
    return HTS221_ERROR;

  T0_degC_x8_u16 = (((uint16_t)(tmp & 0x03)) << 8) | ((uint16_t)buffer[0]);
  T1_degC_x8_u16 = (((uint16_t)(tmp & 0x0C)) << 6) | ((uint16_t)buffer[1]);
  T0_degC = T0_degC_x8_u16 >> 3;
  T1_degC = T1_degC_x8_u16 >> 3;

  if(HTS221_ReadReg(handle, HTS221_T0_OUT_L, 4, buffer))
    return HTS221_ERROR;

  T0_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];
  T1_out = (((uint16_t)buffer[3]) << 8) | (uint16_t)buffer[2];

  if(HTS221_ReadReg(handle, HTS221_TEMP_OUT_L_REG, 2, buffer))
    return HTS221_ERROR;

  T_out = (((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0];

  tmp_f = (float)(T_out - T0_out) * (float)(T1_degC - T0_degC) / (float)(T1_out
  - T0_out)  +  T0_degC; tmp_f *= 10.0f;

  *value = ( int16_t )tmp_f;

  */