#include "lib_LL_modbus_rtu.h"

#ifdef USE_FULL_LL_DRIVER

static const uint16_t kLibCrcTab_Crc16Poly0x8005Ref1[256] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
		0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980,
		0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501,
		0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
		0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681,
		0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
		0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800,
		0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
		0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780,
		0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701,
		0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040, };

//Name:    CRC-16/MODBUS
//Poly:    0x8005 x16+x15+x2+1
//Init:    0xFFFF
//Refin:   True
//Refout:  True
//Xorout:  0x0000
uint16_t Lib_LL_ModbusRtu_CalCRC(uint8_t *data, uint32_t size) {
	uint16_t crc = 0xFFFF;
	uint32_t i;
	for (i = 0; i < size; i++) {
		crc = (crc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(crc & 0xFF) ^ *data];
		data++;
	}
	return crc;
}

////////////////////////////////////////////////////////////////////////////////

static void Lib_LL_ModbusRtu_Default_Callback(uint8_t addr, uint16_t msgsize) {
}

#define FN_HANDLE(cb) \
		if (_msgsize > size) { \
			return; \
		} \
		if (check()) { \
			cb(_buf[_front], _msgsize); \
			_front = (_front + _msgsize) % _bufsize; \
		} else { \
			_front = (_front + 1) % _bufsize; \
		} \
		goto loop; \

////////////////////////////////////////////////////////////////////////////////

void Lib_LL_ModbusRtu_ERROR_Callback(uint8_t addr, uint16_t msgsize) {
}

Lib_LL_ModbusRtu_Master::Lib_LL_ModbusRtu_Master(uint8_t *buf, uint16_t bufsize, bool crchighbytefirst) :
		_buf(buf), _bufsize(bufsize), _crchighbytefirst(crchighbytefirst), _front(0), _rear(0), _msgsize(0) {
	_cbfn01 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn01err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn02 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn02err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn03 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn03err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn04 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn04err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn05 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn05err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn06 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn06err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn0F = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn0Ferr = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn10 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn10err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn14 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn14err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn15 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn15err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn16 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn16err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn17 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn17err = Lib_LL_ModbusRtu_ERROR_Callback;
	_cbfn2B = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn2Berr = Lib_LL_ModbusRtu_ERROR_Callback;
}

Lib_LL_ModbusRtu_Master::~Lib_LL_ModbusRtu_Master(void) {
}

void Lib_LL_ModbusRtu_Master::parse(uint16_t rxrear) {
	_rear = rxrear;
	uint16_t size;
	loop: size = (_rear + _bufsize - _front) % _bufsize;
	if (size < 4) { // 收到数据长度不满足指令最低需求
		return;
	}
	if ((_buf[_front] == 0) || (_buf[_front] > 247)) // 指令地址非从机地址
			{
		_front = (_front + 1) % _bufsize;
		goto loop;
	}
	switch (_buf[(_front + 1) % _bufsize]) { // 功能码检索
	case 0x01:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn01)
	case 0x81:
		_msgsize = 5;
		FN_HANDLE(_cbfn01err)
	case 0x02:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn02)
	case 0x82:
		_msgsize = 5;
		FN_HANDLE(_cbfn02err)
	case 0x03:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn03)
	case 0x83:
		_msgsize = 5;
		FN_HANDLE(_cbfn03err)
	case 0x04:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn04)
	case 0x84:
		_msgsize = 5;
		FN_HANDLE(_cbfn04err)
	case 0x05:
		_msgsize = 8;
		FN_HANDLE(_cbfn05)
	case 0x85:
		_msgsize = 5;
		FN_HANDLE(_cbfn05err)
	case 0x06:
		_msgsize = 8;
		FN_HANDLE(_cbfn06)
	case 0x86:
		_msgsize = 5;
		FN_HANDLE(_cbfn06err)
	case 0x0F:
		_msgsize = 8;
		FN_HANDLE(_cbfn0F)
	case 0x8F:
		_msgsize = 5;
		FN_HANDLE(_cbfn0Ferr)
	case 0x10:
		_msgsize = 8;
		FN_HANDLE(_cbfn10)
	case 0x90:
		_msgsize = 5;
		FN_HANDLE(_cbfn10err)
	case 0x14:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn14)
	case 0x94:
		_msgsize = 5;
		FN_HANDLE(_cbfn14err)
	case 0x15:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn15)
	case 0x95:
		_msgsize = 5;
		FN_HANDLE(_cbfn15err)
	case 0x16:
		_msgsize = 10;
		FN_HANDLE(_cbfn16)
	case 0x96:
		_msgsize = 5;
		FN_HANDLE(_cbfn16err)
	case 0x17:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn17)
	case 0x97:
		_msgsize = 5;
		FN_HANDLE(_cbfn17err)
	case 0x2B:
		_msgsize = 13;
		FN_HANDLE(_cbfn2B)
	case 0xAB:
		_msgsize = 5;
		FN_HANDLE(_cbfn2Berr)
	default:
		break;
	}
	_front = (_front + 1) % _bufsize;
	goto loop;
}

uint8_t Lib_LL_ModbusRtu_Master::read(uint16_t offset) {
	if (offset >= _msgsize) {
		return 0;
	}
	return _buf[(_front + offset) % _bufsize];
}

bool Lib_LL_ModbusRtu_Master::read(uint8_t *dest, uint16_t offset, uint16_t size) {
	if ((offset + size) > _msgsize) {
		return false;
	}
	uint16_t front = (_front + offset) % _bufsize;
	uint8_t *tmp = &_buf[front];
	if ((front + size) <= _bufsize) {
		for (int i = 0; i < size; i++) {
			dest[i] = tmp[i];
		}
	} else {
		uint16_t len = _bufsize - front;
		for (int i = 0; i < len; i++) {
			dest[i] = tmp[i];
		}
		tmp = &dest[len];
		len = size - len;
		for (int i = 0; i < len; i++) {
			tmp[i] = _buf[i];
		}
	}
	return true;
}

bool Lib_LL_ModbusRtu_Master::check(void) {
	uint16_t calcrc = 0xFFFF;
	uint8_t *tmp = &_buf[_front];
	if ((_front + _msgsize - 2) <= _bufsize) {
		for (int i = 0; i < (_msgsize - 2); i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ tmp[i]];
		}
	} else {
		uint16_t len = _bufsize - _front;
		for (int i = 0; i < len; i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ tmp[i]];
		}
		len = _msgsize - len - 2;
		for (int i = 0; i < len; i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ _buf[i]];
		}
	}
	if (!_crchighbytefirst) {
		calcrc = (calcrc << 8) | ((calcrc >> 8) & 0x00FF);
	}
	uint16_t rawcrc = (_buf[(_front + _msgsize - 2) % _bufsize] << 8) | _buf[(_front + _msgsize - 1) % _bufsize];
	if (rawcrc != calcrc) {
		return false;
	}
	return true;
}

void Lib_LL_ModbusRtu_Master::addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn01 = callback;
	_cbfn01err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn02(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn02 = callback;
	_cbfn02err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn03(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn03 = callback;
	_cbfn03err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn04(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn04 = callback;
	_cbfn04err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn05(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn05 = callback;
	_cbfn05err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn06(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn06 = callback;
	_cbfn06err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn0F(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn0F = callback;
	_cbfn0Ferr = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn10(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn10 = callback;
	_cbfn10err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn14(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn14 = callback;
	_cbfn14err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn15(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn15 = callback;
	_cbfn15err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn16(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn16 = callback;
	_cbfn16err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn17(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn17 = callback;
	_cbfn17err = errcallback;
}

void Lib_LL_ModbusRtu_Master::addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn2B = callback;
	_cbfn2Berr = errcallback;
}

////////////////////////////////////////////////////////////////////////////////

Lib_LL_ModbusRtu_Slave::Lib_LL_ModbusRtu_Slave(uint8_t *buf, uint16_t bufsize, bool crchighbytefirst, uint8_t addr) :
		_buf(buf), _bufsize(bufsize), _crchighbytefirst(crchighbytefirst), _addr(addr), _front(0), _rear(0), _msgsize(0) {
	_cbfn01 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn02 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn03 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn04 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn05 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn06 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn0F = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn10 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn14 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn15 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn16 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn17 = Lib_LL_ModbusRtu_Default_Callback;
	_cbfn2B = Lib_LL_ModbusRtu_Default_Callback;
}

Lib_LL_ModbusRtu_Slave::~Lib_LL_ModbusRtu_Slave(void) {
}

bool Lib_LL_ModbusRtu_Slave::setAddr(uint8_t addr) {
	if ((addr == 0) || (addr > 247)) { // 非从机可设置地址
		return false;
	}
	_addr = addr;
	return true;
}

void Lib_LL_ModbusRtu_Slave::parse(uint16_t rxrear) {
	_rear = rxrear;
	uint16_t size;
	loop: size = (_rear + _bufsize - _front) % _bufsize;
	if (size < 4) { // 收到数据长度不满足指令最低需求
		return;
	}
	if (!((_buf[_front] == _addr) || (_buf[_front] == 0))) // 指令地址非本模块地址或广播地址
	{
		_front = (_front + 1) % _bufsize;
		goto loop;
	}
	switch (_buf[(_front + 1) % _bufsize]) { // 功能码检索
	case 0x01:
		_msgsize = 8;
		FN_HANDLE(_cbfn01)
	case 0x02:
		_msgsize = 8;
		FN_HANDLE(_cbfn02)
	case 0x03:
		_msgsize = 8;
		FN_HANDLE(_cbfn03)
	case 0x04:
		_msgsize = 8;
		FN_HANDLE(_cbfn04)
	case 0x05:
		_msgsize = 8;
		FN_HANDLE(_cbfn05)
	case 0x06:
		_msgsize = 8;
		FN_HANDLE(_cbfn06)
	case 0x0F:
		if (size < 7)
			return;
		_msgsize = _buf[(_front + 6) % _bufsize] + 9;
		FN_HANDLE(_cbfn0F)
	case 0x10:
		if (size < 7)
			return;
		_msgsize = _buf[(_front + 6) % _bufsize] + 9;
		FN_HANDLE(_cbfn10)
	case 0x14:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn14)
	case 0x15:
		_msgsize = _buf[(_front + 2) % _bufsize] + 5;
		FN_HANDLE(_cbfn15)
	case 0x16:
		_msgsize = 10;
		FN_HANDLE(_cbfn16)
	case 0x17:
		if (size < 11)
			return;
		_msgsize = _buf[(_front + 10) % _bufsize] + 13;
		FN_HANDLE(_cbfn17)
	case 0x2B:
		_msgsize = 7;
		FN_HANDLE(_cbfn2B)
	default:
		break;
	}
	_front = (_front + 1) % _bufsize;
	goto loop;
}

uint8_t Lib_LL_ModbusRtu_Slave::read(uint16_t offset) {
	if (offset >= _msgsize) {
		return 0;
	}
	return _buf[(_front + offset) % _bufsize];
}

bool Lib_LL_ModbusRtu_Slave::read(uint8_t *dest, uint16_t offset, uint16_t size) {
	if ((offset + size) > _msgsize) {
		return false;
	}
	uint16_t front = (_front + offset) % _bufsize;
	uint8_t *tmp = &_buf[front];
	if ((front + size) <= _bufsize) {
		for (int i = 0; i < size; i++) {
			dest[i] = tmp[i];
		}
	} else {
		uint16_t len = _bufsize - front;
		for (int i = 0; i < len; i++) {
			dest[i] = tmp[i];
		}
		tmp = &dest[len];
		len = size - len;
		for (int i = 0; i < len; i++) {
			tmp[i] = _buf[i];
		}
	}
	return true;
}

bool Lib_LL_ModbusRtu_Slave::check(void) {
	uint16_t calcrc = 0xFFFF;
	uint8_t *tmp = &_buf[_front];
	if ((_front + _msgsize - 2) <= _bufsize) {
		for (int i = 0; i < (_msgsize - 2); i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ tmp[i]];
		}
	} else {
		uint16_t len = _bufsize - _front;
		for (int i = 0; i < len; i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ tmp[i]];
		}
		len = _msgsize - len - 2;
		for (int i = 0; i < len; i++) {
			calcrc = (calcrc >> 8) ^ kLibCrcTab_Crc16Poly0x8005Ref1[(calcrc & 0xFF) ^ _buf[i]];
		}
	}
	if (!_crchighbytefirst) {
		calcrc = (calcrc << 8) | ((calcrc >> 8) & 0x00FF);
	}
	uint16_t rawcrc = (_buf[(_front + _msgsize - 2) % _bufsize] << 8) | _buf[(_front + _msgsize - 1) % _bufsize];
	if (rawcrc != calcrc) {
		return false;
	}
	return true;
}

void Lib_LL_ModbusRtu_Slave::addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn01 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn02(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn02 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn03(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn03 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn04(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn04 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn05(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn05 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn06(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn06 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn0F(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn0F = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn10(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn10 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn14(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn14 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn15(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn15 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn16(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn16 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn17(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn17 = callback;
}

void Lib_LL_ModbusRtu_Slave::addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize)) {
	_cbfn2B = callback;
}

#endif /* LL_UART_MODULE_ENABLED */

