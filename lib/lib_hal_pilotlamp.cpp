#include "lib_hal_pilotlamp.h"

#ifdef HAL_GPIO_MODULE_ENABLED

Lib_HAL_PilotLampBasic::Lib_HAL_PilotLampBasic(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool inversion) :
		_gpiox(GPIOx), _pin(GPIO_Pin), _inversion(inversion) {
}

Lib_HAL_PilotLampBasic::~Lib_HAL_PilotLampBasic(void) {

}

void Lib_HAL_PilotLampBasic::off(void) {
	_ticker.detach();
	HAL_GPIO_WritePin(_gpiox, _pin, _inversion ? GPIO_PIN_SET : GPIO_PIN_RESET);
	_ison = false;
}

void Lib_HAL_PilotLampBasic::on(void) {
	_ticker.detach();
	HAL_GPIO_WritePin(_gpiox, _pin, _inversion ? GPIO_PIN_RESET : GPIO_PIN_SET);
	_ison = true;
}

void Lib_HAL_PilotLampBasic::once(uint32_t ms) {
	HAL_GPIO_WritePin(_gpiox, _pin, _inversion ? GPIO_PIN_RESET : GPIO_PIN_SET);
	_ison = true;
	_ticker.once(ms, _onceCallback, this);
}

void Lib_HAL_PilotLampBasic::_onceCallback(Lib_HAL_PilotLampBasic *lamp) {
	HAL_GPIO_WritePin(lamp->_gpiox, lamp->_pin, lamp->_inversion ? GPIO_PIN_SET : GPIO_PIN_RESET);
	lamp->_ison = false;
}

void Lib_HAL_PilotLampBasic::flicker(uint32_t offms, uint32_t onms) {
	_offms = offms;
	_onms = onms;
	if (_ison) {
		HAL_GPIO_WritePin(_gpiox, _pin, _inversion ? GPIO_PIN_SET : GPIO_PIN_RESET);
		_ison = false;
		_ticker.once(_offms, _flickerCallback, this);
	} else {
		HAL_GPIO_WritePin(_gpiox, _pin, _inversion ? GPIO_PIN_RESET : GPIO_PIN_SET);
		_ison = true;
		_ticker.once(_onms, _flickerCallback, this);
	}
}

void Lib_HAL_PilotLampBasic::_flickerCallback(Lib_HAL_PilotLampBasic *lamp) {
	if (lamp->_ison) {
		HAL_GPIO_WritePin(lamp->_gpiox, lamp->_pin, lamp->_inversion ? GPIO_PIN_SET : GPIO_PIN_RESET);
		lamp->_ison = false;
		lamp->_ticker.once(lamp->_offms, _flickerCallback, lamp);
	} else {
		HAL_GPIO_WritePin(lamp->_gpiox, lamp->_pin, lamp->_inversion ? GPIO_PIN_RESET : GPIO_PIN_SET);
		lamp->_ison = true;
		lamp->_ticker.once(lamp->_onms, _flickerCallback, lamp);
	}
}

////////////////////////////////////////////////////////////////////////////////

Lib_HAL_PilotLampAdvanced::Lib_HAL_PilotLampAdvanced(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool inversion, uint32_t oncems, uint32_t offms, uint32_t onms, uint32_t reflashms) :
		Lib_HAL_PilotLampBasic(GPIOx, GPIO_Pin, inversion), _oncems(oncems), _offms(offms), _onms(onms), _reflashms(reflashms) {
}

Lib_HAL_PilotLampAdvanced::~Lib_HAL_PilotLampAdvanced(void) {
}

void Lib_HAL_PilotLampAdvanced::off(void) {
	_ticker.detach();
	Lib_HAL_PilotLampBasic::off();
}

void Lib_HAL_PilotLampAdvanced::on(void) {
	_ticker.detach();
	Lib_HAL_PilotLampBasic::on();
}

void Lib_HAL_PilotLampAdvanced::once(uint32_t ms) {
	_oncems = ms;
	_ticker.detach();
	Lib_HAL_PilotLampBasic::once(_oncems);
}

void Lib_HAL_PilotLampAdvanced::once(void) {
	_ticker.detach();
	Lib_HAL_PilotLampBasic::once(_oncems);
}

void Lib_HAL_PilotLampAdvanced::flicker(uint32_t offms, uint32_t onms) {
	_offms = offms;
	_onms = onms;
	Lib_HAL_PilotLampBasic::flicker(_offms, _onms);
}

void Lib_HAL_PilotLampAdvanced::flicker(void) {
	Lib_HAL_PilotLampBasic::flicker(_offms, _onms);
}

void Lib_HAL_PilotLampAdvanced::trigger(void) {
	Lib_HAL_PilotLampAdvanced::once(_oncems);
	_ticker.once(_reflashms, _callBack, this);
}

void Lib_HAL_PilotLampAdvanced::_callBack(Lib_HAL_PilotLampAdvanced *lamp) {
	lamp->Lib_HAL_PilotLampBasic::flicker(lamp->_offms, lamp->_onms);
}

#endif /* HAL_GPIO_MODULE_ENABLED */
