#include <reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int

uchar code dis_code[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
uchar code zhc[]={0x10,0x79,0x40,0x79,0x19,0x79,0x24,0x12};
uchar buf[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};

uint k = 71;
uchar sum;
uchar flag = 0;

sbit K1 = P3^4;
sbit K2 = P3^5;
sbit K3 = P3^6;
sbit K4 = P3^7;

void delay(uint t);
void Key1();
void Key2();
void Key3();
void Key4();
void send();

void main() {
	uchar i;
	TMOD = 0x21;
	SCON = 0x40;
	PCON = 0x00;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;

	TH0 = 0x4c;
	TL0 = 0x00;
	EA = 1;
	ET0 = 1;
	TR0 = 0;
	while(1) {
		for (i = 0; i < 4; i++) {
			if (K1 == 0) Key1();
			if (K2 == 0) Key2();
			if (K3 == 0) Key3();
			if (K4 == 0) Key4();
		}
		if (K4 == 1 && flag == 1) send();
	}
}

void delay(uint i) {
	uchar j;
	for (;i>0;i--)
	for (j = 0;j < 125;j++);
}

void Key1() {
	P2 = 0xf0;
	delay(160);
	P2 = ~P2;
	delay(160);
	P2 = 0xff;
	delay(160);
}

void Key2() {
	TR0 = 1;
}

void Key3() {
	uchar l;
	P1 = 0x80;
	for(l=0; l<8; l++) {
		P1=_crol_(P1,1);
		P0=zhc[l];
		delay(2);
		P0=0xff;
	}
	P2 = 0xff;
}

void Key4() {
	SBUF = 0x00;
	while(TI==0);
	TI = 0;
	flag = 1;
}

void send() {
	SBUF = 0xff;
	while(TI==0);
	TI = 0;
	flag = 0;
}

void timer1() interrupt 1{
	TH1 = 0x4c;
	TL1 = 0x00;
	k--;
	if(k <= 0) {
		P2 = ~P2;
		k = 71;
	}
	if (K2 == 1) {
		P2 = 0xff;
		TR0 = 0;
	}
}