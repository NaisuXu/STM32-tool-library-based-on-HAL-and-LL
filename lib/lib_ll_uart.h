#ifndef LIB_LL_UART_H_
#define LIB_LL_UART_H_

#include "main.h"

#if defined(__STM32F4xx_LL_USART_H) && defined(__STM32F4xx_LL_DMA_H)
#define LL_DMA_EnableStreamChannel LL_DMA_EnableStream
#define LL_DMA_DisableStreamChannel LL_DMA_DisableStream
#elif defined(__STM32F1xx_LL_USART_H) && defined(__STM32F1xx_LL_DMA_H)
#define LL_DMA_EnableStreamChannel LL_DMA_EnableChannel
#define LL_DMA_DisableStreamChannel LL_DMA_DisableChannel
#endif

////////////////////////////////////////////////////////////////////////////////

#if (defined(__STM32F4xx_LL_USART_H) && defined(__STM32F4xx_LL_DMA_H)) || \
	(defined(__STM32F1xx_LL_USART_H) && defined(__STM32F1xx_LL_DMA_H))

#define LIB_LL_UARTTX_MAX_SIZE 8

typedef struct {
	uint8_t *data;
	uint16_t size;
} Lib_LL_UartTxInfo;

class Lib_LL_UartTx {
public:
	Lib_LL_UartTx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel);
	~Lib_LL_UartTx(void);
	bool write(uint8_t *data, uint16_t size);
	void dmaTcHandle(uint32_t (*isactiveflag)(DMA_TypeDef *DMAx), void (*clearflag)(DMA_TypeDef *DMAx));

private:
	USART_TypeDef *_uart;
	DMA_TypeDef *_dmax;
	uint32_t _stream_channel;
	Lib_LL_UartTxInfo _info[LIB_LL_UARTTX_MAX_SIZE + 1];
	uint32_t _queuesize;
	uint32_t _queuefront;
	uint32_t _queuerear;
	bool _sending;
	bool _begin;
};

////////////////////////////////////////////////////////////////////////////////

class Lib_LL_UartRx {
public:
	Lib_LL_UartRx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel, uint8_t *buf, uint32_t bufsize, void (*dataParse)(uint16_t rxrear));
	~Lib_LL_UartRx(void);
	void listen(void);
	void uartIdleHandle(void);

private:
	USART_TypeDef *_uart;
	DMA_TypeDef *_dmax;
	uint32_t _stream_channel;
	uint8_t *_buf;
	uint32_t _bufsize;
	void (*_dataParse)(uint16_t rxrear);
};

#endif

#endif /* LIB_LL_UART_H_ */
