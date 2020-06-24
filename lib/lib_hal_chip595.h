#ifndef LIB_HAL_CHIP595_H_
#define LIB_HAL_CHIP595_H_

#include "main.h"

#ifdef HAL_GPIO_MODULE_ENABLED

class Lib_HAL_Chip595 {
public:
	Lib_HAL_Chip595(GPIO_TypeDef *gpioSH, uint16_t pinSH, GPIO_TypeDef *gpioDS, uint16_t pinDS, GPIO_TypeDef *gpioST, uint16_t pinST, GPIO_TypeDef *gpioOE = nullptr, uint16_t pinOE = -1, uint32_t holdrate = 6);
	~Lib_HAL_Chip595(void);
	void enable(void);
	void disable(void);
	void write(uint8_t *data, int size);

private:
	GPIO_TypeDef *_gpioSH;
	uint16_t _pinSH;
	GPIO_TypeDef *_gpioDS;
	uint16_t _pinDS;
	GPIO_TypeDef *_gpioST;
	uint16_t _pinST;
	GPIO_TypeDef *_gpioOE;
	uint16_t _pinOE;
	uint32_t _holdrate;
	void _hold(uint32_t times);
};

#endif /* HAL_GPIO_MODULE_ENABLED */

#endif /* LIB_HAL_CHIP595_H_ */
