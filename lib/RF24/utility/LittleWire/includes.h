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
 * Define a specific platform for this configuration
 */
#define LITTLEWIRE

/**
 * Load the correct configuration for this platform
*/
#include "LittleWire/RF24_arch_config.h"

#endif

/*@}*/	
