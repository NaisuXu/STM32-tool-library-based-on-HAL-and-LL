#include "lib_ll_uart.h"

#if defined(__STM32F4xx_LL_USART_H) && defined(__STM32F4xx_LL_DMA_H) || \
	(defined(__STM32F1xx_LL_USART_H) && defined(__STM32F1xx_LL_DMA_H))

Lib_LL_UartTx::Lib_LL_UartTx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel) :
		_uart(uart), _dmax(dmax), _stream_channel(stream_channel), _queuesize(LIB_LL_UARTTX_MAX_SIZE + 1), _queuefront(0), _queuerear(0), _sending(false), _begin(false) {
}

Lib_LL_UartTx::~Lib_LL_UartTx(void) {
}

bool Lib_LL_UartTx::write(uint8_t *data, uint16_t size) {
	if ((_queuerear + 1) % _queuesize == _queuefront) {
		return false;
	}
	_info[_queuerear].data = data;
	_info[_queuerear].size = size;
	_queuerear = (_queuerear + 1) % _queuesize;
	if (!_sending) {
		_sending = true;
		if (!_begin) {
			_begin = true;
			LL_DMA_SetPeriphAddress(_dmax, _stream_channel, LL_USART_DMA_GetRegAddr(_uart));
			LL_DMA_EnableIT_TC(_dmax, _stream_channel);
			LL_USART_EnableDMAReq_TX(_uart);
		}
		LL_DMA_SetMemoryAddress(_dmax, _stream_channel, (uint32_t) _info[_queuefront].data);
		LL_DMA_SetDataLength(_dmax, _stream_channel, _info[_queuefront].size);
		_queuefront = (_queuefront + 1) % _queuesize;
		LL_DMA_EnableStreamChannel(_dmax, _stream_channel);
	}
	return true;
}

void Lib_LL_UartTx::dmaTcHandle(uint32_t (*isactiveflag)(DMA_TypeDef *DMAx), void (*clearflag)(DMA_TypeDef *DMAx)) {
	if (isactiveflag(_dmax)) {
		clearflag(_dmax);
		LL_DMA_DisableStreamChannel(_dmax, _stream_channel);
		if (_queuerear == _queuefront) {
			_sending = false;
			return;
		}
		LL_DMA_SetMemoryAddress(_dmax, _stream_channel, (uint32_t) _info[_queuefront].data);
		LL_DMA_SetDataLength(_dmax, _stream_channel, _info[_queuefront].size);
		_queuefront = (_queuefront + 1) % _queuesize;
		LL_DMA_EnableStreamChannel(_dmax, _stream_channel);
	}
}

////////////////////////////////////////////////////////////////////////////////

Lib_LL_UartRx::Lib_LL_UartRx(USART_TypeDef *uart, DMA_TypeDef *dmax, uint32_t stream_channel, uint8_t *buf, uint32_t bufsize, void (*dataParse)(uint16_t rxrear)) :
		_uart(uart), _dmax(dmax), _stream_channel(stream_channel), _buf(buf), _bufsize(bufsize), _dataParse(dataParse) {
}

Lib_LL_UartRx::~Lib_LL_UartRx(void) {
}

void Lib_LL_UartRx::listen(void) {
	LL_DMA_SetPeriphAddress(_dmax, _stream_channel, LL_USART_DMA_GetRegAddr(_uart));
	LL_DMA_SetMemoryAddress(_dmax, _stream_channel, (uint32_t) _buf);
	LL_DMA_SetDataLength(_dmax, _stream_channel, _bufsize);
	LL_USART_EnableDMAReq_RX(_uart);
	LL_DMA_EnableStreamChannel(_dmax, _stream_channel);
	LL_USART_ClearFlag_IDLE(_uart);
	LL_USART_EnableIT_IDLE(_uart);
}

void Lib_LL_UartRx::uartIdleHandle(void) {
	if (LL_USART_IsActiveFlag_IDLE(_uart)) {
		LL_USART_ClearFlag_IDLE(_uart);
		_dataParse(_bufsize - LL_DMA_GetDataLength(_dmax, _stream_channel));
	}
}

#endif
