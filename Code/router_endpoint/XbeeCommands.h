#ifndef _XBEE_COMMANDS_
#define _XBEE_COMMANDS_

#include "mbed.h"

#define AT_CMD 						0x08
#define AT_CMD_RSP 				0x88
#define TX_REQUEST 				0x10
#define REMOTE_CMD_RSP 		0x97

#define AT_MSB        		0x00
#define AT_LSB         		0x04
#define TX_MSB    				0x00
#define TX_LSB    				0x17 
#define REMOTE_AT_MSB     0x00
#define REMOTE_AT_LSB     0x10

#define AT_CMD_ID 				0x4944
#define AT_CMD_SC					0x5343
#define AT_CMD_WR					0x5752

#define UINT12_MAX 				4096

#endif