#include "lib_hal_tlv5638.h"

#ifdef HAL_SPI_MODULE_ENABLED

#define LIB_HAL_TLV5638_wBandBuf 0
#define LIB_HAL_TLV5638_wBuf     1
#define LIB_HAL_TLV5638_wAandupB 8
#define LIB_HAL_TLV5638_wCtrReg  9

Lib_HAL_Tlv5638::Lib_HAL_Tlv5638(SPI_HandleTypeDef *spi, GPIO_TypeDef *csIOx, uint16_t csPin, uint32_t holdrate) :
		_spi(spi), _csiox(csIOx), _cspin(csPin), _holdrate(holdrate) {
	_speed = 0;
	_power = 0;
	_ref = 0;
}

Lib_HAL_Tlv5638::~Lib_HAL_Tlv5638(void) {
}

/*
 speed = 0 slow mode
 speed = 1 fast mode
 */
bool Lib_HAL_Tlv5638::setSpeed(uint8_t speed) {
	if (speed > 2) {
		return false;
	}
	_speed = speed;
	uint16_t tmp = (LIB_HAL_TLV5638_wCtrReg << 12) | (_speed << 14) | (_power << 13) | _ref;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5638::setPowerDown(bool enable) {
	_power = enable ? 1 : 0;
	uint16_t tmp = (LIB_HAL_TLV5638_wCtrReg << 12) | (_speed << 14) | (_power << 13) | _ref;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

/*
 ref = 0or3 external
 ref = 1 internal 1024mV
 ref = 2 internal 2048mV
 */
bool Lib_HAL_Tlv5638::setReference(uint8_t ref) {
	if (ref > 3) {
		return false;
	}
	_ref = ref;
	uint16_t tmp = (LIB_HAL_TLV5638_wCtrReg << 12) | (_speed << 14) | (_power << 13) | _ref;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5638::setDAC(uint16_t dataA, uint16_t dataB) {
	if ((dataA > 4095) || (dataB > 4095)) {
		return false;
	}
	uint16_t tmp = (LIB_HAL_TLV5638_wBuf << 12) | (_speed << 14) | (_power << 13) | dataB;
	if (!_write(tmp)) {
		return false;
	}
	tmp = (LIB_HAL_TLV5638_wAandupB << 12) | (_speed << 14) | (_power << 13) | dataA;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5638::setDACB(uint16_t data) {
	if (data > 4095) {
		return false;
	}
	uint16_t tmp = (LIB_HAL_TLV5638_wBandBuf << 12) | (_speed << 14) | (_power << 13) | data;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5638::setDACA(uint16_t data) {
	if (data > 4095) {
		return false;
	}
	uint16_t tmp = (LIB_HAL_TLV5638_wAandupB << 12) | (_speed << 14) | (_power << 13) | data;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5638::_write(uint16_t data) {
	HAL_GPIO_WritePin(_csiox, _cspin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(_spi, (uint8_t*) (&data), 1, 200) != HAL_OK) {
		HAL_GPIO_WritePin(_csiox, _cspin, GPIO_PIN_SET);
		_hold(1);
		return false;
	}
	HAL_GPIO_WritePin(_csiox, _cspin, GPIO_PIN_SET);
	_hold(1);
	return true;
}

void Lib_HAL_Tlv5638::_hold(uint32_t times) {
	uint32_t i = times * _holdrate;
	while (i--)
		;
}

#endif /* HAL_SPI_MODULE_ENABLED */
