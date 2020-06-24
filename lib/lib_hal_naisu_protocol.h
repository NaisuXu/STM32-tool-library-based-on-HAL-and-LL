#ifndef LIB_HAL_NAISU_PROTOCOL_H_
#define LIB_HAL_NAISU_PROTOCOL_H_

#include "main.h"

/*
 format:
 STX CMD LEN-H(data) LEN-L(data) DATA(if have) BCC
 */

#define LIB_HAL_NAISU_PROTOCOL_MAX_SIZE 8

typedef struct {
	void (*callback)(uint16_t msgsize);
	uint8_t stx;
	uint8_t cmd;
} Lib_HAL_NaisuProtocolInfo;

class Lib_HAL_NaisuProtocol {
public:
	Lib_HAL_NaisuProtocol(uint8_t *buf, uint16_t bufsize);
	~Lib_HAL_NaisuProtocol(void);
	bool add(void (*callback)(uint16_t msgsize), uint8_t stx, uint8_t cmd);
	void parse(uint16_t rxsize);
	uint8_t read(uint16_t offset);
	bool read(uint8_t *dest, uint16_t offset, uint16_t size);
	uint8_t calBcc(uint8_t *data, uint16_t size);

private:
	Lib_HAL_NaisuProtocolInfo _info[LIB_HAL_NAISU_PROTOCOL_MAX_SIZE];
	uint16_t _attachsize;
	uint8_t *_buf;
	uint16_t _bufsize;
	uint16_t _front;
	uint16_t _msgsize;
};

#endif /* LIB_HAL_NAISU_PROTOCOL_H_ */
