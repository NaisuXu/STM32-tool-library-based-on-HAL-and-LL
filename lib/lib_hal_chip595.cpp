#include "lib_hal_chip595.h"

#ifdef HAL_GPIO_MODULE_ENABLED

Lib_HAL_Chip595::Lib_HAL_Chip595(GPIO_TypeDef *gpioSH, uint16_t pinSH, GPIO_TypeDef *gpioDS, uint16_t pinDS, GPIO_TypeDef *gpioST, uint16_t pinST, GPIO_TypeDef *gpioOE, uint16_t pinOE, uint32_t holdrate) :
		_gpioSH(gpioSH), _pinSH(pinSH), _gpioDS(gpioDS), _pinDS(pinDS), _gpioST(gpioST), _pinST(pinST), _gpioOE(gpioOE), _pinOE(pinOE), _holdrate(holdrate) {
}

Lib_HAL_Chip595::~Lib_HAL_Chip595(void) {
}

void Lib_HAL_Chip595::enable(void) {
	if (_gpioOE != nullptr) {
		HAL_GPIO_WritePin(_gpioOE, _pinOE, GPIO_PIN_RESET);
		_hold(2);
	}
}

void Lib_HAL_Chip595::disable(void) {
	if (_gpioOE != nullptr) {
		HAL_GPIO_WritePin(_gpioOE, _pinOE, GPIO_PIN_SET);
		_hold(2);
	}
}

void Lib_HAL_Chip595::write(uint8_t *data, int size) {
	for (int i = 0; i < size; i++) {
		uint8_t tmp = *(data + i);
		for (uint8_t j = 0; j < 8; j++) {
			if (tmp & 0x80) //按位从MSB开始写入数据
					{
				HAL_GPIO_WritePin(_gpioDS, _pinDS, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(_gpioDS, _pinDS, GPIO_PIN_RESET);
			}
			tmp <<= 1;
			HAL_GPIO_WritePin(_gpioSH, _pinSH, GPIO_PIN_RESET);
			_hold(1);
			HAL_GPIO_WritePin(_gpioSH, _pinSH, GPIO_PIN_SET); //上升沿
			_hold(1);
		}
	}
	HAL_GPIO_WritePin(_gpioST, _pinST, GPIO_PIN_RESET);
	_hold(1);
	HAL_GPIO_WritePin(_gpioST, _pinST, GPIO_PIN_SET); //上升沿
	_hold(1);
}

void Lib_HAL_Chip595::_hold(uint32_t times) {
	uint32_t i = times * _holdrate;
	while (i--)
		;
}

#endif /* HAL_GPIO_MODULE_ENABLED */
