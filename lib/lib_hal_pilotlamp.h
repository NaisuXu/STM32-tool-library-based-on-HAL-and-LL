#ifndef LIB_HAL_PILOTLAMP_H_
#define LIB_HAL_PILOTLAMP_H_

#include "main.h"
#include "lib_hal_ticker.h"

#ifdef HAL_GPIO_MODULE_ENABLED

class Lib_HAL_PilotLampBasic {
public:
	Lib_HAL_PilotLampBasic(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool inversion = false);
	~Lib_HAL_PilotLampBasic(void);
	void off(void);
	void on(void);
	void once(uint32_t ms);
	void flicker(uint32_t offms, uint32_t onms);

private:
	GPIO_TypeDef *_gpiox;
	uint16_t _pin;
	bool _inversion;
	uint32_t _offms;
	uint32_t _onms;
	bool _ison;
	Lib_HAL_Ticker _ticker;
	static void _onceCallback(Lib_HAL_PilotLampBasic *lamp);
	static void _flickerCallback(Lib_HAL_PilotLampBasic *lamp);
};

////////////////////////////////////////////////////////////////////////////////

class Lib_HAL_PilotLampAdvanced: public Lib_HAL_PilotLampBasic {
public:
	Lib_HAL_PilotLampAdvanced(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool inversion = false, uint32_t oncems = 200, uint32_t offms = 150, uint32_t onms = 50, uint32_t reflashms = 3000);
	~Lib_HAL_PilotLampAdvanced(void);
	void off(void);
	void on(void);
	void once(uint32_t ms);
	void once(void);
	void flicker(uint32_t offms, uint32_t onms);
	void flicker(void);
	void trigger(void);

private:
	uint32_t _oncems;
	uint32_t _offms;
	uint32_t _onms;
	uint32_t _reflashms;
	Lib_HAL_Ticker _ticker;
	static void _callBack(Lib_HAL_PilotLampAdvanced *lamp);
};

#endif /* HAL_GPIO_MODULE_ENABLED */

#endif /* LIB_HAL_PILOTLAMP_H_ */
