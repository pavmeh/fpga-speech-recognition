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

#ifndef __ALT_UP_ACCELEROMETER_SPI_REGS_H__
#define __ALT_UP_ACCELEROMETER_SPI_REGS_H__
#include <io.h>
#include <alt_types.h>

#define IOWR_ALT_UP_ACCELEROMETER_SPI(base, addr, data)  		\
        IOWR(base, addr, data)
/*
 * Address Register 
 */
#define ALT_UP_ACCELEROMETER_SPI_ADDR_REG	0

#define IOADDR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(base)      	\
        __IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_ACCELEROMETER_SPI_ADDR_REG)
#define IORD_ALT_UP_ACCELEROMETER_SPI_ADDRESS(base)        	\
        IORD_8DIRECT(base, ALT_UP_ACCELEROMETER_SPI_ADDR_REG)
#define IOWR_ALT_UP_ACCELEROMETER_SPI_ADDRESS(base, data)  	\
        IOWR_8DIRECT(base, ALT_UP_ACCELEROMETER_SPI_ADDR_REG, data)

#define ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK			(0x0000003F)
#define ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_OFST			(0)

#define ALT_UP_ACCELEROMETER_SPI_ADDRESS_VALID_MSK  		(ALT_UP_ACCELEROMETER_SPI_ADDRESS_ADDR_MSK)

/*
 * Data Register 
 */
#define ALT_UP_ACCELEROMETER_SPI_DATA_REG		1

#define IOADDR_ALT_UP_ACCELEROMETER_SPI_DATA(base)      	\
        __IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_ACCELEROMETER_SPI_DATA_REG)
#define IORD_ALT_UP_ACCELEROMETER_SPI_DATA(base)        	\
        IORD_8DIRECT(base, ALT_UP_ACCELEROMETER_SPI_DATA_REG)
#define IOWR_ALT_UP_ACCELEROMETER_SPI_DATA(base, data)  	\
        IOWR_8DIRECT(base, ALT_UP_ACCELEROMETER_SPI_DATA_REG, data)

#define ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK				(0x0000FFFF)
#define ALT_UP_ACCELEROMETER_SPI_DATA_DATA_OFST				(0)

#define ALT_UP_ACCELEROMETER_SPI_DATA_VALID_MSK  			(ALT_UP_ACCELEROMETER_SPI_DATA_DATA_MSK)


// Register map address of the Accelerometer
// see the datasheet for details
#define ACCELEROMETER_SPI_ID 						(0x0000000)
#define ACCELEROMETER_SPI_X_AXIS_LOWER_BYTE 	(0x0000032)
#define ACCELEROMETER_SPI_X_AXIS_UPPER_BYTE 	(0x0000033)
#define ACCELEROMETER_SPI_Y_AXIS_LOWER_BYTE 	(0x0000034)
#define ACCELEROMETER_SPI_Y_AXIS_UPPER_BYTE 	(0x0000035)
#define ACCELEROMETER_SPI_Z_AXIS_LOWER_BYTE 	(0x0000036)
#define ACCELEROMETER_SPI_Z_AXIS_UPPER_BYTE 	(0x0000037)

#endif /*__ALT_UP_ACCELEROMETER_SPI_REGS_H__*/
