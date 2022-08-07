#ifndef __IAP_H
#define __IAP_H

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "usart.h"
#include "crc.h"

/* Uart Port Config */
#define C_UART	&huart1		/* Define UART Controller to communicate to Host */
#define D_UART	NULL			/* Define UART	Debuger to communicate to Host */
#define OTA_SIZE  0x18000
/*  Address flash OTA size  */
#define OTA_SLOT_1_ADDRESS (uint32_t)0x08008000
#define OTA_SLOT_2_ADDRESS (uint32_t)0x08020000
#define OTA_SLOT_SIZE	(uint32_t)0x00000000
#define CRC_1_ADDRESS	(uint32_t)0x00000000
#define CRC_2_ADDRESS	(uint32_t)0x00000000


/*  Address flash flag  */
#define SLOT_ACTIVATE_ADDRESS	(uint32_t)0x00000000
#define STATUS_OTA_ADDRESS	(uint32_t)0x00000000
#define STATUS_TEST_ADDRESS	(uint32_t)0x00000000


/*  Bootloader error code  */
enum eBootloaderErrorCode {

	BL_OK = 0,			/* No error */
	BL_NO_APP,			/* No application found in flash */
	BL_SIZE_ERROR,	/* New application larger for flash */
	BL_CRC_ERROR,		/* Check sum error */
	BL_ERASE_ERROR,	/* Erese flash error */
	BL_WRITE_ERROR,	/* Write flash error */
	
};

typedef struct {

	uint32_t flash_base_address;			/*  Base address flash OTA slot  */
	uint32_t ota_size;								/*  Data receiver OTA size  */
	uint32_t ota_receive;							/*  Data receiver count  */

} OTA_SLOT_t;												/*  Type slot update OTA  */

/* Chunk data */
#define CHUNK	150

/*  OTA UART CODE  */
#define CMD_ERASE					0x01
#define CMD_START					0x02
#define CMD_REQUEST				0x03
#define CMD_WRITE					0x04
#define CMD_END						0x05
#define ACK								0x11
#define NACK							0x22


/* Function */
void Bootloader_UartReadData(void);

void Bootloader_Erase(void);
void Bootloader_CheckSum(void);

void BootloaderFlash_Begin(uint8_t *buff);
void BootloaderFlash_Write(uint8_t *buff);
void BootloaderFlash_Finish(void);

void Bootloader_JumToApplication(void);
void Bootloader_Deinit(void);
void GetOTA_config(void);
void send_ACK(void);
void send_NACK(void);



#endif
