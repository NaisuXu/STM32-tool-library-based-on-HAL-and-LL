#ifndef LIB_HAL_TLV5630_H_
#define LIB_HAL_TLV5630_H_

#include "main.h"

#ifdef HAL_SPI_MODULE_ENABLED

/*
 SPI working as master
 hardware nss disable
 frame format Motorola
 data size 16bit, MSB first
 cpol = Low, cpha = 2Edge
 SPI working on Polling mode
 TLV5630 working on µC Mode(pin-MODE High)
 */

/*
 Vout = 2REF * DATA / 4096
 TLV5630 DATA range is 0x000 to 0xFFF
 TLV5631 DATA range is 0x000 to 0xFFC
 TLV5632 DATA range is 0x000 to 0xFF0
 */

// Adjust the delay according to this
#define LIB_HAL_TLV5630_DELAY_COEFFICIENT 2

class Lib_HAL_Tlv5630 {
public:
	Lib_HAL_Tlv5630(SPI_HandleTypeDef *spi, GPIO_TypeDef *fsIOx, uint16_t fsPin, GPIO_TypeDef *preIOx, uint16_t prePin, GPIO_TypeDef *ldacIOx, uint16_t ldacPin, uint32_t holdrate = 2);
	~Lib_HAL_Tlv5630(void);
	bool init(uint16_t data = 0);
	bool write(uint8_t addr, uint16_t data);
	void load(void);
	bool setPowerDown(bool enable);
	bool setDout(bool enable);
	bool setReference(uint8_t ref);
	bool setInputMode(uint8_t mode);
	bool setPowerDownXY(uint8_t addr, bool enable);
	bool setSpeedXY(uint8_t addr, uint8_t speed);

private:
	SPI_HandleTypeDef *_spi;
	GPIO_TypeDef *_fsiox;
	uint16_t _fspin;
	GPIO_TypeDef *_preiox;
	uint16_t _prepin;
	GPIO_TypeDef *_ldaciox;
	uint16_t _ldacpin;
	uint8_t _pd;
	uint8_t _do;
	uint8_t _ref;
	uint8_t _im;
	uint8_t _pdab;
	uint8_t _pdcd;
	uint8_t _pdef;
	uint8_t _pdgh;
	uint8_t _sab;
	uint8_t _scd;
	uint8_t _sef;
	uint8_t _sgh;
	bool _write(uint16_t data);
	bool _preset(uint16_t data = 0);
	uint32_t _holdrate;
	void _hold(uint32_t times);
};

#endif /* HAL_SPI_MODULE_ENABLED */

#endif /* LIB_HAL_TLV5630_H_ */
