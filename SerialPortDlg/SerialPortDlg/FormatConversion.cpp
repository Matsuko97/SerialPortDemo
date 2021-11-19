#include "include.h"

BYTE HexChar(char c)
{
    if((c>='0')&&(c<='9'))
        return c-'0';     //将0-9的数字字符转为十六进制格式
    else if((c>='A')&&(c<='F'))
        return c-'A'+10;  //将A-F的字符转为十六进制格式，例如字符'C'-'A'+10=12=0x0C
    else if((c>='a')&&(c<='f'))
        return c-'a'+10;   //将a-f的字符转为十六进制格式
    else
        return 0x10;
}

int Str2Hex(TCHAR str[], BYTE *data)
{
    int t,t1;
    int rlen=0,len=_tcsclen(str);
    if(len==1)
    {
        char h=str[0];
        t=HexChar(h);
        data[0]=(BYTE)t;
        rlen++;
    }

	for(int i=0;i<len;)
    {
        char l,h=str[i];
        if(h==' ')
        {
            i++;
            continue;
        }
        i++;
        if(i>=len)
            break;
        l=str[i];
        t=HexChar(h);
        t1=HexChar(l);
        if((t==16)||(t1==16))//判断为非法的16进制数
            break;
        else
            t=t*16+t1;
        i++;
        data[rlen]=(BYTE)t;
        rlen++;
    }
    return rlen;
}

