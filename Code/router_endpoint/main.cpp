#include "mbed.h"
#define UINT12_MAX 				4096

DigitalIn cs(p20);
DigitalOut led(LED1);

Serial pc(USBTX,USBRX);
Serial xbee(p28,p27);

I2C accel(p9,p10);

int getAccReading()
{
	const int readAddr 			= 0x3B;
	const int regOutZMSB 		= 0x05;
	const int regOutZLSB 		= 0x06;
	int offset							= 0x00;
	char data[2];
	char cmd[2] = {regOutZMSB,regOutZLSB};
	accel.write(readAddr,cmd,2,true);
	int ack = accel.read(readAddr,data,2);
	int acc = data[0] | data[1] >> 4;
	if(acc > UINT12_MAX /2)
		acc -= UINT12_MAX;
	acc *= 360;
	acc /= 255;
	acc -= offset;
	if(acc < 0)
	{
		acc = abs(acc);
	}
	if(acc > 270)
	{
		acc -= 270;
	}
	if(acc > 180)
	{
		acc -= 180;
	}
	if(acc > 90)
	{
		acc -= 90;
	}
	return acc;
}

void requestNetworkAcces()
{
	
}

int main ()
{
	char cmd[2] = {0x2A, 0x03};
	accel.write(0x3B,cmd,2);
	
	while(1)
	{
		if(cs == 1)
			led = 1;
		else
			led = 0;
		
		int acc = getAccReading();
		//pc.printf("%d\r",acc);
		
		char buffer[64];
		
		xbee.printf("%s",buffer);
		wait(0.01);
	}
}