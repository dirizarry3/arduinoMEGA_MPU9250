#include "mpu9250_registerMap.h"
#include <Wire.h> //arduino i2c comm library

class MPU9250
{
protected:
   //create enumerated lists for scaling
   //of the accel, gyro, magno - meters
   enum Ascale {
      AFS_2G = 0,
      AFS_4G
   };

   enum Gscale {
      GFS_250DPS = 0,
      GFS_500DPS
   };

   enum Mscale {
      MFS_14BITS = 0,
      MFS_16BITS
   };


private:
   float roll;
   float pitch;
   float yaw;
   float accX;
   float accY;
   float accZ;
public:
   MPU9250();
   ~MPU9250();
   void init();
   void writeI2cByte(uint8_t addr, unit8_t subAddr, uint8_t data);
   void readI2cByte(uint8_t addr, unit8_t subAddr, uint8_t& dataOut);
   void readAccelerometerRegisters(int16_t& dataOut);
   void readGyroscopeRegisters(int16_t& dataOut);
   void readMegnometerRegisters(int16_t& dataOut);
   void initAK8963Magnometer();
}
