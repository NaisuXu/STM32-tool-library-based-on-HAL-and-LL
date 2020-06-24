#include "lib_hal_naisu_protocol.h"

// bufsize must > 5
Lib_HAL_NaisuProtocol::Lib_HAL_NaisuProtocol(uint8_t *buf, uint16_t bufsize) :
		_attachsize(0), _buf(buf), _bufsize(bufsize), _front(0), _msgsize(0) {
}

Lib_HAL_NaisuProtocol::~Lib_HAL_NaisuProtocol(void) {
}

bool Lib_HAL_NaisuProtocol::add(void (*callback)(uint16_t size), uint8_t stx, uint8_t cmd) {
	if (_attachsize >= LIB_HAL_NAISU_PROTOCOL_MAX_SIZE) {
		return false;
	}
	_info[_attachsize].callback = callback;
	_info[_attachsize].stx = stx;
	_info[_attachsize].cmd = cmd;
	_attachsize++;
	return true;
}

// 解析串口收到的数据
// 该方法可以处理一帧数据中有多条消息、消息之外有异常数据的情况
// 但无法处理一条消息触发多次空闲中断才接收完整的情况
void Lib_HAL_NaisuProtocol::parse(uint16_t rxsize) {
	// if (rxsize > _bufsize) return; // 不应该发生这种情况
	_front = 0; // 初始化消息头指针
	loop: // 循环检索数据
	if ((rxsize - _front) < 5) { // 未处理数据不足最小消息长度
		return;
	}
	for (uint16_t i = 0; i < _attachsize; i++) { // 解析当前数据是否符合已注册协议
		if ((_buf[_front] != _info[i].stx) || (_buf[_front + 1] != _info[i].cmd)) { // 判断stx和cmd是否能对上
			continue;
		}
		uint16_t msgsize = ((_buf[_front + 2] << 8) | _buf[_front + 3]) + 5; // 计算消息长度
		if ((_front + msgsize) > rxsize) { // 消息长度超出缓存
			_buf[_front] = 0; // 清零标识以防意外
			_front += 1; //移动消息头指针
			goto loop;
		}
		uint8_t bcc = calBcc(&_buf[_front], msgsize - 1); // 计算校验数据
		if (_buf[_front + msgsize - 1] != bcc) { // 如果校验错误
			_buf[_front] = 0; // 清零标识以防意外
			_front += 1; //移动消息头指针
			goto loop;
		}
		_msgsize = msgsize; // 记录当前消息长度（供read方法使用）
		_info[i].callback(_msgsize); // 消息回调函数
		_buf[_front] = 0; // 清零标识以防意外
		_front += _msgsize; //移动消息头指针
		goto loop;
	}
	_front += 1; //移动消息头指针
	goto loop;
}

uint8_t Lib_HAL_NaisuProtocol::read(uint16_t offset) {
	if (offset >= _msgsize) {
		return 0;
	}
	return _buf[_front + offset];
}

bool Lib_HAL_NaisuProtocol::read(uint8_t *dest, uint16_t offset, uint16_t size) {
	if ((offset + size) > _msgsize) {
		return false;
	}
	uint16_t front = _front + offset;
	uint8_t *tmp = &_buf[front];
	for (uint16_t i = 0; i < size; i++) {
		dest[i] = tmp[i];
	}
	return true;
}

uint8_t Lib_HAL_NaisuProtocol::calBcc(uint8_t *data, uint16_t size) {
	uint8_t bcc = data[0];
	for (uint16_t i = 1; i < size; i++) {
		bcc = bcc ^ data[i];
	}
	return bcc;
}
