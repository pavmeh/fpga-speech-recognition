#ifndef __ALTERA_UP_AVALON_ACCELEROMETER_SPI_H__
#define __ALTERA_UP_AVALON_ACCELEROMETER_SPI_H__

#include <stddef.h>
#include <alt_types.h>
#include <sys/alt_dev.h>

#include "altera_up_avalon_accelerometer_spi_regs.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////
// 
/*
 * Device structure definition. Each instance of the driver uses one
 * of these structures to hold its associated state.
 */
typedef struct alt_up_accelerometer_spi_dev {
	/// @brief character mode device structure .
	/// @sa Developing Device Drivers for the HAL in Nios II Software Developer's Handbook.
	alt_dev dev;
	/// @brief the base address of the device.
	unsigned int base;
} alt_up_accelerometer_spi_dev;

//////////////////////////////////////////////////////////////////////////
// HAL system functions

//////////////////////////////////////////////////////////////////////////
// direct operation functions

/*
 * Macros used by alt_sys_init 
 */
#define ALTERA_UP_AVALON_ACCELEROMETER_SPI_INSTANCE(name, device)	\
  static alt_up_accelerometer_spi_dev device =									\
  {                                            				 	    	\
    {                                        				 	     	\
      ALT_LLIST_ENTRY,                         				 	    	\
      name##_NAME,                           	 			 	     	\
      NULL , 	/* open  */												\
      NULL , 	/* close */												\
      NULL , 	/* read  */												\
      NULL , 	/* write */												\
      NULL , 	/* lseek */												\
      NULL , 	/* fstat */												\
      NULL , 	/* ioctl */												\
    },                                  				            	\
    name##_BASE                               					 		\
  }

#define ALTERA_UP_AVALON_ACCELEROMETER_SPI_INIT(name, device)	\
{																	\
    alt_dev_reg(&device.dev);                       			   	\
}

/**
 * @brief Opens the Accelerometer SPI Mode device specified by <em> name </em>.
 *
 * @param name -- the Accelerometer SPI Mode component name in SOPC Builder. 
 *
 * @return The corresponding device structure, or NULL if the device is not found.
 **/
alt_up_accelerometer_spi_dev* alt_up_accelerometer_spi_open_dev(const char* name);

/**
 * @brief Reads configuration data from one of the on-board video device's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- a pointer to the location where the read address should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read_address_register(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 *addr);

/**
 * @brief Reads data from the Accelerometer's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- the device's configuration register's address
 * @param data -- a pointer to the location where the read data should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 addr, alt_u8 *data);

/**
 * @brief Writes data to the Accelerometer's registers.
 *
 * @param accel_spi -- the device structure 
 * @param addr -- the device's configuration register's address
 * @param data -- the data to be written.
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_write(alt_up_accelerometer_spi_dev *accel_spi, alt_u8 addr, alt_u8 data);

/**
 * @brief Reads the X Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param x_axis -- a pointer to the location where the x axis data should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read_x_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *x_axis);

/**
 * @brief Reads the Y Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param y_axis -- a pointer to the location where the y axis data should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read_y_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *y_axis);

/**
 * @brief Reads the Z Axis value from both registers from the Accelerometer and converts the value to a signed integer.
 *
 * @param accel_spi -- the device structure 
 * @param z_axis -- a pointer to the location where the z axis data should be stored
 *
 * @return 0 for success
 **/
int alt_up_accelerometer_spi_read_z_axis(alt_up_accelerometer_spi_dev *accel_spi, alt_32 *z_axis);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ALTERA_UP_AVALON_ACCELEROMETER_SPI_H__ */


