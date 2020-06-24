#ifndef LIB_FAKEHEAP_H_
#define LIB_FAKEHEAP_H_

#include "main.h"

class Lib_FakeHeap {
public:
	Lib_FakeHeap(uint8_t *buf, uint16_t size);
	~Lib_FakeHeap(void);
	uint8_t* get(uint16_t size);

private:
	uint8_t *_buf;
	uint16_t _size;
	uint16_t _index;
};

#endif /* LIB_FAKEHEAP_H_ */
