//#include <Wire.h>
#include "mpu9250.h"

int i;
float ax,ay,az, gx,gy,gz, mx,my,mz;
MPU9250 imu;

void setup() {
  // put your setup code here, to run once:
  //init the mpu9250
  imu.init();
  

  //initialize the i2c bus and join as master
  Wire.begin();

  //initialize the communication to the terminal
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  //As master, send a request for data from the i2c slave (mpu9250)
  //Wire.requestFrom(address, dataSize);
  while(1) {

    //check for new data in registers
    if(imu.dataReadyToRead()) {
      //read new registers
      imu.readGyroscopeRegisters();
      imu.readMagnometerRegisters();
      imu.readAccelerometerRegisters();
      
      //gyro
      gx = (float) imu.gyroX;
      gy = (float) imu.gyroY;
      gz = (float) imu.gyroZ;
      
      //accel
      ax = (float) imu.accelX;
      ay = (float) imu.accelY;
      az = (float) imu.accelZ;

      //magno
      mx = (float) imu.magX;
      my = (float) imu.magY;
      mz = (float) imu.magZ;
      
    }
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println();
  delay(1000);
  i++;
  if (i==30) {
    Serial.end();
  }
}

