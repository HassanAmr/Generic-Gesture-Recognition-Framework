#include <BNO055.h>

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <thread>
#include <cmath>
#include<string.h>    //strlen
#include<string>  //string
#include <sstream>
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent

#include <time.h>
#include "math.h"
//#include "GRT/GRT.h"

using namespace std;
//using namespace GRT;

/* bno055 I2C Address */
#define BNO055_I2C_ADDR1                0x28
#define BNO055_I2C_ADDR2                0x29


void LoopBreaker();
void sig_handler(int signo);



//Globals
//static struct bno055_t *bno055;

int running = 0;
bool breakLoop = false;

	thread br (LoopBreaker);


int main(int argc, char** argv)
{

	/*-----------------------------------------------------------------------*
	 ************************* START INITIALIZATION ***********************
	 *-------------------------------------------------------------------------*/

	uint8_t rx_tx_buf[ARRAY_SIZE_SIX];		//buffer for communication between the sensors and the host device
	I2c* i2c;	
	i2c = new mraa::I2c(0);

	bno055_t sensor1, sensor2;
	

	u8 power_mode = BNO055_ZERO_U8X;

	bno055_mag_offset_t offsetData1;
	bno055_mag_offset_t offsetData2;

	/*********read raw mag data***********/
	/* variable used to read the mag x data from the first sensor*/
	s16 mag_datax1  = BNO055_ZERO_U8X;
	/* variable used to read the mag y data from the first sensor*/
	s16 mag_datay1  = BNO055_ZERO_U8X;
	/* variable used to read the mag z data from the first sensor*/
	s16 mag_dataz1  = BNO055_ZERO_U8X;
	/* variable used to read the mag x data from the second sensor*/
	s16 mag_datax2  = BNO055_ZERO_U8X;
	/* variable used to read the mag y data from the second sensor*/
	s16 mag_datay2  = BNO055_ZERO_U8X;
	/* variable used to read the mag z data from the second sensor*/
	s16 mag_dataz2  = BNO055_ZERO_U8X;

	signal(SIGINT, sig_handler);

	unsigned char mag_calib_status1 = 0;
	unsigned char mag_calib_status2 = 0;


	sensor1.dev_addr = BNO055_I2C_ADDR1;
	sensor2.dev_addr = BNO055_I2C_ADDR2;
	
	s32 comres1 = ERROR;
	s32 comres2 = ERROR;
	comres1 = bno055_init(&sensor1, i2c, rx_tx_buf);
	comres2 = bno055_init(&sensor2, i2c, rx_tx_buf);

	power_mode = POWER_MODE_NORMAL; /* set the power mode as NORMAL*/
	comres1 += bno055_set_power_mode(power_mode,&sensor1, i2c, rx_tx_buf);
	comres1 += bno055_set_operation_mode(OPERATION_MODE_MAGONLY,&sensor1, i2c, rx_tx_buf);

	comres2 += bno055_set_power_mode(power_mode,&sensor2, i2c, rx_tx_buf);
	comres2 += bno055_set_operation_mode(OPERATION_MODE_MAGONLY,&sensor2, i2c, rx_tx_buf);

	u8 magConfig;
	magConfig = MAG_DATA_OUTPUT_RATE_30HZ | MAG_OPR_MODE_HIGH_ACCURACY | MAG_POWER_MODE_NORMAL;
	BNO055_RETURN_FUNCTION_TYPE kcomres1 = bno055_write_register(MAG_CONFIG_ADDR,
			&magConfig,
			BNO055_ONE_U8X,
			&sensor1
			, i2c, rx_tx_buf);
	BNO055_RETURN_FUNCTION_TYPE jcomres2 = bno055_write_register(MAG_CONFIG_ADDR,
			&magConfig,
			BNO055_ONE_U8X,
			&sensor2
			, i2c, rx_tx_buf);

	/*---------------------------------------------------------------------*
	 ************************* END INITIALIZATION **********************
	 *---------------------------------------------------------------------*/
	
	 BNO055_delay_msek(1000);

	//printf("xOffset1: %x, yOffset1: %x, zOffset1: %x\n",offsetData1.x,offsetData1.y,offsetData1.z);
	//printf("xOffset2: %x, yOffset2: %x, zOffset2: %x\n\n",offsetData2.x,offsetData2.y,offsetData2.z);


	//bno055_read_mag_offset(&offsetData1, &sensor1);
	//bno055_read_mag_offset(&offsetData1, &sensor2);

	//printf("xOffset1: %x, yOffset1: %x, zOffset1: %x\n",offsetData1.x,offsetData1.y,offsetData1.z);
	//printf("xOffset2: %x, yOffset2: %x, zOffset2: %x\n\n",offsetData2.x,offsetData2.y,offsetData2.z);

	double x1, x2, y1, y2, z1, z2;

	//read data first once, then have it as an offset (-ve) for the next readings
	BNO055_I2C_bus_read(sensor1.dev_addr,BNO055_MAG_DATA_X_LSB_VALUEX__REG,rx_tx_buf, BNO055_SIX_U8X, i2c, rx_tx_buf);
	/* Data X*/
	rx_tx_buf[INDEX_ZERO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ZERO],BNO055_MAG_DATA_X_LSB_VALUEX);
	rx_tx_buf[INDEX_ONE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ONE], BNO055_MAG_DATA_X_MSB_VALUEX);
	mag_datax1 = ((((s32)((s8)rx_tx_buf[INDEX_ONE])) << BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_ZERO]));
	x1 = (double)(mag_datax1);// /MAG_DIV_UT);
	/* Data Y*/
	rx_tx_buf[INDEX_TWO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_TWO],BNO055_MAG_DATA_Y_LSB_VALUEY);
	rx_tx_buf[INDEX_THREE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_THREE], BNO055_MAG_DATA_Y_MSB_VALUEY);
	mag_datay1 = ((((s32)((s8)rx_tx_buf[INDEX_THREE])) <<BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_TWO]));
	y1 = (double)(mag_datay1);// /MAG_DIV_UT);
	/* Data Z*/
	rx_tx_buf[INDEX_FOUR] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FOUR],BNO055_MAG_DATA_Z_LSB_VALUEZ);
	rx_tx_buf[INDEX_FIVE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FIVE],BNO055_MAG_DATA_Z_MSB_VALUEZ);
	mag_dataz1 = ((((s32)((s8)rx_tx_buf[INDEX_FIVE])) << BNO055_SHIFT_8_POSITION)| (rx_tx_buf[INDEX_FOUR]));
	z1 = (double)(mag_dataz1);// /MAG_DIV_UT);

	/*Read the six byte value of mag xyz from second sesnor*/
	BNO055_I2C_bus_read(sensor2.dev_addr,BNO055_MAG_DATA_X_LSB_VALUEX__REG,rx_tx_buf, BNO055_SIX_U8X, i2c, rx_tx_buf);
	/* Data X*/
	rx_tx_buf[INDEX_ZERO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ZERO],BNO055_MAG_DATA_X_LSB_VALUEX);
	rx_tx_buf[INDEX_ONE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ONE], BNO055_MAG_DATA_X_MSB_VALUEX);
	mag_datax2 = ((((s32)((s8)rx_tx_buf[INDEX_ONE])) << BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_ZERO]));
	x2 = (double)(mag_datax2);// /MAG_DIV_UT);
	/* Data Y*/
	rx_tx_buf[INDEX_TWO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_TWO],BNO055_MAG_DATA_Y_LSB_VALUEY);
	rx_tx_buf[INDEX_THREE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_THREE], BNO055_MAG_DATA_Y_MSB_VALUEY);
	mag_datay2 = ((((s32)((s8)rx_tx_buf[INDEX_THREE])) <<BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_TWO]));
	y2 = (double)(mag_datay2);// /MAG_DIV_UT);
	/* Data Z*/
	rx_tx_buf[INDEX_FOUR] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FOUR],BNO055_MAG_DATA_Z_LSB_VALUEZ);
	rx_tx_buf[INDEX_FIVE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FIVE],BNO055_MAG_DATA_Z_MSB_VALUEZ);
	mag_dataz2 = ((((s32)((s8)rx_tx_buf[INDEX_FIVE])) << BNO055_SHIFT_8_POSITION)| (rx_tx_buf[INDEX_FOUR]));
	z2 = (double)(mag_dataz2); // /MAG_DIV_UT);

	printf("%f \t %f \t %f \t%f \t%f \t%f\n", x1, y1, z1, x2, y2, z2);

	while (1)
	{
		if(running == -1)
		{
			printf("Something went wrong on the board\n");
			break;
		}

		//bno055_get_mag_calib_stat(&mag_calib_status1, &sensor1);

		//bno055_get_mag_calib_stat(&mag_calib_status2, &sensor2);

		//printf("%x\t%x\n", mag_calib_status1, mag_calib_status2);

		/*Read the six byte value of mag xyz from first sensor*/
		BNO055_I2C_bus_read(sensor1.dev_addr,BNO055_MAG_DATA_X_LSB_VALUEX__REG,rx_tx_buf, BNO055_SIX_U8X, i2c, rx_tx_buf);
		/* Data X*/
		rx_tx_buf[INDEX_ZERO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ZERO],BNO055_MAG_DATA_X_LSB_VALUEX);
		rx_tx_buf[INDEX_ONE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ONE], BNO055_MAG_DATA_X_MSB_VALUEX);
		mag_datax1 = ((((s32)((s8)rx_tx_buf[INDEX_ONE])) << BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_ZERO]));
		x1 = (double)(mag_datax1);// /MAG_DIV_UT);
		/* Data Y*/
		rx_tx_buf[INDEX_TWO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_TWO],BNO055_MAG_DATA_Y_LSB_VALUEY);
		rx_tx_buf[INDEX_THREE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_THREE], BNO055_MAG_DATA_Y_MSB_VALUEY);
		mag_datay1 = ((((s32)((s8)rx_tx_buf[INDEX_THREE])) <<BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_TWO]));
		y1 = (double)(mag_datay1);// /MAG_DIV_UT);
		/* Data Z*/
		rx_tx_buf[INDEX_FOUR] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FOUR],BNO055_MAG_DATA_Z_LSB_VALUEZ);
		rx_tx_buf[INDEX_FIVE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FIVE],BNO055_MAG_DATA_Z_MSB_VALUEZ);
		mag_dataz1 = ((((s32)((s8)rx_tx_buf[INDEX_FIVE])) << BNO055_SHIFT_8_POSITION)| (rx_tx_buf[INDEX_FOUR]));
		z1 = (double)(mag_dataz1);// /MAG_DIV_UT);

		/*Read the six byte value of mag xyz from second sesnor*/
		BNO055_I2C_bus_read(sensor2.dev_addr,BNO055_MAG_DATA_X_LSB_VALUEX__REG,rx_tx_buf, BNO055_SIX_U8X, i2c, rx_tx_buf);
		/* Data X*/
		rx_tx_buf[INDEX_ZERO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ZERO],BNO055_MAG_DATA_X_LSB_VALUEX);
		rx_tx_buf[INDEX_ONE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_ONE], BNO055_MAG_DATA_X_MSB_VALUEX);
		mag_datax2 = ((((s32)((s8)rx_tx_buf[INDEX_ONE])) << BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_ZERO]));
		x2 = (double)(mag_datax2);// /MAG_DIV_UT);
		/* Data Y*/
		rx_tx_buf[INDEX_TWO] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_TWO],BNO055_MAG_DATA_Y_LSB_VALUEY);
		rx_tx_buf[INDEX_THREE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_THREE], BNO055_MAG_DATA_Y_MSB_VALUEY);
		mag_datay2 = ((((s32)((s8)rx_tx_buf[INDEX_THREE])) <<BNO055_SHIFT_8_POSITION) |(rx_tx_buf[INDEX_TWO]));
		y2 = (double)(mag_datay2);// /MAG_DIV_UT);
		/* Data Z*/
		rx_tx_buf[INDEX_FOUR] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FOUR],BNO055_MAG_DATA_Z_LSB_VALUEZ);
		rx_tx_buf[INDEX_FIVE] = BNO055_GET_BITSLICE(rx_tx_buf[INDEX_FIVE],BNO055_MAG_DATA_Z_MSB_VALUEZ);
		mag_dataz2 = ((((s32)((s8)rx_tx_buf[INDEX_FIVE])) << BNO055_SHIFT_8_POSITION)| (rx_tx_buf[INDEX_FOUR]));
		z2 = (double)(mag_dataz2);// /MAG_DIV_UT);

		printf("%f \t %f \t %f \t%f \t%f \t%f\n", x1, y1, z1, x2, y2, z2);
		fflush(stdout);

		if (breakLoop)
		{
			//double seconds_since_start = difftime( time(0), start);
			//printf("Gestures recognised: %d in %lf seconds\n", classifiedCounter, seconds_since_start);
			printf("Exit command received\n");
			break;
		}
		BNO055_delay_msek(30);
	}


	/*-----------------------------------------------------------------------*
	 ************************* START DE-INITIALIZATION ***********************
	 *-------------------------------------------------------------------------*/
	/*	For de - initializing the BNO sensor it is required to the operation mode
     of the sensor as SUSPEND
     Suspend mode can set from the register
     Page - page0
     register - 0x3E
     bit positions - 0 and 1*/
	power_mode = POWER_MODE_SUSPEND; /* set the power mode as SUSPEND*/
	comres1 += bno055_set_power_mode(power_mode,&sensor1, i2c, rx_tx_buf);
	comres2 += bno055_set_power_mode(power_mode,&sensor2, i2c, rx_tx_buf);

	delete i2c;

	/*---------------------------------------------------------------------*
	 ************************* END DE-INITIALIZATION **********************
	 *---------------------------------------------------------------------*/

	return MRAA_SUCCESS;
	return 0;
}

void sig_handler(int signo)
{
	if (signo == SIGINT) {
		printf("closing nicely\n");
		running = -1;
	}
}

void LoopBreaker()
{
	char c;
	while (1)
	{
		scanf("%c", &c);
		if (c == 's')
		{
			breakLoop = true;
			br.detach();
			break;
		}
	}
}