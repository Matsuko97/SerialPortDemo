#include "include.h"

static unsigned short crcTable[256] = {0};

void CreatCRCTable(void)
{
    unsigned short remainder;
    unsigned short dividend;
    int bit;
    for(dividend = 0; dividend < 256; dividend++)
    {
        remainder = dividend << (WIDTH - 8);
        for(bit = 0; bit < 8; bit++)
        {
            if(remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = remainder << 1;
            }
        }
        crcTable[dividend] = remainder;
    }
} 

unsigned short crcCompute(unsigned char * message, unsigned int nBytes)
{
    unsigned int offset;
    unsigned char byte;
    unsigned short remainder = INITIAL_REMAINDER;

	for( offset = 0; offset < nBytes; offset++)
    {
        byte = (remainder >> (WIDTH - 8)) ^ message[offset];
        remainder = crcTable[byte] ^ (remainder << 8);
    }

    return (remainder ^ FINAL_XOR_VALUE);
}

/********************************************************************
* Name: CRC-16/XMODEM x16+x12+x5+1
* Poly: 0x1021
* Init: 0x0000
* Refin: False
* Refout: False
* Xorout: 0x0000
* Alias: CRC-16/ZMODEM,CRC-16/ACORN
*******************************************************************/
unsigned short crc16_xmodem(unsigned char *data, unsigned short length)
{
	unsigned short i;
	unsigned short crc = 0; // Initial value
	while(length--)
	{
		crc ^= (unsigned short)(*data++) << 8; // crc ^= (uint16_t)(*data)<<8; data++;
		for (i = 0; i < 8; ++i)
		{
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc <<= 1;
		}
	}
	return crc;
}

/********************************************************************
* Name: CRC-16/DNP x16+x13+x12+x11+x10+x8+x6+x5+x2+1
* Poly: 0x3D65
* Init: 0x0000
* Refin: True
* Refout: True
* Xorout: 0xFFFF
* Use: M-Bus,ect.
*******************************************************************/
unsigned short crc16_dnp(unsigned char *data, unsigned short length)
{
	unsigned short i;
	unsigned short crc = 0; // Initial value
	while(length--)
	{
		crc ^= *data++; // crc ^= *data; data++;
		for (i = 0; i < 8; ++i)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xA6BC; // 0xA6BC = reverse 0x3D65
			else
				crc = (crc >> 1);
		}
	}
	return ~crc; // crc^Xorout
}

/********************************************************************
* Name: CRC-16/MODBUS x16+x15+x2+1
* Poly: 0x8005
* Init: 0xFFFF
* Refin: True
* Refout: True
* Xorout: 0x0000
* Note:
*******************************************************************/
unsigned short crc16_modbus(unsigned char *data, unsigned short length)
{  
    unsigned short i;  
    unsigned short crc = 0xffff;  // Initial value  
    while(length--)  
    {  
        crc ^= *data++;     // crc ^= *data; data++;  
        for (i = 0; i < 8; ++i)  
        {  
            if (crc & 1)  
                crc = (crc >> 1) ^ 0xA001;    // 0xA001 = reverse 0x8005  
            else  
                crc = (crc >> 1);  
        }  
    }  
    return crc;  
} 
