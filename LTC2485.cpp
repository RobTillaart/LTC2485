//
//    FILE: LTC2485.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.2
//    DATE: 2025-02-21
// PURPOSE: Arduino library for LTC2485 I2C 24 bit ADC.
//     URL: https://github.com/RobTillaart/LTC2485


#include "LTC2485.h"



/////////////////////////////////////////////////////
//
//  CONSTRUCTORS
//
LTC2485::LTC2485(uint8_t address, TwoWire *wire)
{
  _address = address;
  _wire = wire;
}


bool LTC2485::begin(float VREF)
{
  _vref = VREF;
  //  optional address check
  if (! isConnected()) return false;
  configure(0x00);
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
      _timeout = 80;
    }
    else
    {
      _timeout = 160;
    }
  }
  return rv;
}


int32_t LTC2485::getADC()
{
  if ((_config & LTC2485_INTERNAL_TEMP) > 0)
  {
    if (configure(_config & 0x07) != 0) 
    {
      Serial.println("FAIL TO CONFIG-A");
      return 0;
    }
  }
  while ((millis() - _lastAccess) < _timeout)
  {
    delay(1);
  }
  int32_t value = _read();
  _lastAccess = millis();
  //  TODO check read error
  value ^= 0x80000000;
  return value;
}


float LTC2485::getVolts()
{
  //  return (getADC() * _vref) / 16777215L;
  return getADC() * _vref * 6.1643863061e-8;
}


float LTC2485::getTemperature()
{
  if ((_config & LTC2485_INTERNAL_TEMP) == 0)
  {
    if (configure(_config | LTC2485_INTERNAL_TEMP) != 0)
    {
      Serial.println("FAIL TO CONFIG-T");
      return 0;
    }
  }
  while ((millis() - _lastAccess) < _timeout)
  {
    delay(1);
  }
  float volts = int32_t(_read() ^ 0x80000000) * _vref * 6.1643863061e-8;
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
    // rv |= _wire->read();
    // rv <<= 8;
    // rv |= _wire->read();
    // rv <<= 8;
    // rv |= _wire->read();
    // rv <<= 8;
    // rv |= _wire->read();
    // return rv;

    uint8_t A = _wire->read();
    uint8_t B = _wire->read();
    uint8_t C = _wire->read();
    uint8_t D = _wire->read();
    if (A < 0x10) Serial.print(0);
    Serial.print(A, HEX);
    Serial.print("\t");
    if (B < 0x10) Serial.print(0);
    Serial.print(B, HEX);
    Serial.print("\t");
    if (C < 0x10) Serial.print(0);
    Serial.print(C, HEX);
    Serial.print("\t");
    if (D < 0x10) Serial.print(0);
    Serial.print(D, HEX);
    Serial.print("\t");

    rv = (A << 24) + (B << 16) + (C << 8) + D;
    return rv;
  }
  return n;
}



//  -- END OF FILE --

