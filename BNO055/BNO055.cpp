#include <BNO055.h>

BNO055_RETURN_FUNCTION_TYPE bno055_init(struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8 = BNO055_ZERO_U8X;
	u8 v_page_zero_u8 = PAGE_ZERO;
	/* Array holding the Software revision id
	 */
	u8 a_SW_ID_u8[ARRAY_SIZE_TWO] = {BNO055_ZERO_U8X, BNO055_ZERO_U8X};
	/* Write the default page as zero*/
	com_rslt = BNO055_I2C_bus_write(bno055->dev_addr,BNO055_PAGE_ID__REG, &v_page_zero_u8, BNO055_ONE_U8X, i2c, rx_tx_buf);
	/* Read the chip id of the sensor from page
     zero 0x00 register*/
	com_rslt += BNO055_I2C_bus_read(bno055->dev_addr,BNO055_CHIP_ID__REG, &v_data_u8, BNO055_ONE_U8X, i2c, rx_tx_buf);
	bno055->chip_id = v_data_u8;
	/* Read the accel revision id from page
     zero 0x01 register*/
	//com_rslt += p_bno055->BNO055_BUS_READ_FUNC(p_bno055->dev_addr,BNO055_ACCEL_REV_ID__REG, &v_data_u8, BNO055_ONE_U8X);
	//p_bno055->accel_rev_id = v_data_u8;
	/* Read the mag revision id from page
     zero 0x02 register*/
	com_rslt += BNO055_I2C_bus_read(bno055->dev_addr, BNO055_MAG_REV_ID__REG, &v_data_u8, BNO055_ONE_U8X, i2c, rx_tx_buf);
	bno055->mag_rev_id = v_data_u8;
	/* Read the gyro revision id from page
     zero 0x02 register*/
	//com_rslt += p_bno055->BNO055_BUS_READ_FUNC
	//(p_bno055->dev_addr,
	// BNO055_GYRO_REV_ID__REG, &v_data_u8, BNO055_ONE_U8X);
	//p_bno055->gyro_rev_id = v_data_u8;
	/* Read the boot loader revision from page
     zero 0x06 register*/
	com_rslt += BNO055_I2C_bus_read(bno055->dev_addr,BNO055_BL_REV_ID__REG, &v_data_u8, BNO055_ONE_U8X, i2c, rx_tx_buf);
	bno055->bl_rev_id = v_data_u8;
	/* Read the software revision id from page
     zero 0x04 and 0x05 register( 2 bytes of data)*/
	com_rslt += BNO055_I2C_bus_read(bno055->dev_addr, BNO055_SW_REV_ID_LSB__REG, a_SW_ID_u8, BNO055_TWO_U8X, i2c, rx_tx_buf);
	a_SW_ID_u8[INDEX_ZERO] = BNO055_GET_BITSLICE(a_SW_ID_u8[INDEX_ZERO],BNO055_SW_REV_ID_LSB);
	bno055->sw_rev_id = (u16)((((u32)((u8)a_SW_ID_u8[INDEX_ONE])) << BNO055_SHIFT_8_POSITION) | (a_SW_ID_u8[INDEX_ZERO]));
	/* Read the page id from the register 0x07*/
	com_rslt += BNO055_I2C_bus_read(bno055->dev_addr,BNO055_PAGE_ID__REG, &v_data_u8, BNO055_ONE_U8X, i2c, rx_tx_buf);
	bno055->page_id = v_data_u8;

	return com_rslt;
}


BNO055_RETURN_FUNCTION_TYPE bno055_set_power_mode(u8 v_power_mode_u8, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8r = BNO055_ZERO_U8X;
	u8 v_prev_opmode_u8 = OPERATION_MODE_CONFIG;
	s8 v_stat_s8 = ERROR;
	/* Check the struct p_bno055 is empty */

	/* The write operation effective only if the operation
     mode is in config mode, this part of code is checking the
     current operation mode and set the config mode */
	v_stat_s8 = bno055_get_operation_mode(&v_prev_opmode_u8, bno055, i2c, rx_tx_buf);
	if (v_stat_s8 == SUCCESS)
	{
		if (v_prev_opmode_u8 != OPERATION_MODE_CONFIG)
			v_stat_s8 += bno055_set_operation_mode(OPERATION_MODE_CONFIG, bno055, i2c, rx_tx_buf);
		if (v_stat_s8 == SUCCESS)
		{
			/* Write the value of power mode */
			com_rslt = BNO055_I2C_bus_read(bno055->dev_addr,BNO055_POWER_MODE__REG, &v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
			if (com_rslt == SUCCESS) {
				v_data_u8r = BNO055_SET_BITSLICE(v_data_u8r,BNO055_POWER_MODE, v_power_mode_u8);
				com_rslt += BNO055_I2C_bus_write(bno055->dev_addr,BNO055_POWER_MODE__REG,&v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
			}
		}
		else
		{
			com_rslt = ERROR;
		}
	}
	else
	{
		com_rslt = ERROR;
	}
	if (v_prev_opmode_u8 != OPERATION_MODE_CONFIG)
		/* set the operation mode
     of previous operation mode*/
		com_rslt += bno055_set_operation_mode(v_prev_opmode_u8, bno055, i2c, rx_tx_buf);
	return com_rslt;
}


BNO055_RETURN_FUNCTION_TYPE bno055_set_operation_mode(u8 v_operation_mode_u8, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8r = BNO055_ZERO_U8X;
	u8 v_prev_opmode_u8 = OPERATION_MODE_CONFIG;
	s8 v_stat_s8 = ERROR;
	/* Check the struct p_bno055 is empty */

	/* The write operation effective only if the operation
     mode is in config mode, this part of code is checking the
     current operation mode and set the config mode */
	v_stat_s8 = bno055_get_operation_mode(&v_prev_opmode_u8,bno055, i2c, rx_tx_buf);
	if (v_stat_s8 == SUCCESS)
	{
		/* If the previous operation mode is config it is
         directly write the operation mode */
		if (v_prev_opmode_u8 == OPERATION_MODE_CONFIG)
		{
			com_rslt = BNO055_I2C_bus_read(bno055->dev_addr, BNO055_OPERATION_MODE__REG,&v_data_u8r,BNO055_ONE_U8X, i2c, rx_tx_buf);
			if (com_rslt == SUCCESS)
			{
				v_data_u8r =
						BNO055_SET_BITSLICE(v_data_u8r, BNO055_OPERATION_MODE,v_operation_mode_u8);
				com_rslt += BNO055_I2C_bus_write(bno055->dev_addr,BNO055_OPERATION_MODE__REG,&v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
				/* Config mode to other
                 operation mode switching
                 required delay of 600ms*/
				BNO055_delay_msek(BNO055_SIX_HUNDRES_U8X);
			}
		}
		else
		{
			/* If the previous operation
             mode is not config it is
             write the config mode */
			com_rslt = BNO055_I2C_bus_read(bno055->dev_addr, BNO055_OPERATION_MODE__REG, &v_data_u8r,BNO055_ONE_U8X, i2c, rx_tx_buf);
			if (com_rslt == SUCCESS)
			{
				v_data_u8r =
						BNO055_SET_BITSLICE(v_data_u8r, BNO055_OPERATION_MODE, OPERATION_MODE_CONFIG);
				com_rslt += bno055_write_register(BNO055_OPERATION_MODE__REG,&v_data_u8r, BNO055_ONE_U8X, bno055, i2c, rx_tx_buf);
				/* other mode to config mode switching
                 required delay of 20ms*/
				BNO055_delay_msek(BNO055_TWENTY_U8X);
			}
			/* Write the operation mode */
			if (v_operation_mode_u8 != OPERATION_MODE_CONFIG)
			{
				com_rslt = BNO055_I2C_bus_read(bno055->dev_addr,BNO055_OPERATION_MODE__REG,&v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
				if (com_rslt == SUCCESS)
				{
					v_data_u8r = BNO055_SET_BITSLICE(v_data_u8r,BNO055_OPERATION_MODE,v_operation_mode_u8);
					com_rslt +=BNO055_I2C_bus_write(bno055->dev_addr, BNO055_OPERATION_MODE__REG, &v_data_u8r,BNO055_ONE_U8X, i2c, rx_tx_buf);
					/* Config mode to other
                     operation mode switching
                     required delay of 600ms*/
					BNO055_delay_msek(BNO055_SIX_HUNDRES_U8X);
				}
			}
		}
	}
	else
	{
		com_rslt = ERROR;
	}

	return com_rslt;
}


BNO055_RETURN_FUNCTION_TYPE bno055_get_operation_mode(u8 *v_operation_mode_u8, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8r = BNO055_ZERO_U8X;
	s8 v_stat_s8 = ERROR;
	/*condition check for page, operation mode is
     available in the page zero*/
	if (bno055->page_id != PAGE_ZERO)
		/* Write the page zero*/
		v_stat_s8 = bno055_write_page_id(PAGE_ZERO, bno055, i2c, rx_tx_buf);
	if ((v_stat_s8 == SUCCESS) || (bno055->page_id == PAGE_ZERO))
	{
		/* Read the value of operation mode*/
		com_rslt = BNO055_I2C_bus_read(bno055->dev_addr,BNO055_OPERATION_MODE__REG, &v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
		*v_operation_mode_u8 = BNO055_GET_BITSLICE(v_data_u8r,BNO055_OPERATION_MODE);
	}
	else
	{
		com_rslt = ERROR;
	}
	return com_rslt;
}

BNO055_RETURN_FUNCTION_TYPE bno055_write_page_id(u8 v_page_id_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8r = BNO055_ZERO_U8X;

	/* Read the current page*/
	com_rslt = BNO055_I2C_bus_read(bno055->dev_addr,BNO055_PAGE_ID__REG, &v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
	/* Check condition for communication success*/
	if (com_rslt == SUCCESS) {
		v_data_u8r = BNO055_SET_BITSLICE(v_data_u8r, BNO055_PAGE_ID, v_page_id_u8);
		/* Write the page id*/
		com_rslt += BNO055_I2C_bus_write(bno055->dev_addr,BNO055_PAGE_ID__REG,&v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
		if (com_rslt == SUCCESS)
			bno055->page_id = v_page_id_u8;
	}
	else
	{
		com_rslt = ERROR;
	}

	return com_rslt;
}

BNO055_RETURN_FUNCTION_TYPE bno055_write_register(u8 v_addr_u8, u8 *p_data_u8, u8 v_len_u8,struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	/* Check the struct p_bno055 is empty */
	/* Write the values of respective given register */
	com_rslt = BNO055_I2C_bus_write(bno055->dev_addr, v_addr_u8, p_data_u8, v_len_u8, i2c, rx_tx_buf);

	return com_rslt;
}

/*!
 *	@brief This API used to read
 *	mag calibration status from register from 0x35 bit 0 and 1
 *
 *	@param v_mag_calib_u8 : The value of mag calib status
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval 1 -> Error
 *
 */
BNO055_RETURN_FUNCTION_TYPE bno055_get_mag_calib_stat(u8 *v_mag_calib_u8, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	u8 v_data_u8r = BNO055_ZERO_U8X;
	s8 v_stat_s8 = ERROR;
	/* Check the struct p_bno055 is empty */
	/*condition check for page, mag calib
         available in the page zero*/
	if (bno055->page_id != PAGE_ZERO)
		/* Write the page zero*/
		v_stat_s8 = bno055_write_page_id(PAGE_ZERO, bno055, i2c, rx_tx_buf);
	if ((v_stat_s8 == SUCCESS) ||(bno055->page_id == PAGE_ZERO))
	{
		/* Read the mag calib v_stat_s8 */
		com_rslt = BNO055_I2C_bus_read(bno055->dev_addr,BNO055_MAG_CALIB_STAT__REG,&v_data_u8r, BNO055_ONE_U8X, i2c, rx_tx_buf);
		*v_mag_calib_u8 =BNO055_GET_BITSLICE(v_data_u8r,BNO055_MAG_CALIB_STAT);
	}
	else
	{
		com_rslt = ERROR;
	}

	return com_rslt;
}

BNO055_RETURN_FUNCTION_TYPE bno055_read_mag_offset(struct bno055_mag_offset_t  *mag_offset, struct bno055_t *bno055, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	/* Variable used to return value of
     communication routine*/
	BNO055_RETURN_FUNCTION_TYPE com_rslt = ERROR;
	/* Array holding the mag offset values
     v_data_u8[INDEX_ZERO] - offset x->LSB
     v_data_u8[INDEX_ONE] - offset x->MSB
     v_data_u8[INDEX_TWO] - offset y->LSB
     v_data_u8[INDEX_THREE] - offset y->MSB
     v_data_u8[INDEX_FOUR] - offset z->LSB
     v_data_u8[INDEX_FIVE] - offset z->MSB
	 */
	u8 v_data_u8[ARRAY_SIZE_SIX] = {
			BNO055_ZERO_U8X, BNO055_ZERO_U8X,
			BNO055_ZERO_U8X, BNO055_ZERO_U8X,
			BNO055_ZERO_U8X, BNO055_ZERO_U8X};
	s8 v_stat_s8 = ERROR;
	/* Check the struct p_bno055 is empty */

	/*condition check for page, mag offset is
        available in the page zero*/
	if (bno055->page_id != PAGE_ZERO)
		/* Write the page zero*/
		v_stat_s8 = bno055_write_page_id(PAGE_ZERO, bno055, i2c, rx_tx_buf);
	if ((v_stat_s8 == SUCCESS) ||(bno055->page_id == PAGE_ZERO))
	{
		/* Read mag offset value it the six bytes of data */
		com_rslt = BNO055_I2C_bus_read(bno055->dev_addr, BNO055_MAG_OFFSET_X_LSB__REG,v_data_u8, BNO055_SIX_U8X, i2c, rx_tx_buf);
		if (com_rslt == SUCCESS) {
			/* Read mag x offset value*/
			v_data_u8[INDEX_ZERO] = BNO055_GET_BITSLICE(v_data_u8[INDEX_ZERO],BNO055_MAG_OFFSET_X_LSB);
			v_data_u8[INDEX_ONE] = BNO055_GET_BITSLICE(v_data_u8[INDEX_ONE],BNO055_MAG_OFFSET_X_MSB);
			mag_offset->x = (s16)((((s32)(s8)(v_data_u8[INDEX_ONE])) <<(BNO055_SHIFT_8_POSITION)) |(v_data_u8[INDEX_ZERO]));

			/* Read mag y offset value*/
			v_data_u8[INDEX_TWO] = BNO055_GET_BITSLICE(v_data_u8[INDEX_TWO],BNO055_MAG_OFFSET_Y_LSB);
			v_data_u8[INDEX_THREE] =BNO055_GET_BITSLICE(v_data_u8[INDEX_THREE],BNO055_MAG_OFFSET_Y_MSB);
			mag_offset->y = (s16)((((s32)(s8)(v_data_u8[INDEX_THREE])) <<(BNO055_SHIFT_8_POSITION))| (v_data_u8[INDEX_TWO]));

			/* Read mag z offset value*/
			v_data_u8[INDEX_FOUR] = BNO055_GET_BITSLICE(v_data_u8[INDEX_FOUR], BNO055_MAG_OFFSET_Z_LSB);
			v_data_u8[INDEX_FIVE] = BNO055_GET_BITSLICE(v_data_u8[INDEX_FIVE], BNO055_MAG_OFFSET_Z_MSB);
			mag_offset->z = (s16)((((s32)(s8)(v_data_u8[INDEX_FIVE])) << (BNO055_SHIFT_8_POSITION)) | (v_data_u8[INDEX_FOUR]));

			/* Read mag radius value
                 it the two bytes of data */
			com_rslt += BNO055_I2C_bus_read(bno055->dev_addr, BNO055_MAG_RADIUS_LSB__REG, v_data_u8, BNO055_TWO_U8X, i2c, rx_tx_buf);
			if (com_rslt == SUCCESS) {
				/* Array holding the mag radius values
                     v_data_u8[INDEX_ZERO] - radius->LSB
                     v_data_u8[INDEX_ONE] - radius->MSB
				 */
				v_data_u8[INDEX_ZERO] = BNO055_GET_BITSLICE(v_data_u8[INDEX_ZERO],BNO055_MAG_RADIUS_LSB);
				v_data_u8[INDEX_ONE] = BNO055_GET_BITSLICE(v_data_u8[INDEX_ONE], BNO055_MAG_RADIUS_MSB);
				mag_offset->r = (s16)((((s32)(s8)(v_data_u8[INDEX_ONE])) << (BNO055_SHIFT_8_POSITION)) | (v_data_u8[INDEX_ZERO]));
			}
			else
			{
				com_rslt = ERROR;
			}
		}
		else
		{
			com_rslt = ERROR;
		}
	}

	return com_rslt;
}

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{

	//uint8_t rx_tx_buf[ARRAY_SIZE_SIX];
	i2c->address(dev_addr);
	rx_tx_buf[0] = reg_addr;
	rx_tx_buf[1] = *reg_data;
	//Result status = 
	i2c->write(rx_tx_buf, 2);

	//if (status == MRAA_SUCCESS)
	return SUCCESS;
	//else
	//    return (s8)iError;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt, I2c* i2c, uint8_t rx_tx_buf[ARRAY_SIZE_SIX])
{
	uint8_t reg = reg_addr;
	int size = cnt;
	i2c->address(dev_addr);
	i2c->writeByte(reg);

	i2c->address(dev_addr);
	i2c->read(rx_tx_buf, size);

	//if (status == MRAA_SUCCESS)
	return SUCCESS;
	//else
	//    return (s8)iError;
}

void BNO055_delay_msek(u32 msek)
{
	int ms = msek;
	/*Here you can write your own delay routine*/
	usleep(1000*ms);
}