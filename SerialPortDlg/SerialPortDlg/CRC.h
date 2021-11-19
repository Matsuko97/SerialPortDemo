#ifndef __CALCRC_H__
#define __CALCRC_H__

#define POLYNOMIAL          0x1021
#define INITIAL_REMAINDER   0xFFFF
#define FINAL_XOR_VALUE     0x0000

#define WIDTH    (8 * sizeof(unsigned short))
#define TOPBIT   (1 << (WIDTH - 1))

void CreatCRCTable(void);
unsigned short crcCompute(unsigned char * message, unsigned int nBytes);
unsigned short crc16_xmodem(unsigned char *data, unsigned short length);
unsigned short crc16_dnp(unsigned char *data, unsigned short length);
unsigned short crc16_modbus(unsigned char *data, unsigned short length);

#endif //__CALCRC_H__