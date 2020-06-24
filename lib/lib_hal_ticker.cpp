#include "lib_hal_ticker.h"

#ifdef HAL_MODULE_ENABLED

typedef void (*Lib_HAL_TickerCallBack)(uint32_t arg);

Lib_HAL_Ticker::Lib_HAL_Ticker(void) :
		_active(false), _ready(false), _period(0), _count(0), _callback(nullptr), _arg(0) {
}

Lib_HAL_Ticker::~Lib_HAL_Ticker(void) {
	detach();
}

void Lib_HAL_Ticker::detach(void) {
	if (_active) {
		__disable_irq();
		_active = false;
		_ready = false;
		_period = 0;
		_callback = nullptr;
		_arg = 0;
		_ticker[_index] = nullptr;
		__enable_irq();
	}
}

bool Lib_HAL_Ticker::active(void) {
	return _active;
}

bool Lib_HAL_Ticker::_attach(uint32_t period, uint32_t count, callback_with_arg_t callback, uint32_t arg) {
	if (callback == nullptr) {
		return false;
	}
	if (_active) {
		_ready = false;
		_period = period;
		_count = count;
		_callback = callback;
		_arg = arg;
		return true;
	} else {
		for (uint32_t i = 0; i < LIB_HAL_TICKER_MAX_SIZE; i++) {
			if (_ticker[i] == nullptr) {
				__disable_irq();
				_active = true;
				_index = i;
				_ready = false;
				_period = period;
				_count = count;
				_callback = callback;
				_arg = arg;
				_ticker[i] = this;
				if (_count == 0) {
					_ready = true;
				}
				__enable_irq();
				return true;
			}
		}
	}
	return false;
}

void Lib_HAL_Ticker::handle(void) {
	for (uint32_t i = 0; i < LIB_HAL_TICKER_MAX_SIZE; i++) {
		if (_ticker[i] == nullptr) {
			continue;
		}
		if (!_ticker[i]->_active) {
			continue;
		}
		if (_ticker[i]->_ready) {
			_ticker[i]->_ready = false;
			_ticker[i]->_callback(reinterpret_cast<void*>(_ticker[i]->_arg));
			if ((_ticker[i]->_period == 0) && (_ticker[i]->_count == 0)) {
				_ticker[i]->detach();
			}
		}
	}
}

void Lib_HAL_Ticker::schedule(void) {
	for (uint32_t i = 0; i < LIB_HAL_TICKER_MAX_SIZE; i++) {
		if (_ticker[i] == nullptr) {
			continue;
		}
		if (!_ticker[i]->_active) {
			continue;
		}
		if (_ticker[i]->_count) {
			_ticker[i]->_count--;
		}
		if (_ticker[i]->_count == 0) {
			_ticker[i]->_ready = true;
			_ticker[i]->_count = _ticker[i]->_period;
		}
	}
}

Lib_HAL_Ticker *Lib_HAL_Ticker::_ticker[LIB_HAL_TICKER_MAX_SIZE] = { nullptr };

extern __IO uint32_t uwTick;
extern HAL_TickFreqTypeDef uwTickFreq;
void HAL_IncTick(void) {
	uwTick += uwTickFreq;
	Lib_HAL_Ticker::schedule();
}

#endif /* HAL_MODULE_ENABLED */
