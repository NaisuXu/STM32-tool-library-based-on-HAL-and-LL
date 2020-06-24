#ifndef LIB_CRC_H_
#define LIB_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

// Calculate the CRC directly.
unsigned char Lib_Crc8_Cal(unsigned char *data, unsigned int size, unsigned char poly, unsigned char init, unsigned char xorout, unsigned char reversed);
unsigned short int Lib_Crc16_Cal(unsigned char *data, unsigned int size, unsigned short int poly, unsigned short int init, unsigned short int xorout, unsigned char reversed);
unsigned int Lib_Crc32_Cal(unsigned char *data, unsigned int size, unsigned int poly, unsigned int init, unsigned int xorout, unsigned char reversed);

// Select the table-lookup method of computing the CRC By setting this to 1.
// The table-lookup method will consume RAM, but faster.
#ifndef LIB_CRC_TABLE
#define LIB_CRC_TABLE 1U
#endif

#if LIB_CRC_TABLE

//Name:    CRC-8
//Poly:    0x07	x8+x2+x+1
//Init:    0x00
//Refin:   False
//Refout:  False
//Xorout:  0x00
unsigned char Lib_Crc8_Tab(unsigned char *data, unsigned int size);

//Name:    CRC-8/ITU
//Poly:    0x07	x8+x2+x+1
//Init:    0x00
//Refin:   False
//Refout:  False
//Xorout:  0x55
unsigned char Lib_Crc8_Tab_ITU(unsigned char *data, unsigned int size);

//Name:    CRC-8/ROHC
//Poly:    0x07 x8+x2+x+1
//Init:    0xFF
//Refin:   True
//Refout:  True
//Xorout:  0x00
unsigned char Lib_Crc8_Tab_ROHC(unsigned char *data, unsigned int size);

//Name:    CRC-8/MAXIM
//Poly:    0x31 x8+x5+x4+1
//Init:    0x00
//Refin:   True
//Refout:  True
//Xorout:  0x00
unsigned char Lib_Crc8_Tab_MAXIM(unsigned char *data, unsigned int size);

//Name:    CRC-16/IBM
//Poly:    0x8005 x16+x15+x2+1
//Init:    0x0000
//Refin:   True
//Refout:  True
//Xorout:  0x0000
unsigned short int Lib_Crc16_Tab_IBM(unsigned char *data, unsigned int size);

//Name:    CRC-16/MAXIM
//Poly:    0x8005 x16+x15+x2+1
//Init:    0x0000
//Refin:   True
//Refout:  True
//Xorout:  0xFFFF
unsigned short int Lib_Crc16_Tab_MAXIM(unsigned char *data, unsigned int size);

//Name:    CRC-16/USB
//Poly:    0x8005 x16+x15+x2+1
//Init:    0xFFFF
//Refin:   True
//Refout:  True
//Xorout:  0xFFFF
unsigned short int Lib_Crc16_Tab_USB(unsigned char *data, unsigned int size);

//Name:    CRC-16/MODBUS
//Poly:    0x8005 x16+x15+x2+1
//Init:    0xFFFF
//Refin:   True
//Refout:  True
//Xorout:  0x0000
unsigned short int Lib_Crc16_Tab_MODBUS(unsigned char *data, unsigned int size);

//Name:    CRC-16/CCITT
//Poly:    0x1021 x16+x12+x5+1
//Init:    0x0000
//Refin:   True
//Refout:  True
//Xorout:  0x0000
unsigned short int Lib_Crc16_Tab_CCITT(unsigned char *data, unsigned int size);

//Name:    CRC-16/CCITT-FALSE
//Poly:    0x1021 x16+x12+x5+1
//Init:    0xFFFF
//Refin:   False
//Refout:  False
//Xorout:  0x0000
unsigned short int Lib_Crc16_Tab_CCITTFALSE(unsigned char *data, unsigned int size);

//Name:    CRC-16/X25
//Poly:    0x1021  x16+x12+x5+1
//Init:    0xFFFF
//Refin:   True
//Refout:  True
//Xorout:  0XFFFF
unsigned short int Lib_Crc16_Tab_X25(unsigned char *data, unsigned int size);

//Name:    CRC-16/XMODEM
//Poly:    0x1021 x16+x12+x5+1
//Init:    0x0000
//Refin:   False
//Refout:  False
//Xorout:  0x0000
unsigned short int Lib_Crc16_Tab_XMODEM(unsigned char *data, unsigned int size);

//Name:    CRC-32
//Poly:    0x04C11DB7 x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
//Init:    0xFFFFFFFF
//Refin:   True
//Refout:  True
//Xorout:  0xFFFFFFFF
unsigned int Lib_Crc32_Tab(unsigned char *data, unsigned int size);

//Name:    CRC-32/MPEG-2
//Poly:    0x04C11DB7 x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
//Init:    0xFFFFFFFF
//Refin:   False
//Refout:  False
//Xorout:  0x00000000
unsigned int Lib_Crc32_Tab_MPEG2(unsigned char *data, unsigned int size);

#endif //LIB_CRC_TABLE

#ifdef __cplusplus
}
#endif

#endif /* LIB_CRC_H_ */
