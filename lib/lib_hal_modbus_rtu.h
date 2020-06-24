#ifndef LIB_HAL_MODBUS_RTU_H_
#define LIB_HAL_MODBUS_RTU_H_

#include "main.h"

//Name:    CRC-16/MODBUS
//Poly:    0x8005 x16+x15+x2+1
//Init:    0xFFFF
//Refin:   True
//Refout:  True
//Xorout:  0x0000
extern uint16_t Lib_HAL_ModbusRtu_CalCRC(uint8_t *data, uint32_t size);

#define LIB_HAL_MODBUS_RTU_RXBUF_SIZE 256

////////////////////////////////////////////////////////////////////////////////

extern void Lib_HAL_ModbusRtu_ERROR_Callback(uint8_t addr, uint16_t msgsize);

class Lib_HAL_ModbusRtu_Master {
public:
	Lib_HAL_ModbusRtu_Master(uint8_t *buf, uint16_t bufsize =
	LIB_HAL_MODBUS_RTU_RXBUF_SIZE, bool crchighbytefirst = false);
	~Lib_HAL_ModbusRtu_Master(void);
	void parse(uint16_t rxsize);
	uint8_t read(uint16_t offset);
	bool read(uint8_t *dest, uint16_t offset, uint16_t size);
	void addCbFnUnknown(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn02(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn03(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn04(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn05(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn06(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn0F(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn10(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn14(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn15(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn16(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn17(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);
	void addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize), void (*errcallback)(uint8_t addr, uint16_t msgsize) = Lib_HAL_ModbusRtu_ERROR_Callback);

private:
	uint8_t *_buf;
	uint16_t _bufsize;
	uint16_t _crchighbytefirst;
	uint16_t _msgsize;
	void (*_cbfnunknown)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn01)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn01err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn02)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn02err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn03)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn03err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn04)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn04err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn05)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn05err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn06)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn06err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn0F)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn0Ferr)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn10)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn10err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn14)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn14err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn15)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn15err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn16)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn16err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn17)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn17err)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn2B)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn2Berr)(uint8_t addr, uint16_t msgsize);
};

////////////////////////////////////////////////////////////////////////////////

class Lib_HAL_ModbusRtu_Slave {
public:
	Lib_HAL_ModbusRtu_Slave(uint8_t *buf, uint16_t bufsize =
	LIB_HAL_MODBUS_RTU_RXBUF_SIZE, bool crchighbytefirst = false, uint8_t addr = 1);
	~Lib_HAL_ModbusRtu_Slave(void);
	bool setAddr(uint8_t addr);
	void parse(uint16_t rxsize);
	uint8_t read(uint16_t offset);
	bool read(uint8_t *dest, uint16_t offset, uint16_t size);
	void addCbFnUnknown(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn01(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn02(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn03(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn04(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn05(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn06(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn0F(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn10(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn14(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn15(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn16(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn17(void (*callback)(uint8_t addr, uint16_t msgsize));
	void addCbFn2B(void (*callback)(uint8_t addr, uint16_t msgsize));

private:
	uint8_t *_buf;
	uint16_t _bufsize;
	uint16_t _crchighbytefirst;
	uint8_t _addr;
	uint16_t _msgsize;
	void (*_cbfnunknown)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn01)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn02)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn03)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn04)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn05)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn06)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn0F)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn10)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn14)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn15)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn16)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn17)(uint8_t addr, uint16_t msgsize);
	void (*_cbfn2B)(uint8_t addr, uint16_t msgsize);
};

#endif /* LIB_HAL_MODBUS_RTU_H_ */
