#include "lib_hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

Lib_HAL_UartTx::Lib_HAL_UartTx(UART_HandleTypeDef *uart) :
		_uart(uart), _queuesize(LIB_HAL_UARTTX_MAX_SIZE + 1), _queuefront(0), _queuerear(0), _sending(false) {
}

Lib_HAL_UartTx::~Lib_HAL_UartTx(void) {
}

bool Lib_HAL_UartTx::write(uint8_t *data, uint16_t size) {
	if ((_queuerear + 1) % _queuesize == _queuefront) {
		return false;
	}
	_info[_queuerear].data = data;
	_info[_queuerear].size = size;
	_queuerear = (_queuerear + 1) % _queuesize;
	if (!_sending) {
		_sending = true;
		HAL_UART_Transmit_DMA(_uart, _info[_queuefront].data, _info[_queuefront].size);
		_queuefront = (_queuefront + 1) % _queuesize;
	}
	return true;
}

void Lib_HAL_UartTx::dmaTcHandle(UART_HandleTypeDef *uart) {
	if (uart != _uart) {
		return;
	}
	if (_queuerear == _queuefront) {
		_sending = false;
		return;
	}
	HAL_UART_Transmit_DMA(_uart, _info[_queuefront].data, _info[_queuefront].size);
	_queuefront = (_queuefront + 1) % _queuesize;
}

////////////////////////////////////////////////////////////////////////////////

Lib_HAL_UartRx::Lib_HAL_UartRx(UART_HandleTypeDef *uart, DMA_HandleTypeDef *dma, uint8_t *buf, size_t bufsize, void (*dataParse)(uint16_t rxsize)) :
		_uart(uart), _dma(dma), _buf(buf), _bufsize(bufsize), _reset(false), _dataParse(dataParse) {
}

Lib_HAL_UartRx::~Lib_HAL_UartRx(void) {
}

void Lib_HAL_UartRx::listen(void) {
	__HAL_UART_CLEAR_IDLEFLAG(_uart);
	__HAL_UART_ENABLE_IT(_uart, UART_IT_IDLE); // 使能空闲接收中断
	HAL_UART_AbortReceive_IT(_uart); // 停止当前数据接收
	if (HAL_UART_Receive_DMA(_uart, _buf, _bufsize) != HAL_OK) { // 开启UART DMA数据接收
		_reset = true;
	}
}

void Lib_HAL_UartRx::reset(void) {
	if (_reset) { // 这么做理论上存在一些问题，但HAL库的一些问题导致目前这么做还算不错吧
		_reset = false;
		HAL_UART_AbortReceive_IT(_uart); // 停止当前数据接收
		// HAL_UART_Receive_DMA这个函数，在F0上每次都正常，F4上时不时就返回HAL_BUSY
		if (HAL_UART_Receive_DMA(_uart, _buf, _bufsize) != HAL_OK) {
			_reset = true;
		}
	}
}

void Lib_HAL_UartRx::uartIdleHandle(void) {
	if (__HAL_UART_GET_FLAG(_uart, UART_FLAG_IDLE)) { // 如果是空闲中断
		__HAL_UART_CLEAR_IDLEFLAG(_uart); // 清除空闲中断标志
		_dataParse(_bufsize - __HAL_DMA_GET_COUNTER(_dma)); // 解析接收缓存中的数据
		HAL_UART_AbortReceive_IT(_uart); // 停止当前数据接收
		if (HAL_UART_Receive_DMA(_uart, _buf, _bufsize) != HAL_OK) { // 重新开启UART DMA数据接收
			_reset = true;
		}
	}
}

#endif /* HAL_UART_MODULE_ENABLED */
