#include <reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define TIME 26
#define RES 10000

uchar code dis_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar code dis_code_point[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
uchar array[] = {0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

uint k = 200 * TIME;
uchar sum;
uchar flag = 0;

sbit K1 = P2^4;
sbit K2 = P2^5;
sbit K3 = P2^6;
sbit K4 = P2^7;
sbit CS = P3^3;
sbit CLK = P3^2;
sbit DIN = P3^4;
sbit SDO = P3^5;
sbit CS2 = P3^6;
sbit SCLK = P3^7;

void delay(uint t);
void Key1();
void Key2();
void Key3();
void Key4();
short int detacte();
void DA_OUTPUT(uint value);
void delay18us();
uchar TLC549_Convert();

void main() {
	short int old_status, status = -1;
	TMOD = 0x01;
	TH0 = 0xee;
	TL0 = 0x00;
	EA = 1;
	ET0 = 1;
	TR0 = 0;
	while(1) {
		old_status = status;
		status = detacte();
		if (old_status != status) P0 = 0xff;
		switch(status) {
			case 0:
				Key1();
				break;
			case 1:
				Key2();
				break;
			case 2:
				Key3();
				break;
			case 3:
				Key4();
				break;
			default:
				P0 = 0xff;
		}
	}
}

void delay(uint i) {
	uchar j;
	for (;i>0;i--)
	for (j = 0;j < 125;j++);
}

void delay18us() {
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
}

short int detacte() {
	P2 = 0xf0;
	if ((short int)K1 + (short int)K2 + (short int)K3 + (short int)K4 != 3) return -1;
	if (K1 == 0) return 0;
	else if (K2 == 0) return 1;
	else if (K3 == 0) return 2;
	else if (K4 == 0) return 3;
	else return -1;
}

void Key1() {
	uchar i;
	for (i = 0; i < 15; i++) {
		if ((short int)K1 + (short int)K2 + (short int)K3 + (short int)K4 != 3) break;
		P0 = array[14-i];
		delay(500);
	}
}

void Key2() {
	TR0 = 1;
}

void Key3() {
	int i;
	for (i = 0; i < 1025; i+=4) {
		if (K3) DA_OUTPUT(0);
		if ((short int)K1 + (short int)K2 + (short int)K3 + (short int)K4 != 3) break;
		DA_OUTPUT(i);
		delay(20);
	}
	for (i = 1024; i >= 0; i-=4) {
		if (K3) DA_OUTPUT(0);
		if ((short int)K1 + (short int)K2 + (short int)K3 + (short int)K4 != 3) break;
		DA_OUTPUT(i);
		delay(20);
	}
	delay(100);
}

void Key4() {
	int result;
	uchar num[4];
	uchar l;
	int temp;
	CS2=1;
	SCLK=0;
	SDO=1;
	while(!K4) {
		result = RES / 255 * TLC549_Convert();
		num[0] = result / 1000;
		num[1] = (result - num[0]*1000) / 100;
		num[2] = (result - num[0]*1000 - num[1]*100) / 10;
		num[3] = result % 10;
		temp = 0x80;
		for(l=0; l<4; l++) {
			delay(2);
			temp = _crol_(temp,1);
			P2 = temp | 0xf0;
			if(l < 3) {
				P1 = dis_code[num[l]];
			}else {
				P1 = dis_code_point[num[l]];
			}
			delay(2);
			P1 = 0xff;
		}
		P1 = 0xff;
	}
}

void timer1() interrupt 1{
	TH0 = 0xee;
	TL0 = 0x00;
	if(k == 200 * TIME) P0 = ~P0;
	else if(k == 200 * (TIME - 1)) P0 = ~P0;
	k--;
	if(k <= 0) {
		k = 200 * TIME;
	}
	if (K2 == 1) {
		P0 = 0xff;
		k = 200 * TIME;
		TR0 = 0;
	}
}

void DA_OUTPUT(uint value) {
	unsigned char i;
	value<<=6;
	CS=0;
	CLK=0;
	for(i = 0; i < 12; i++) {
		DIN=(bit)(value&0x8000);
		CLK=1;
		value<<=1;
		CLK=0;
	}
	CS=1;
	CLK=0;
}

uchar TLC549_Convert() {
	uchar i, temp;
	CS2 = 0;
	delay18us();
	for(i = 0; i < 8; i++) {
		if(SDO==1)temp=temp|0x01;
		if(i<7)temp=temp<<1;
		SCLK=1;
		_nop_();_nop_();_nop_();_nop_();
		SCLK=0;
		_nop_();_nop_();
	}
	CS2=1;
	return temp;
}