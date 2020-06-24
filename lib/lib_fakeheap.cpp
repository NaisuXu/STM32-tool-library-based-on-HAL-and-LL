#include "lib_fakeheap.h"

Lib_FakeHeap::Lib_FakeHeap(uint8_t *buf, uint16_t size) :
		_buf(buf), _size(size), _index(0) {
}

Lib_FakeHeap::~Lib_FakeHeap(void) {
}

uint8_t* Lib_FakeHeap::get(uint16_t size) {
	if ((size == 0) || (size > _size)) {
		return nullptr;
	}
	if ((_index + size) > _size) {
		_index = size;
		return _buf;
	}
	uint8_t *tmp = _buf + _index;
	_index = (_index + size) % _size;
	return tmp;
}
