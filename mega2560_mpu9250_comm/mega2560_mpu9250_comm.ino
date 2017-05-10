#include <Wire.h>
#include "mpu9250_registerMap.h"

int i;

void setup() {
  // put your setup code here, to run once:

  //initialize the i2c bus and join as master
  Wire.begin();

  //initialize the communication to the terminal
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  //As master, send a request for data from the i2c slave (mpu9250)
  Wire.requestFrom(address, dataSize);
  while(Wire.available()) {
    char c = Wire.read();
    Serial.print(c)
  }
  Serial.println();
  delay(1000);
  i++;
  if (i==30) {
    Serial.end();
  }
}

