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
   uint8_t magCalibX;
   uint8_t magCalibY;
   uint8_t magCalibZ;
public:
   uint16_t gyroX;
   uint16_t gyroY;
   uint16_t gyroZ;
   uint16_t accelX;
   uint16_t accelY;
   uint16_t accelZ;
   uint16_t magX;
   uint16_t magY;
   uint16_t magZ;

   MPU9250();
   ~MPU9250();
   void init();
   void writeI2cByte(uint8_t addr, uint8_t subAddr, uint8_t data);
   void readI2cByte(uint8_t addr, uint8_t subAddr, uint8_t& dataOut);
   void readAccelerometerRegisters();
   void readGyroscopeRegisters();
   void readMagnometerRegisters();
   void initAK8963Magnometer();
   bool dataReadyToRead();
};
