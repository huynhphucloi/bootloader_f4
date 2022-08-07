#include "IAP.h"

uint8_t bl_rx_buffer [200];
uint8_t rcv_len=0;

OTA_SLOT_t ota;

	uint32_t crc = 0;
	uint32_t checkSum = 0;

void Bootloader_UartReadData(){

	ota.flash_base_address = OTA_SLOT_1_ADDRESS;
	

	while(1)
	{
		memset(bl_rx_buffer,0,200);
    HAL_UART_Receive(C_UART,bl_rx_buffer,1,HAL_MAX_DELAY);
		rcv_len= bl_rx_buffer[0];
		HAL_UART_Receive(C_UART,&bl_rx_buffer[1],rcv_len,HAL_MAX_DELAY);
		switch(bl_rx_buffer[1])
		{
            case CMD_ERASE:
                Bootloader_Erase();
                break;
						case CMD_START:
								BootloaderFlash_Begin(bl_rx_buffer);
								break;
						case CMD_WRITE:
								BootloaderFlash_Write(bl_rx_buffer);
								break;
						case CMD_END:
								BootloaderFlash_Finish();
								break;
            default:
								break;

		}

	}

};

void Bootloader_Erase(){
	
	FLASH_EraseInitTypeDef flashErase_handle;
	uint32_t sectorError;
	HAL_StatusTypeDef status;
	
	flashErase_handle.TypeErase = FLASH_TYPEERASE_SECTORS;
	flashErase_handle.Sector = 2;			/* start erase sector */
	flashErase_handle.NbSectors = 3; 	/* number sector erase */
	
	flashErase_handle.Banks = FLASH_BANK_1;
	
	HAL_FLASH_Unlock();
	flashErase_handle.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	status = (uint8_t) HAL_FLASHEx_Erase(&flashErase_handle, &sectorError);
	HAL_FLASH_Lock();
	ota.ota_receive = 0;
	if(status == HAL_OK){
		send_ACK();
	} else {
		send_NACK();
	}
};


void BootloaderFlash_Begin(uint8_t *buff){
	
	ota.ota_size = * (uint32_t *) &buff[2];
	ota.ota_receive = 0;
	if( ota.ota_size > OTA_SIZE){
	
		send_NACK();
	
	} else {
	
		send_ACK();
		
	};

};

void BootloaderFlash_Write(uint8_t *buff){
	ota.flash_base_address = OTA_SLOT_1_ADDRESS;
	HAL_StatusTypeDef status;
	uint8_t len = buff[2];
	
	HAL_FLASH_Unlock();
	for(uint8_t i = 0; i < len; i++){
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ota.flash_base_address + ota.ota_receive, buff[3+i]);
		ota.ota_receive += 1;
	}
	
	HAL_FLASH_Lock();
	if(status == HAL_OK){
		send_ACK();
	} else {
	
		send_NACK();
	
	}

};

void BootloaderFlash_Finish(){
	send_ACK();
	Bootloader_Deinit();
	Bootloader_CheckSum();
	Bootloader_JumToApplication();

};

void GetOTA_config(){

	ota.flash_base_address = OTA_SLOT_1_ADDRESS;

};

void send_ACK(){

	uint8_t buff[2] = {1, ACK};
	HAL_UART_Transmit(C_UART, buff, sizeof(buff), HAL_MAX_DELAY);
	
};

void send_NACK(){

	uint8_t buff[2] = {1, NACK};
	HAL_UART_Transmit(C_UART, buff, sizeof(buff), HAL_MAX_DELAY);
	
};

void Bootloader_JumToApplication(){

	void (*app_reset_handler)(void);


    // 1. configure the MSP by reading the value from the base address
    uint32_t msp_value = *(__IO uint32_t *) (OTA_SLOT_1_ADDRESS);

    //This function comes from CMSIS.
		
    __set_MSP(msp_value);


    /* 2. Now fetch the reset handler address of the user application
     * from the location FLASH_OTA_BASE_ADDRESS+4
     */
    uint32_t resethandler_address = *(__IO uint32_t *) (OTA_SLOT_1_ADDRESS + 4);

    app_reset_handler = (void*) resethandler_address;

    //3. jump to reset handler of the user application
    app_reset_handler();

};

void Bootloader_Deinit(){

	HAL_RCC_DeInit();
	HAL_DeInit();
	

};

void Bootloader_CheckSum(){
	
	crc =  HAL_CRC_Calculate ( & hcrc ,  ( uint32_t * )(OTA_SLOT_1_ADDRESS) ,  ( uint32_t ) (ota.ota_receive-4)/4);
	
	checkSum = *(uint32_t *) (OTA_SLOT_1_ADDRESS+ota.ota_receive-4);
	
	if(crc == checkSum){
	
		return;
		
	} else {
	
		return;
	
	}
	
}