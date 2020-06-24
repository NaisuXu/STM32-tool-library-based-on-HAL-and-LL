#ifndef LIB_HAL_TLV5638_H_
#define LIB_HAL_TLV5638_H_

#include "main.h"

#ifdef HAL_SPI_MODULE_ENABLED

/*
 SPI working as master
 hardware nss disable
 frame format Motorola
 data size 16bit, MSB first
 cpol = High,  cpha = 1Edge
 SPI working on Polling mode
 */

/*
 Vout = 2REF * DATA / 4096
 DATA range is 0x000 to 0xFFF
 */

class Lib_HAL_Tlv5638 {
public:
	Lib_HAL_Tlv5638(SPI_HandleTypeDef *spi, GPIO_TypeDef *csIOx, uint16_t csPin, uint32_t holdrate = 2);
	~Lib_HAL_Tlv5638(void);
	bool setSpeed(uint8_t speed);
	bool setPowerDown(bool enable);
	bool setReference(uint8_t ref);
	bool setDAC(uint16_t dataA, uint16_t dataB);
	bool setDACB(uint16_t data);
	bool setDACA(uint16_t data);

private:
	SPI_HandleTypeDef *_spi;
	GPIO_TypeDef *_csiox;
	uint16_t _cspin;
	uint8_t _speed;
	uint8_t _power;
	uint8_t _ref;
	bool _write(uint16_t data);
	uint32_t _holdrate;
	void _hold(uint32_t times);
};

#endif /* HAL_SPI_MODULE_ENABLED */

#endif /* LIB_HAL_TLV5638_H_ */
