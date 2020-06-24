#ifndef LIB_HAL_UART_H_
#define LIB_HAL_UART_H_

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#define LIB_HAL_UARTTX_MAX_SIZE 8

typedef struct {
	uint8_t *data;
	uint16_t size;
} Lib_HAL_UartTxInfo;

class Lib_HAL_UartTx {
public:
	Lib_HAL_UartTx(UART_HandleTypeDef *uart);
	~Lib_HAL_UartTx(void);
	bool write(uint8_t *data, uint16_t size);
	void dmaTcHandle(UART_HandleTypeDef *uart);

private:
	UART_HandleTypeDef *_uart;
	Lib_HAL_UartTxInfo _info[LIB_HAL_UARTTX_MAX_SIZE + 1];
	size_t _queuesize;
	size_t _queuefront;
	size_t _queuerear;
	bool _sending;
};

////////////////////////////////////////////////////////////////////////////////

class Lib_HAL_UartRx {
public:
	Lib_HAL_UartRx(UART_HandleTypeDef *uart, DMA_HandleTypeDef *dma, uint8_t *buf, size_t bufsize, void (*dataParse)(uint16_t rxsize));
	~Lib_HAL_UartRx(void);
	void listen(void);
	void reset(void);
	void uartIdleHandle(void);

private:
	UART_HandleTypeDef *_uart;
	DMA_HandleTypeDef *_dma;
	uint8_t *_buf;
	size_t _bufsize;
	bool _reset;
	void (*_dataParse)(uint16_t rxsize);
};

#endif /* HAL_UART_MODULE_ENABLED */

#endif /* LIB_HAL_UART_H_ */
