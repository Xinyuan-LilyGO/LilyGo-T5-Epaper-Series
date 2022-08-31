/**
* @file includes.h
* Configuration defines for RF24/Linux
*/

/**
* Example of includes.h for RF24 Linux portability
*
* @defgroup Porting_Includes Porting: Includes
*
*
* @{
*/

#ifndef __RF24_INCLUDES_H__
#define __RF24_INCLUDES_H__

/**
 * Define RF24_WIRINGPI configuration for RaspberryPi platform
 */
#define RF24_WIRINGPI

/**
 * Load the correct configuration for this platform
 */
#include "wiringPi/RF24_arch_config.h"

#endif
/*@}*/