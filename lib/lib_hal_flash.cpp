#include "lib_hal_flash.h"

#ifdef HAL_FLASH_MODULE_ENABLED

Lib_HAL_Flash libhalflash;

Lib_HAL_Flash::Lib_HAL_Flash(void) {
}

Lib_HAL_Flash::~Lib_HAL_Flash(void) {
}

uint8_t Lib_HAL_Flash::read(size_t addr) {
	if (addr >= LIB_HAL_FLASH_USER_MAX_SIZE) {
		return 0xFF;
	}
	return *(__IO uint8_t*) (LIB_HAL_FLASH_ADDRESS + addr);
}

bool Lib_HAL_Flash::read(uint8_t *data, size_t startaddr, size_t size) {
	if ((startaddr + size) > LIB_HAL_FLASH_USER_MAX_SIZE) {
		return false;
	}
	startaddr = LIB_HAL_FLASH_ADDRESS + startaddr;
	for (size_t i = 0; i < size; i++) {
		data[i] = *(__IO uint8_t*) (startaddr + i);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Lib_HAL_Flash::write(uint8_t data, size_t addr) {

	return write(&data, addr, 1);
}

bool Lib_HAL_Flash::write(uint8_t *data, size_t startaddr, size_t size) {
	if ((startaddr + size) > LIB_HAL_FLASH_USER_MAX_SIZE) {
		return false;
	}
	size_t doublewordsize = LIB_HAL_FLASH_USER_MAX_SIZE / 8;
	uint64_t buf[doublewordsize];
	for (size_t i = 0; i < doublewordsize; i++) {
		buf[i] = ((uint64_t)read(i * 8) << 56) | ((uint64_t)read(i * 8 + 1) << 48) | ((uint64_t)read(i * 8 + 2) << 40) | ((uint64_t)read(i * 8 + 3) << 32);
		buf[i] |= ((uint64_t)read(i * 8 + 4) << 24) | ((uint64_t)read(i * 8 + 5) << 16) | ((uint64_t)read(i * 8 + 6) << 8) | (uint64_t)read(i * 8 + 7);
	}
	for (size_t i = 0; i < size; i++) {
		size_t index = (startaddr + i) / 8;
		size_t rem = (startaddr + i) % 8;
		buf[index] &= ~(0x00000000000000FF << ((uint64_t)rem * 8));
		buf[index] |= (uint64_t)data[i] << ((uint64_t)rem * 8);
	}
	erase();
	HAL_FLASH_Unlock();
	for (size_t i = 0; i < doublewordsize; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, i * 8 + LIB_HAL_FLASH_ADDRESS, buf[i]);
	}
	HAL_FLASH_Lock();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void Lib_HAL_Flash::erase(void) {
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef flash;

#if defined(STM32G0xx_HAL_FLASH_EX_H)
	flash.TypeErase = LIB_HAL_FLASH_TYPEERASE;
	flash.Banks = LIB_HAL_FLASH_BANKS;
	flash.Page = LIB_HAL_FLASH_PAGE;
	flash.NbPages = 1;

#elif defined(STM32F030x6) || defined(STM32F042x6) || defined(STM32F070x6)
	flash.TypeErase = LIB_HAL_FLASH_TYPEERASE;
	flash.PageAddress = LIB_HAL_FLASH_ADDRESS;
	flash.NbPages = 1;

#elif defined(STM32F100xB) || defined(STM32F101xB) || defined(STM32F102xB) || defined(STM32F103xB)
	flash.TypeErase = LIB_HAL_FLASH_TYPEERASE;
	flash.Banks = LIB_HAL_FLASH_BANKS;
	flash.PageAddress = LIB_HAL_FLASH_ADDRESS;
	flash.NbPages = 1;

#elif defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
	defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F410Tx) || defined(STM32F410Cx) ||\
	defined(STM32F410Rx) || defined(STM32F411xE) || defined(STM32F446xx) || defined(STM32F412Zx) ||\
	defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) ||\
	defined(STM32F423xx)
	flash.TypeErase = LIB_HAL_FLASH_TYPEERASE;
	flash.Banks = LIB_HAL_FLASH_BANKS;
	flash.Sector = LIB_HAL_FLASH_SECTOR;
	flash.NbSectors = 1;
	flash.VoltageRange = LIB_HAL_FLASH_VOLTAGERANGE;

#endif

	uint32_t PageError = 0;
	HAL_FLASHEx_Erase(&flash, &PageError);
	HAL_FLASH_Lock();
}

#endif /* HAL_FLASH_MODULE_ENABLED */
