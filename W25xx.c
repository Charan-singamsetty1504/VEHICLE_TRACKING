/*
 * W25Qxx.c
 *
 *  Created on: Feb 7, 2024
 *      Author: Lenovo
 */


#include "main.h"
#include "W25Qxx.h"

extern SPI_HandleTypeDef hspi1;
#define HSPI1 hspi1

uint32_t gAddr;

void ReadId(uint8_t *read, uint8_t datasize)
{
	static uint8_t cmd = 0x9F;
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&HSPI1, &cmd, 1, 1000);
	  HAL_SPI_Receive(&HSPI1, read, 3, 3000);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void WriteEnable(void)
{
    static uint8_t cmd = 0x06;
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&HSPI1, &cmd, 1, HAL_MAX_DELAY);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
uint32_t GetCurrentAddress(uint32_t addr, uint8_t *data, uint8_t datasize)
{
	uint32_t ans = PAGE_SIZE-datasize;
	addr += ans;
	return addr;
}
void WritePage(uint32_t address, uint8_t *data, uint8_t datasize)
{
		static uint8_t cmd[4];
		    cmd[0] = 0x02;
		    cmd[1] = (uint8_t)(address >> 16);
		    cmd[2] = (uint8_t)(address >> 8);
		    cmd[3] = (uint8_t)address;
	        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	   	    HAL_SPI_Transmit(&HSPI1, cmd, sizeof(cmd), HAL_MAX_DELAY);
	   	    HAL_SPI_Transmit(&HSPI1, (uint8_t *)data, datasize, HAL_MAX_DELAY);
	   	   gAddr+=datasize;
	   		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void WriteDataToFlash(uint32_t startAddress, uint8_t *data, uint32_t dataSize) {
    uint32_t offset = 0;

    while (offset < dataSize) {
        // Calculate remaining space in the current page
        uint32_t remainingBytes = PAGE_SIZE - (offset % PAGE_SIZE);

        // Determine how many bytes to write in the current page
        uint32_t bytesToWrite = (dataSize - offset <= remainingBytes) ? dataSize - offset : remainingBytes;

        // Write data to the current page starting from the calculated offset
        WritePage(startAddress + offset, &data[offset], bytesToWrite);

        // Update the offset with the number of bytes written
        offset += bytesToWrite;

        // If there is more data to write, move to the next page
        if (offset < dataSize) {
            startAddress += PAGE_SIZE;
        }
    }
}
void ReadPage(uint32_t address, uint8_t *data, uint8_t datasize)
{
	    static uint8_t cmd[4];
	    cmd[0] = 0x03;
	    cmd[1] = (uint8_t)(address >> 16);
	    cmd[2] = (uint8_t)(address >> 8);
	    cmd[3] = (uint8_t)address;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	    HAL_SPI_Transmit(&HSPI1, cmd, sizeof(cmd), HAL_MAX_DELAY);
	    HAL_SPI_Receive(&HSPI1, (uint8_t *)data, datasize, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void EraseSector(uint32_t address, uint8_t *data, uint8_t datasize)
{
	static uint8_t cmd[4];
			    cmd[0] = 0x06;
			    cmd[1] = (uint8_t)(address >> 16);
			    cmd[2] = (uint8_t)(address >> 8);
			    cmd[3] = (uint8_t)address;
		        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		   	    HAL_SPI_Transmit(&HSPI1, cmd, sizeof(cmd), HAL_MAX_DELAY);
		   	    HAL_SPI_Transmit(&HSPI1, (uint8_t *)data, datasize, HAL_MAX_DELAY);
}

