//    FILE: LTC2485_nolib.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: raw read the device
//     URL: https://github.com/RobTillaart/LTC2485

#include "Arduino.h"
#include "Wire.h"

void setup() 
{
  while(!Serial);
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  delay(200);  //  for first conversion

  Wire.beginTransaction(0x16);    //  address
  Wire.write(0x00);               //  default configuration
  int n = Wire.endTransaction();  //  send config over I2C to address
  Serial.println(n);

  n = Wire.requestFrom(0x16, 4);
  Serial.println(n);
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());

  Serial.println("\ndone...");
}

void loop()
{
}

//  -- END OF FILE --
