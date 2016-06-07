/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 1991-2013 Altera Corporation, San Jose, California, USA.      *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#include <errno.h>

#include <priv/alt_file.h>
#include <alt_types.h>

#include "altera_up_avalon_accelerometer_spi.h"
#include "altera_up_avalon_accelerometer_spi_regs.h"

alt_up_accelerometer_spi_dev* alt_up_accelerometer_spi_open_dev(const char* name)
{
  // find the device from the device list 
  // (see altera_hal/HAL/inc/priv/alt_file.h 
  // and altera_hal/HAL/src/alt_find_dev.c 
  // for details)
  alt_up_accelerometer_spi_dev *dev = (alt_up_accelerometer_spi_dev*)alt_find_dev(name, &alt_dev_list);

  return dev;
}

/**
 * @brief Reads configuration data from one of the on-board video device's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- a pointer to the location where the read address should be stored
 *
 * @return 0 for success 
 **/
int alt_up_accelerometer_spi_read_address_register(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 *addr)
{
	// reads data from the device Address register
	*(addr) = IORD_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base); 

	return 0;
}

/**
 * @brief Reads data from the Accelerometer's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- the device's configuration register's address
 * @param data -- a pointer to the location where the read data should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 addr, alt_u8 *data)
{
	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, addr & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(data) = IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK;

	return 0;
}

/**
 * @brief Writes data to the Accelerometer's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- the device's configuration register's address
 * @param data -- the data to be written
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_write(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 addr, alt_u8 data)
{
	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, addr & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// write data to the device Data register
	IOWR_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base, data & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK); 

	return 0;
}

/**
 * @brief Reads the X Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param x_axis -- a pointer to the location where the x axis data should be stored
 *
 * @return 0 for success or -1 for failure
 **/
int alt_up_accelerometer_spi_read_x_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *x_axis)
{
	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_X_AXIS_LOWER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(x_axis) = IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK;

	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_X_AXIS_UPPER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(x_axis) += (IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK) << 8;

	if (*(x_axis) & 0x00008000)
	{
		*(x_axis) |= 0xFFFF0000;
	}

	return 0;
}

/**
 * @brief Reads the Y Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param y_axis -- a pointer to the location where the y axis data should be stored
 *
 * @return 0 for success or -1 for failure
 **/
int alt_up_accelerometer_spi_read_y_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *y_axis)
{
	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_Y_AXIS_LOWER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(y_axis) = IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK;

	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_Y_AXIS_UPPER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(y_axis) += (IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK) << 8;

	if (*(y_axis) & 0x00008000)
	{
		*(y_axis) |= 0xFFFF0000;
	}

	return 0;
}

/**
 * @brief Reads the Z Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param z_axis -- a pointer to the location where the z axis data should be stored
 *
 * @return 0 for success or -1 for failure
 **/
int alt_up_accelerometer_spi_read_z_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *z_axis)
{
	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_Z_AXIS_LOWER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(z_axis) = IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK;

	// set the register address in the Address register
	IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(accel_spi->base, ACCELEROMETER_SPI_Z_AXIS_UPPER_BYTE & ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK); 

	// read data to the device Data register
	*(z_axis) += (IORD_ALT_UP_ACCELEROMETER_SPI_DATA(accel_spi->base) & ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK) << 8;

	if (*(z_axis) & 0x00008000)
	{
		*(z_axis) |= 0xFFFF0000;
	}

	return 0;
}

