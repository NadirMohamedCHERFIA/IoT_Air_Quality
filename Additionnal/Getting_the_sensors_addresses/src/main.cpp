/************************************************************************/
/*With I2C communication, each slave on the bus has its own address,    */
/*a hexadecimal number that allows the ESP32 to communicate with each   */
/*device.                                                               */
/*The I2C address can be usually found on the component’s datasheet.    */
/*However, if it is difficult to find out,we need to run an I2C         */
/*scanner sketch to find out the I2C addresses.                         */
/************************************************************************/
#include <Wire.h>
#include"Arduino.h"
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
