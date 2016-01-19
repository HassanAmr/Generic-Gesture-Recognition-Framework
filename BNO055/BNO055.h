#include "mraa.hpp"

using namespace mraa;

/*signed integer types*/
typedef	signed char  s8;/**< used for signed 8bit */
typedef	signed short int s16;/**< used for signed 16bit */
typedef	signed int s32;/**< used for signed 32bit */
typedef	signed long long int s64;/**< used for signed 64bit */

/*unsigned integer types*/
typedef	unsigned char u8;/**< used for unsigned 8bit */
typedef	unsigned short int u16;/**< used for unsigned 16bit */
typedef	unsigned int u32;/**< used for unsigned 32bit */
typedef	unsigned long long int u64;/**< used for unsigned 64bit */

/********************************************************/
/**\name	I2C ADDRESS DEFINITION FOR BNO055           */
/********************************************************/


/***************************************************/
/**\name	CONSTANT DEFINITIONS                   */
/***************************************************/
#define         BNO055_ZERO_U8X           ((u8)0)
#define         BNO055_ONE_U8X           ((u8)1)
#define         BNO055_TWO_U8X			  ((u8)2)
#define         BNO055_FOUR_U8X           ((u8)4)
#define         BNO055_FIVE_U8X           ((u8)5)
#define         BNO055_SIX_U8X            ((u8)6)
#define         BNO055_SEVEN_U8X          ((u8)7)
#define         BNO055_ELEVEN_U8X         ((u8)11)
#define         BNO055_SIXTEEN_U8X        ((u8)16)
#define			BNO055_EIGHT_U8X		  ((u8)8)
#define			BNO055_TWENTY_U8X         ((u8)20)
#define			BNO055_EIGHTEEN_U8X       ((u8)18)


/*< This refers BNO055 return type as s8 */
#define BNO055_RETURN_FUNCTION_TYPE        s8


/* Power mode*/
#define POWER_MODE_NORMAL	0X00
#define POWER_MODE_LOWPOWER	0X01
#define POWER_MODE_SUSPEND	0X02

/* Operation mode settings*/
#define OPERATION_MODE_CONFIG			0x00
#define OPERATION_MODE_ACCONLY			0x01
#define OPERATION_MODE_MAGONLY			0x02
#define OPERATION_MODE_GYRONLY			0x03
#define OPERATION_MODE_ACCMAG			0x04
#define OPERATION_MODE_ACCGYRO			0x05
#define OPERATION_MODE_MAGGYRO			0x06
#define OPERATION_MODE_AMG				0x07
#define OPERATION_MODE_IMUPLUS			0x08
#define OPERATION_MODE_COMPASS			0x09
#define OPERATION_MODE_M4G				0x0A
#define OPERATION_MODE_NDOF_FMC_OFF		0x0B
#define OPERATION_MODE_NDOF				0x0C

/*  BNO055 API error codes */
#define E_NULL_PTR                  ((s8)-127)
#define E_BNO055_OUT_OF_RANGE       ((s8)-2)
#define	SUCCESS						((u8)0)
#define	ERROR                       ((s8)-1)

/* Page ID */
#define PAGE_ZERO		0x00
#define PAGE_ONE		0x01

/* Mode registers*/
#define BNO055_OPR_MODE_ADDR				0x3D
#define BNO055_PWR_MODE_ADDR				0x3E

/* Page id register definition*/
#define BNO055_PAGE_ID_ADDR				    0X07
/* PAGE0 REGISTER DEFINITION START*/
#define BNO055_CHIP_ID_ADDR                 0x00
#define BNO055_ACCEL_REV_ID_ADDR			0x01
#define BNO055_MAG_REV_ID_ADDR              0x02
#define BNO055_GYRO_REV_ID_ADDR             0x03
#define BNO055_SW_REV_ID_LSB_ADDR			0x04
#define BNO055_SW_REV_ID_MSB_ADDR			0x05
#define BNO055_BL_REV_ID_ADDR				0x06

/* Chip ID */
#define BNO055_CHIP_ID__POS             0
#define BNO055_CHIP_ID__MSK             0xFF
#define BNO055_CHIP_ID__LEN             8
#define BNO055_CHIP_ID__REG             BNO055_CHIP_ID_ADDR

/*Operation Mode data register*/
#define BNO055_OPERATION_MODE__POS			0
#define BNO055_OPERATION_MODE__MSK			0x0F
#define BNO055_OPERATION_MODE__LEN			4
#define BNO055_OPERATION_MODE__REG			BNO055_OPR_MODE_ADDR

/* Power Mode register*/
#define BNO055_POWER_MODE__POS             0
#define BNO055_POWER_MODE__MSK             0x03
#define BNO055_POWER_MODE__LEN             2
#define BNO055_POWER_MODE__REG             BNO055_PWR_MODE_ADDR

/*Page id*/
#define BNO055_PAGE_ID__POS             0
#define BNO055_PAGE_ID__MSK             0xFF
#define BNO055_PAGE_ID__LEN             8
#define BNO055_PAGE_ID__REG             BNO055_PAGE_ID_ADDR

/* Mag revision id*/
#define BNO055_MAG_REV_ID__POS             0
#define BNO055_MAG_REV_ID__MSK             0xFF
#define BNO055_MAG_REV_ID__LEN             8
#define BNO055_MAG_REV_ID__REG             BNO055_MAG_REV_ID_ADDR

/*Software revision id LSB*/
#define BNO055_SW_REV_ID_LSB__POS             0
#define BNO055_SW_REV_ID_LSB__MSK             0xFF
#define BNO055_SW_REV_ID_LSB__LEN             8
#define BNO055_SW_REV_ID_LSB__REG             BNO055_SW_REV_ID_LSB_ADDR

/* BOOTLODER revision id*/
#define BNO055_BL_REV_ID__POS             0
#define BNO055_BL_REV_ID__MSK             0xFF
#define BNO055_BL_REV_ID__LEN             8
#define BNO055_BL_REV_ID__REG             BNO055_BL_REV_ID_ADDR

/*Mag_config address register*/
#define MAG_CONFIG_ADDR					0x09
#define MAG_DATA_OUTPUT_RATE_30HZ		0x07
#define MAG_OPR_MODE_HIGH_ACCURACY		0x03
#define MAG_POWER_MODE_NORMAL			0x00


#define INDEX_ZERO		0
#define INDEX_ONE		1
#define INDEX_TWO		2
#define INDEX_THREE		3
#define INDEX_FOUR		4
#define INDEX_FIVE		5
#define INDEX_SIX		6
#define INDEX_SEVEN		7
#define INDEX_EIGHT		8

#define ARRAY_SIZE_TWO		2
#define ARRAY_SIZE_THREE	3
#define ARRAY_SIZE_SIX		6
#define ARRAY_SIZE_FIVE		5
#define ARRAY_SIZE_EIGHT	8

#define         BNO055_SHIFT_8_POSITION	   ((u8)8)

/*Mag division factor*/
#define MAG_DIV_UT	16.0

#define	I2C_BUFFER_LEN 8

#define	BNO055_SIX_HUNDRES_U8X	600

/**\name GET AND SET BITSLICE FUNCTIONS    */
/*************************************************/
#define BNO055_GET_BITSLICE(regvar, bitname)\
		((regvar & bitname##__MSK) >> bitname##__POS)


#define BNO055_SET_BITSLICE(regvar, bitname, val)\
		((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

/*Mag data register*/
#define BNO055_MAG_DATA_X_LSB_ADDR			0X0E
#define BNO055_MAG_DATA_X_MSB_ADDR			0X0F
#define BNO055_MAG_DATA_Y_LSB_ADDR			0X10
#define BNO055_MAG_DATA_Y_MSB_ADDR			0X11
#define BNO055_MAG_DATA_Z_LSB_ADDR			0X12
#define BNO055_MAG_DATA_Z_MSB_ADDR			0X13

/*Mag status register*/
#define BNO055_CALIB_STAT_ADDR				0X35

/* Magnetometer Offset registers*/
#define MAG_OFFSET_X_LSB_ADDR				0X5B
#define MAG_OFFSET_X_MSB_ADDR				0X5C
#define MAG_OFFSET_Y_LSB_ADDR				0X5D
#define MAG_OFFSET_Y_MSB_ADDR				0X5E
#define MAG_OFFSET_Z_LSB_ADDR				0X5F
#define MAG_OFFSET_Z_MSB_ADDR				0X60

/* Mag data X-LSB register*/
#define BNO055_MAG_DATA_X_LSB_VALUEX__POS             0
#define BNO055_MAG_DATA_X_LSB_VALUEX__MSK             0xFF
#define BNO055_MAG_DATA_X_LSB_VALUEX__LEN             8
#define BNO055_MAG_DATA_X_LSB_VALUEX__REG             \
		BNO055_MAG_DATA_X_LSB_ADDR

/* Mag data X-MSB register*/
#define BNO055_MAG_DATA_X_MSB_VALUEX__POS             0
#define BNO055_MAG_DATA_X_MSB_VALUEX__MSK             0xFF
#define BNO055_MAG_DATA_X_MSB_VALUEX__LEN             8
#define BNO055_MAG_DATA_X_MSB_VALUEX__REG             BNO055_MAG_DATA_X_MSB_ADDR

/* Mag data Y-LSB register*/
#define BNO055_MAG_DATA_Y_LSB_VALUEY__POS             0
#define BNO055_MAG_DATA_Y_LSB_VALUEY__MSK             0xFF
#define BNO055_MAG_DATA_Y_LSB_VALUEY__LEN             8
#define BNO055_MAG_DATA_Y_LSB_VALUEY__REG             BNO055_MAG_DATA_Y_LSB_ADDR

/* Mag data Y-MSB register*/
#define BNO055_MAG_DATA_Y_MSB_VALUEY__POS             0
#define BNO055_MAG_DATA_Y_MSB_VALUEY__MSK             0xFF
#define BNO055_MAG_DATA_Y_MSB_VALUEY__LEN             8
#define BNO055_MAG_DATA_Y_MSB_VALUEY__REG             BNO055_MAG_DATA_Y_MSB_ADDR

/* Mag data Z-LSB register*/
#define BNO055_MAG_DATA_Z_LSB_VALUEZ__POS             0
#define BNO055_MAG_DATA_Z_LSB_VALUEZ__MSK             0xFF
#define BNO055_MAG_DATA_Z_LSB_VALUEZ__LEN             8
#define BNO055_MAG_DATA_Z_LSB_VALUEZ__REG             BNO055_MAG_DATA_Z_LSB_ADDR

/* Mag data Z-MSB register*/
#define BNO055_MAG_DATA_Z_MSB_VALUEZ__POS             0
#define BNO055_MAG_DATA_Z_MSB_VALUEZ__MSK             0xFF
#define BNO055_MAG_DATA_Z_MSB_VALUEZ__LEN             8
#define BNO055_MAG_DATA_Z_MSB_VALUEZ__REG             BNO055_MAG_DATA_Z_MSB_ADDR

/*Mag_Calib status register*/
#define BNO055_MAG_CALIB_STAT__POS             0
#define BNO055_MAG_CALIB_STAT__MSK             0X03
#define BNO055_MAG_CALIB_STAT__LEN             2
#define BNO055_MAG_CALIB_STAT__REG             BNO055_CALIB_STAT_ADDR

//*Mag Offset registers*/
#define BNO055_MAG_OFFSET_X_LSB__POS		0
#define BNO055_MAG_OFFSET_X_LSB__MSK		0XFF
#define BNO055_MAG_OFFSET_X_LSB__LEN		8
#define BNO055_MAG_OFFSET_X_LSB__REG		MAG_OFFSET_X_LSB_ADDR

#define BNO055_MAG_OFFSET_X_MSB__POS		0
#define BNO055_MAG_OFFSET_X_MSB__MSK		0XFF
#define BNO055_MAG_OFFSET_X_MSB__LEN		8
#define BNO055_MAG_OFFSET_X_MSB__REG		MAG_OFFSET_X_MSB_ADDR

#define BNO055_MAG_OFFSET_Y_LSB__POS		0
#define BNO055_MAG_OFFSET_Y_LSB__MSK		0XFF
#define BNO055_MAG_OFFSET_Y_LSB__LEN		8
#define BNO055_MAG_OFFSET_Y_LSB__REG		MAG_OFFSET_Y_LSB_ADDR

#define BNO055_MAG_OFFSET_Y_MSB__POS		0
#define BNO055_MAG_OFFSET_Y_MSB__MSK		0XFF
#define BNO055_MAG_OFFSET_Y_MSB__LEN		8
#define BNO055_MAG_OFFSET_Y_MSB__REG		MAG_OFFSET_Y_MSB_ADDR

#define BNO055_MAG_OFFSET_Z_LSB__POS		0
#define BNO055_MAG_OFFSET_Z_LSB__MSK		0XFF
#define BNO055_MAG_OFFSET_Z_LSB__LEN		8
#define BNO055_MAG_OFFSET_Z_LSB__REG		MAG_OFFSET_Z_LSB_ADDR

#define BNO055_MAG_OFFSET_Z_MSB__POS		0
#define BNO055_MAG_OFFSET_Z_MSB__MSK		0XFF
#define BNO055_MAG_OFFSET_Z_MSB__LEN		8
#define BNO055_MAG_OFFSET_Z_MSB__REG		MAG_OFFSET_Z_MSB_ADDR

/* Radius registers*/
#define	MAG_RADIUS_LSB_ADDR					0X69
#define	MAG_RADIUS_MSB_ADDR					0X6A

/* Radius register definition*/
#define BNO055_MAG_RADIUS_LSB__POS		0
#define BNO055_MAG_RADIUS_LSB__MSK		0XFF
#define BNO055_MAG_RADIUS_LSB__LEN		8
#define BNO055_MAG_RADIUS_LSB__REG		MAG_RADIUS_LSB_ADDR

#define BNO055_MAG_RADIUS_MSB__POS		0
#define BNO055_MAG_RADIUS_MSB__MSK		0XFF
#define BNO055_MAG_RADIUS_MSB__LEN		8
#define BNO055_MAG_RADIUS_MSB__REG		MAG_RADIUS_MSB_ADDR

//end of Definitions

//Funcions
BNO055_RETURN_FUNCTION_TYPE bno055_init(struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
void BNO055_delay_msek(u32 msek);


BNO055_RETURN_FUNCTION_TYPE bno055_set_power_mode(u8 v_power_mode_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
BNO055_RETURN_FUNCTION_TYPE bno055_set_operation_mode(u8 v_operation_mode_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
BNO055_RETURN_FUNCTION_TYPE bno055_get_operation_mode(u8 *v_operation_mode_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
BNO055_RETURN_FUNCTION_TYPE bno055_write_page_id(u8 v_page_id_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
BNO055_RETURN_FUNCTION_TYPE bno055_write_register(u8 v_addr_u8, u8 *p_data_u8, u8 v_len_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
BNO055_RETURN_FUNCTION_TYPE bno055_get_mag_calib_stat(u8 *v_mag_calib_u8, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf);
BNO055_RETURN_FUNCTION_TYPE bno055_read_mag_offset(struct bno055_mag_offset_t  *mag_offset, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX]);
//End of functions

//Structs
struct bno055_t {
	u8 chip_id;/**< chip_id of bno055 */
	u16 sw_rev_id;/**< software revision id of bno055 */
	u8 page_id;/**< page_id of bno055 */
	u8 accel_rev_id;/**< accel revision id of bno055 */
	u8 mag_rev_id;/**< mag revision id of bno055 */
	u8 gyro_rev_id;/**< gyro revision id of bno055 */
	u8 bl_rev_id;/**< boot loader revision id of bno055 */
	u8 dev_addr;/**< i2c device address of bno055 */
	//BNO055_WR_FUNC_PTR;/**< bus write function pointer */
	//BNO055_RD_FUNC_PTR;/**<bus read function pointer */
	//void (*delay_msec)(BNO055_MDELAY_DATA_TYPE);/**< delay function pointer */
};

struct bno055_mag_offset_t {
	s16 x;/**< Mag offset x data */
	s16 y;/**< Mag offset y data */
	s16 z;/**< Mag offset z data */
	s16 r;/**< Mag radius x data */
};