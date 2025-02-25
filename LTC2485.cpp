//
//    FILE: LTC2485.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.2
//    DATE: 2025-02-21
// PURPOSE: Arduino library for LTC2485 I2C 24 bit ADC.
//     URL: https://github.com/RobTillaart/LTC2485


#include "LTC2485.h"

//  CONVERSION
//  needed as I2C is blocked during conversion.
//  you can optimize these a bit as low as about 160 and 80.
//  at your own risk.
#define LTC2485_DELAY_1X                        200
#define LTC2485_DELAY_2X                        100


/////////////////////////////////////////////////////
//
//  CONSTRUCTORS
//
LTC2485::LTC2485(uint8_t address, TwoWire *wire)
{
  _address    = address;
  _wire       = wire;
  //  defaults
  _lastAccess = 0;
  _timeout    = LTC2485_DELAY_1X;
  _error      = 0;
  _vref       = 5.0;
  _config     = LTC2485_SPEED_1X | LTC2485_REJECT_50_60_HZ;
}


bool LTC2485::begin(float VREF)
{
  _vref = VREF;
  //  optional address check
  if (! isConnected()) return false;
  configure(_config);
  return true;
}


bool LTC2485::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


uint8_t LTC2485::getAddress()
{
  return _address;
}

int LTC2485::configure(uint8_t config)
{
  //  skip invalid configuration, see table 1.
  if ((config & 0x06) || (config == 0x09)) return 255;
  if (config & 0xF0) return 255;
  _config = config;

  //  wait until conversion is done...
  //  TODO:  while(_write(_config) != 0 && not timeout); ?
  //
  while ((millis() - _lastAccess) < _timeout)
  {
    delay(1);
  }
  int rv = _write(_config);

  //  update lastAccess and timeout on successful write
  if (rv == 0)
  {
    _lastAccess = millis();
    if (_config & LTC2485_SPEED_2X)
    {
      _timeout = LTC2485_DELAY_2X;
    }
    else
    {
      _timeout = LTC2485_DELAY_1X;
    }
  }
  return rv;
}


int32_t LTC2485::getADC()
{
  //  only reconfigure if TEMPERATURE flag is set.
  if ((_config & LTC2485_INTERNAL_TEMP) > 0)
  {
    if (configure(_config & 0x07) != 0)
    {
      //  TODO cleaner error handling.
      Serial.println("FAIL TO CONFIG-A");
      return 0;
    }
  }

  //  wait until last conversion is done...
  //  TODO:  can this be optimized with a _read() check.
  while ((millis() - _lastAccess) < _timeout)
  {
    delay(1);
  }
  int32_t value = _read();
  //  update lastAccess
  _lastAccess = millis();
  //  make two complements.
  value ^= 0x80000000;
  return value;
}


float LTC2485::getVolts()
{
  //  return (getADC() * _vref) / 16777215L;
  return getADC() * _vref * 5.960464832810e-8;
}


float LTC2485::getTemperature()
{
  //  only reconfigure if TEMPERATURE flag is NOT set.
  if ((_config & LTC2485_INTERNAL_TEMP) == 0)
  {
    if (configure(_config | LTC2485_INTERNAL_TEMP) != 0)
    {
      Serial.println("FAIL TO CONFIG-T");
      return 0;
    }
  }

  //  wait until last conversion is done...
  while ((millis() - _lastAccess) < _timeout)
  {
    delay(1);
  }
  float volts = int32_t(_read() ^ 0x80000000) * _vref * 5.960464832810e-8;
  //  datasheet page 20
  //  27 C  == 420 mV
  //  SLOPE == 1.40 mV
  float TC = 27.0 + 1.40 * (volts - 0.420);
  return TC;
}


uint32_t LTC2485::lastAccessed()
{
  return _lastAccess;
}


//////////////////////////////////////////////////////////////////
//
//  PRIVATE functions
//
int LTC2485::_write(uint8_t value)
{
  _wire->beginTransmission(_address);
  _wire->write(value);
  return _wire->endTransmission();
}


uint32_t LTC2485::_read()
{
  uint32_t rv = 0;
  int n = _wire->requestFrom(_address, (uint8_t)4);
  if (n == 4)
  {
    rv |= _wire->read();
    rv <<= 8;
    rv |= _wire->read();
    rv <<= 8;
    rv |= _wire->read();
    rv <<= 8;
    rv |= _wire->read();
    return rv;

    //  DEBUG
    // uint32_t A = _wire->read();
    // uint32_t B = _wire->read();
    // uint32_t C = _wire->read();
    // uint32_t D = _wire->read();
    // if (A < 0x10) Serial.print(0);
    // Serial.print(A, HEX);
    // Serial.print("\t");
    // if (B < 0x10) Serial.print(0);
    // Serial.print(B, HEX);
    // Serial.print("\t");
    // if (C < 0x10) Serial.print(0);
    // Serial.print(C, HEX);
    // Serial.print("\t");
    // if (D < 0x10) Serial.print(0);
    // Serial.print(D, HEX);
    // Serial.print("\t");

    // rv = (A << 24) + (B << 16) + (C << 8) + D;
    // return rv;
  }
  return n;
}



//  -- END OF FILE --

