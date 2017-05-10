#include "mpu9250.h"

MPU9250::MPU9250()
{

}

MPU9250::~MPU9250()
{
}


//MPU init function and settings sourced from below
//https://github.com/kriswiner/MPU-9250/blob/master/STM32F401/MPU9250.h
void MPU9250::init()
{
   // Initialize MPU9250 device
   // wake up device
   writeI2cByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
   wait(0.1); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

   // get stable time source
   writeI2cByte(MPU9250_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

   // Configure Gyro and Accelerometer
   // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
   // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
   // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
   writeI2cByte(MPU9250_ADDRESS, CONFIG, 0x03);

   // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
   writeI2cByte(MPU9250_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

   // Set gyroscope full scale range
   // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
   uint8_t c;
   readI2cByte(MPU9250_ADDRESS, GYRO_CONFIG, c); // get current GYRO_CONFIG register value
   // c = c & ~0xE0; // Clear self-test bits [7:5]
   c = c & ~0x02; // Clear Fchoice bits [1:0]
   c = c & ~0x18; // Clear AFS bits [4:3]
   c = c | Gscale << 3; // Set full scale range for the gyro
   // c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
   writeI2cByte(MPU9250_ADDRESS, GYRO_CONFIG, c ); // Write new GYRO_CONFIG value to register

   // Set accelerometer full-scale range configuration
   readI2cByte(MPU9250_ADDRESS, ACCEL_CONFIG, c); // get current ACCEL_CONFIG register value
   // c = c & ~0xE0; // Clear self-test bits [7:5]
   c = c & ~0x18;  // Clear AFS bits [4:3]
   c = c | Ascale << 3; // Set full scale range for the accelerometer
   writeI2cByte(MPU9250_ADDRESS, ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

   // Set accelerometer sample rate configuration
   // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
   // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
   readI2cByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c); // get current ACCEL_CONFIG2 register value
   c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
   c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
   writeI2cByte(MPU9250_ADDRESS, ACCEL_CONFIG2, c); // Write new ACCEL_CONFIG2 register value

   // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
   // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

   //set the i2c buss for passthrough, this enables the external magn
   writeI2cByte(MPU9250_ADDRESS, MPU9250_INT_PIN_CFG, 0x22);
   writeI2cByte(MPU9250_ADDRESS, MPU9250_INT_ENABLE, 0x01);
}

void MPU9250::writeI2cByte(uint8_t addr, unit8_t subAddr, uint8_t data)
{
   Wire.beginTransmission(addr); //Let Tx know we are going to be sending
   Wire.write(subAddr);          //put slave reg on the Tx line
   Wire.write(data);             //put data on the line
   Wire.endTransmission();       //send
   return;
}

void MPU9250::readI2cByte(uint8_t addr, unit8_t subAddr, uint8_t& dataOut)
{
   Wire.beginTransmission(addr); //wake up the Tx
   Wire.write(subAddr);          //put slave adder on Tx to let us know where to request from
   Wire.endTransmission(false);  //send but keep connection up
   Wire.requestFrom(address, (unit8_t) 1);   //request 1 byte from the slave address
   *dataOut = Wire.read();       //readd the Rx byte requested
   return;
}

MPU9250::readAccelerometerRegisters(int16_t& dataOut)
{
   //2 bytes for each data value
   //H/L, H/L, H/L
   uint8_t accelData[6];

   //address of mpu, address of register, read 6 bytes from the subAddr, read into accelData
   readI2cByte(MPU9250_ADDRESS, MPU9250_ACCEL_XOUT_H, 6, &accelData[0])

   //Parse the data
   dataOut[0] = (uint16_t)(accelData[0] << 8 | accelData[1]);
   dataOut[1] = (uint16_t)(accelData[2] << 8 | accelData[3]);
   dataOut[2] = (uint16_t)(accelData[4] << 8 | accelData[5]);
   return;
}

//this device is located on a slave chip: AK8963
//it has a slightly different protocol
MPU9250::readMegnometerRegisters(int16_t& dataOut)
{
   //check the ST1 register to see if data is read
   //ST1, 1 = datareadytoread, 0 = nothing to read
   uint8_t magData[7];

   //if the data in ST1 is == 1
   if(readBytes(AK8963_ADDRESS, AK8963_ST1) & 0x01) {

      //read 7 bytes of data, h/l, h/l, h/l, ST2.
      //ST2 must be read so that it can clear the data registers
      readBytes(AK8963_ADDRESS, AK8963_HXH, 7, &magData[0]);

      //put data into output array
      dataOut[0] = (uint16_t)(magData[0] << 8 | magData[1]);
      dataOut[1] = (uint16_t)(magData[2] << 8 | magData[3]);
      dataOut[2] = (uint16_t)(magData[4] << 8 | magData[5]);
   }
   return;
}

MPU9250::readGyroscopeRegisters(int16_t& dataOut)
{
   //2 bytes for each data value
   //H/L, H/L, H/L
   uint8_t gyroData[6];

   //address of mpu, address of register, read 6 bytes from the subAddr, read into accelData
   readBytes(MPU9250_ADDRESS, MPU9250_GYRO_XOUT_H, 6, &gyroData[0])

   //Parse the data
   dataOut[0] = (uint16_t)(gyroData[0] << 8 | gyroData[1]);
   dataOut[1] = (uint16_t)(gyroData[2] << 8 | gyroData[3]);
   dataOut[2] = (uint16_t)(gyroData[4] << 8 | gyroData[5]);
   return;
}

//the purpose of this function is to set the frequency of dataOut
//and to also extract the ROM calibration data stored in the AK8963
void MPU9250::initAK8963Magnometer(unit8_t& magCalibOut)
{
   unit8_t temp[3];

   //power down mag
   writeI2cByte(AK8963_ADDRESS, AK8963_CNTL, 0x00);

   //enter ROM fuse mode
   writeI2cByte(AK8963_ADDRESS, AK8963_CNTL, 0x0F);

   //read stored calibration data
   readBytes(AK8963_ADDRESS, AK8963_ASAX, 3, &temp[0]);
   magCalibOut[0] = (((float)temp[0]-128.0)*0.5/128.0) + 1.0;
   magCalibOut[1] = (((float)temp[1]-128.0)*0.5/128.0) + 1.0;
   magCalibOut[2] = (((float)temp[2]-128.0)*0.5/128.0) + 1.0;

   //power down mag
   writeI2cByte(AK8963_ADDRESS, AK8963_CNTL, 0x00);

   //set configuration to mode 2: 100Hz continues sample rate
   //16 bit resolution: 0001_0110
   writeI2cByte(AK8963_ADDRESS, AK8963_CNTL, 0x16);
   return;
}
