#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "energy.h"

// ADAfruit INA219 current monitoring in C
// inpired from arduino library : https://github.com/adafruit/Adafruit_INA219
// and python library : https://github.com/scottjw/subfact_pi_ina219

int main (int argc, char *argv[]) {
	// Open log file 
	FILE* log = fopen("/mnt/tmp/log_current.csv","a");

	// Init I2C
	int fd;
        wiringPiSetup();
        fd=wiringPiI2CSetup(0x40);
        if(fd==-1) {
                printf("Can't setup the I2C device\n");
                return -1;
        }
        else {
		// Calibrate INA219 to 16V 400mA (highest precision)
		// Calibration val is 8192 ==> 0x2000 in haxdecimal 
		// warning: we need to change endianness when sending data 
		// so 0x2000 become 0x0020 
  		wiringPiI2CWriteReg16(fd,INA219_REG_CALIBRATION, 0x0020);
  
  		// Set Config register to take into account the settings above
 		uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
                		    INA219_CONFIG_GAIN_1_40MV |
                		    INA219_CONFIG_BADCRES_12BIT |
                		    INA219_CONFIG_SADCRES_12BIT_1S_532US |
        		            INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
		// Set config and don't forget to swap bytes before sending it 
  		wiringPiI2CWriteReg16(fd,INA219_REG_CONFIG, (((config & 0x00FF)<<8) | ((config & 0xFF00)>>8)));
		
		// Write log header
     		fprintf(log,"Time,Difftime,Current\n");

		// Timestamp for mesure 
		struct timeval t;
		// Timestamp in microseconds
		unsigned long long tms_start=0; 
		unsigned long long tms;

		// Compute start time
		gettimeofday(&t,NULL);
		tms_start = 1000000*t.tv_sec + t.tv_usec;
                while(1) {
			// Sometimes a sharp load will reset the INA219, which will
  			// reset the cal register, meaning CURRENT and POWER will
  			// not be available ... avoid this by always setting a cal
  			// value even if it's an unfortunate extra step
			//wiringPiI2CWriteReg16(fd,INA219_REG_CALIBRATION, 0x0020);

                        uint16_t data=wiringPiI2CReadReg16(fd,INA219_REG_CURRENT);
			gettimeofday(&t, NULL);
                        tms = 1000000*t.tv_sec + t.tv_usec;
                        if(data==-1) {
                                printf("No data\n");
                        }
                        else {
                                //save current in mA
				//swap received bytes 
				//20 => current divider
				uint16_t res =(((data & 0x00FF)<<8) | ((data & 0xFF00)>>8));
                                fprintf(log,"%llu,%llu,%f\n",tms,tms-tms_start,((float)res)/20);
                        }
                }
        }
	return 0;
}
