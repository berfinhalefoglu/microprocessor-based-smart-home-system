#include<reg51.h>
#define DATA_BUS P0
#define ADC_DATA P1

sbit WRadc = P3^3;
sbit RDadc = P3^4;
sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
sbit BZ = P2^3;

typedef unsigned char uchar;
typedef unsigned int uint;

void LCD_init(void);
void lcdcmd(uchar value);
void lcddata(uchar value);
void printascii(uchar value);
void string(uchar ad, uchar * s);
void convert_display(uchar temp);

void delay(uint msec)
{
    uint i,j;
    for(i = 0; i < msec; i++)
        for(j = 0; j < 1275; j++);
}

void main()
{
    uchar temp = 0;
    LCD_init();
    while(1)
    {
        string(0x82, "TEMPERATURE:");

        RDadc = 1;
        WRadc = 0;
        delay(1);
        WRadc = 1;
        delay(1);
        RDadc = 0;
        delay(1);

        temp = ADC_DATA;
        convert_display(temp);
    }
}

void lcdcmd(uchar value)
{
    DATA_BUS = value;
    RS = 0;
    RW = 0;
    EN = 1;
    delay(1);
    EN = 0;
}

void lcddata(uchar value)
{
    DATA_BUS = value;
    RS = 1;
    RW = 0;
    EN = 1;
    delay(1);
    EN = 0;
}

void printascii(uchar value)
{
    value = value + 0x30;
    lcddata(value);
}

void LCD_init()
{
    lcdcmd(0x38);
    lcdcmd(0x0C);
    lcdcmd(0x06);
    lcdcmd(0x01);
    delay(1);
}

void string(uchar ad, uchar * s)
{
    lcdcmd(ad);
    while(*s > 0)
    {
        lcddata(*s++);
        delay(1);
    }
}

void convert_display(uchar temp)
{
    uchar h, t, u;
    lcdcmd(0xC6);
    if(temp >= 29)
        BZ = 1;
    else
        BZ = 0;

    h = temp / 100;
    t = (temp / 10) % 10;
    u = temp % 10;

    printascii(h);
    printascii(t);
    printascii(u);
    lcddata(0xDF);
    lcddata('C');
}
