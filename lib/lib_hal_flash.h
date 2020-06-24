#ifndef LIB_HAL_FLASH_H_
#define LIB_HAL_FLASH_H_

#include "main.h"

#ifdef HAL_FLASH_MODULE_ENABLED

//this must less then or equal to page or sector size
//and must be even
#define LIB_HAL_FLASH_USER_MAX_SIZE 100

#if defined(STM32F030x6) || defined(STM32F042x6) || defined(STM32F070x6)
#define LIB_HAL_FLASH_TYPEERASE (FLASH_TYPEERASE_PAGES) //see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_ADDRESS (FLASH_BANK1_END - FLASH_PAGE_SIZE + 1)

#elif defined(STM32F100xB) || defined(STM32F101xB) || defined(STM32F102xB) || defined(STM32F103xB)
#define LIB_HAL_FLASH_TYPEERASE (FLASH_TYPEERASE_PAGES) //see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_BANKS (FLASH_BANK_1)				//see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_ADDRESS (FLASH_BANK1_END - FLASH_PAGE_SIZE + 1)

#elif defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
      defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F410Tx) || defined(STM32F410Cx) ||\
      defined(STM32F410Rx) || defined(STM32F411xE) || defined(STM32F446xx) || defined(STM32F412Zx) ||\
      defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) ||\
      defined(STM32F423xx)
#define LIB_HAL_FLASH_TYPEERASE (FLASH_TYPEERASE_SECTORS) //see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_BANKS (FLASH_BANK_1)				//see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_SECTOR (FLASH_SECTOR_11)				//see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_VOLTAGERANGE (FLASH_VOLTAGE_RANGE_3)				//see "stm32fxxx_hal_flash_ex.h"
#define LIB_HAL_FLASH_ADDRESS (0x080E0000) //see document<RM0090>

#endif

class Lib_HAL_Flash {
public:
	Lib_HAL_Flash(void);
	~Lib_HAL_Flash(void);
	uint8_t read(size_t addr);
	bool read(uint8_t *data, size_t startaddr, size_t size);
	bool write(uint8_t data, size_t addr);
	bool write(uint8_t *data, size_t startaddr, size_t size);
	void erase(void);
};

extern Lib_HAL_Flash libhalflash;

#endif /* HAL_FLASH_MODULE_ENABLED */

#endif /* LIB_HAL_FLASH_H_ */
