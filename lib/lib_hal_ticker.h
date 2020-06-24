#ifndef LIB_HAL_TICKER_H_
#define LIB_HAL_TICKER_H_

#include "main.h"

#ifdef HAL_MODULE_ENABLED

#define LIB_HAL_TICKER_MAX_SIZE 16

class Lib_HAL_Ticker {
public:
	Lib_HAL_Ticker(void);
	~Lib_HAL_Ticker(void);

	typedef void (*callback_t)(void);
	typedef void (*callback_with_arg_t)(void*);

	bool attach(uint32_t milliseconds, callback_t callback) {
		if (!milliseconds) {
			return false;
		}
		return _attach(milliseconds, milliseconds, reinterpret_cast<callback_with_arg_t>(callback), 0);
	}

	template<typename T>
	bool attach(uint32_t milliseconds, void (*callback)(T), T arg) {
		if (!milliseconds) {
			return false;
		}
		if (sizeof(T) > sizeof(uint32_t)) {
			return false;
		}
		uint32_t arg32 = (uint32_t) arg;
		return _attach(milliseconds, milliseconds, reinterpret_cast<callback_with_arg_t>(callback), arg32);
	}

	bool once(uint32_t milliseconds, callback_t callback) {
		return _attach(0, milliseconds, reinterpret_cast<callback_with_arg_t>(callback), 0);
	}

	template<typename T>
	bool once(uint32_t milliseconds, void (*callback)(T), T arg) {
		if (sizeof(T) > sizeof(uint32_t)) {
			return false;
		}
		uint32_t arg32 = (uint32_t) arg;
		return _attach(0, milliseconds, reinterpret_cast<callback_with_arg_t>(callback), arg32);
	}
	void detach(void);
	bool active(void);
	static void handle(void);
	static void schedule(void);

private:
	bool _active;
	uint32_t _index;
	bool _ready;
	uint32_t _period;
	uint32_t _count;
	callback_with_arg_t _callback;
	uint32_t _arg;
	bool _attach(uint32_t period, uint32_t count, callback_with_arg_t callback, uint32_t arg);
	static Lib_HAL_Ticker *_ticker[LIB_HAL_TICKER_MAX_SIZE];
};

#endif /* HAL_MODULE_ENABLED */

#endif /* LIB_HAL_TICKER_H_ */
