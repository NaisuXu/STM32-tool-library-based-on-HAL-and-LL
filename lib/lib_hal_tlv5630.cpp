#include "lib_hal_tlv5630.h"

#ifdef HAL_SPI_MODULE_ENABLED

#define LIB_HAL_TLV5630_CTRL0 8
#define LIB_HAL_TLV5630_CTRL1 9
#define LIB_HAL_TLV5630_Preset 10
#define LIB_HAL_TLV5630_Reserved 11

Lib_HAL_Tlv5630::Lib_HAL_Tlv5630(SPI_HandleTypeDef *spi, GPIO_TypeDef *fsIOx, uint16_t fsPin, GPIO_TypeDef *preIOx, uint16_t prePin, GPIO_TypeDef *ldacIOx, uint16_t ldacPin, uint32_t holdrate) :
		_spi(spi), _fsiox(fsIOx), _fspin(fsPin), _preiox(preIOx), _prepin(prePin), _ldaciox(ldacIOx), _ldacpin(ldacPin), _holdrate(holdrate) {
	_pd = 0;
	_do = 0;
	_ref = 0;
	_im = 0;
	_pdab = 0;
	_pdcd = 0;
	_pdef = 0;
	_pdgh = 0;
	_sab = 0;
	_scd = 0;
	_sef = 0;
	_sgh = 0;
}

Lib_HAL_Tlv5630::~Lib_HAL_Tlv5630(void) {
}

bool Lib_HAL_Tlv5630::init(uint16_t data) {
	if (!_preset(data)) {
		return false;
	}
	load();
	return true;
}

/*
 addr map:
 out_a = 0,
 out_b = 1,
 out_c = 2,
 out_d = 3,
 out_e = 4,
 out_f = 5,
 out_g = 6,
 out_h = 7,
 out_a&b = 12,
 out_c&d = 13,
 out_e&f = 14,
 out_g&h = 15
 */
bool Lib_HAL_Tlv5630::write(uint8_t addr, uint16_t data) {
	if (data > 4095) {
		return false;
	}
	if ((addr > 15) || ((addr < 12) && (addr > 7))) {
		return false;
	}
	uint16_t tmp = (addr << 12) | data;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

void Lib_HAL_Tlv5630::load(void) {
	HAL_GPIO_WritePin(_ldaciox, _ldacpin, GPIO_PIN_RESET);
	_hold(1);
	HAL_GPIO_WritePin(_ldaciox, _ldacpin, GPIO_PIN_SET);
	_hold(100);
}

bool Lib_HAL_Tlv5630::setPowerDown(bool enable) {
	_pd = enable ? 1 : 0;
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL0 << 12) | (_pd << 4) | (_do << 3) | (_ref << 1) | _im;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5630::setDout(bool enable) {
	_do = enable ? 1 : 0;
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL0 << 12) | (_pd << 4) | (_do << 3) | (_ref << 1) | _im;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

/*
 ref = 0or1 external
 ref = 2 internal 1024mV
 ref = 3 internal 2048mV
 */
bool Lib_HAL_Tlv5630::setReference(uint8_t ref) {
	if (ref > 3) {
		return false;
	}
	_ref = ref;
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL0 << 12) | (_pd << 4) | (_do << 3) | (_ref << 1) | _im;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

/*
 mode = 0 straight binary
 mode = 1 twos complement
 */
bool Lib_HAL_Tlv5630::setInputMode(uint8_t mode) {
	if (mode > 1) {
		return false;
	}
	_im = mode;
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL0 << 12) | (_pd << 4) | (_do << 3) | (_ref << 1) | _im;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

/*
 addr see function write() above
 */
bool Lib_HAL_Tlv5630::setPowerDownXY(uint8_t addr, bool enable) {
	if ((addr > 15) || (addr < 12)) {
		return false;
	}
	if (addr == 12) {
		_pdab = enable ? 1 : 0;
	}
	if (addr == 13) {
		_pdcd = enable ? 1 : 0;
	}
	if (addr == 14) {
		_pdef = enable ? 1 : 0;
	}
	if (addr == 15) {
		_pdgh = enable ? 1 : 0;
	}
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL1 << 12) | (_pdgh << 7) | (_pdef << 6) | (_pdcd << 5) | (_pdab << 4) | (_sgh << 3) | (_sef << 2) | (_scd << 1) | _sab;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

/*
 addr see function write() above
 speed = 0 slow
 speed = 1 fast
 */
bool Lib_HAL_Tlv5630::setSpeedXY(uint8_t addr, uint8_t speed) {
	if ((addr > 15) || (addr < 12)) {
		return false;
	}
	if (speed > 1) {
		return false;
	}
	if (addr == 12) {
		_sab = speed;
	}
	if (addr == 13) {
		_scd = speed;
	}
	if (addr == 14) {
		_sef = speed;
	}
	if (addr == 15) {
		_sgh = speed;
	}
	uint16_t tmp = (LIB_HAL_TLV5630_CTRL1 << 12) | (_pdgh << 7) | (_pdef << 6) | (_pdcd << 5) | (_pdab << 4) | (_sgh << 3) | (_sef << 2) | (_scd << 1) | _sab;
	if (!_write(tmp)) {
		return false;
	}
	return true;
}

bool Lib_HAL_Tlv5630::_write(uint16_t data) {
	HAL_GPIO_WritePin(_fsiox, _fspin, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit(_spi, (uint8_t*) (&data), 1, 1000) != HAL_OK) {
		HAL_GPIO_WritePin(_fsiox, _fspin, GPIO_PIN_SET);
		_hold(1);
		return false;
	}
	HAL_GPIO_WritePin(_fsiox, _fspin, GPIO_PIN_SET);
	_hold(1);
	return true;
}

bool Lib_HAL_Tlv5630::_preset(uint16_t data) {
	if (data > 4095) {
		return false;
	}
	uint16_t tmp = (LIB_HAL_TLV5630_Preset << 12) | data;
	if (!_write(tmp)) {
		return false;
	}
	HAL_GPIO_WritePin(_preiox, _prepin, GPIO_PIN_RESET);
	_hold(1);
	HAL_GPIO_WritePin(_preiox, _prepin, GPIO_PIN_SET);
	_hold(1);
	return true;
}

void Lib_HAL_Tlv5630::_hold(uint32_t times) {
	uint32_t i = times * _holdrate;
	while (i--)
		;
}

#endif /* HAL_SPI_MODULE_ENABLED */
