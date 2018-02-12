#include "XbeeCommands.h"

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

//char createFrame(char *data, unsigned int size, char frameType, char frameID, char *commands)
void createFrame(char* data)
{
	char checksum = 0;
	
	char frame[64] = "";
	
	frame[0] = 0x7E;					//Start frame char
	frame[1] = 0x00;					//MSB of length is always 0.
	frame[2] = 0x04; 	//The lenght of data that we are given.
	frame[3] = AT_CMD;				//FrameType
	frame[4] = 0x52;					//frame ID.
	frame[5] = 'I';
	frame[6] = 'D';
	
	for(int i = 3; i < 7; i++)
		checksum += frame[i];
	checksum = 0xFF - checksum;
	
	frame[7] = checksum;
	
	for(int i = 0; i<7; i++)
	{
		*data = frame[i];
		data++;
	}
	//frame[5] = ((char)AT_CMD_ID & 0xFF00) >> 2; 
	//frame[6] = (char)AT_CMD_ID & 0x00FF;
	
}

void sendFrame(char *frame)
{
	xbee.printf(frame);
	while(xbee.readable())
	{
		pc.printf("%X",xbee.getc());
	}
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
		char data[64] = "";
		createFrame(data);
		for(int i =0;i<10;i++)
			pc.printf("%02X",data[i]);
		pc.printf("\r");
		sendFrame(data);
	}
}