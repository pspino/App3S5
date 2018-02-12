#include "XbeeCommands.h"
#include "EthernetInterface.h"
#include "Websocket.h"

DigitalIn cs(p20);
DigitalOut led(LED1);

Serial pc(USBTX,USBRX);
Serial xbee(p28,p27);

I2C accel(p9,p10);

LocalFileSystem local("local");

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



bool readConfigFile(char *PANID, char* URL)
{
	FILE *config = fopen("/local/config.txt","r");
	if(config == NULL)
	{
		pc.printf("no config file were found\n");
		return false;
	}
	
	char buffer[64 + 8 + 128];		// maxkeys + 8 + maxvalues
	while(fgets(buffer, sizeof(buffer), config) != NULL)
	{
		if(buffer[0] == '#') //ignore comments
			continue;
		
		const size_t len = strlen(buffer);
		for (int i = 0; i < len; i++)
			if ((buffer[i] == '\r') || (buffer[i] == '\n')) //checking if there is returns or whitespace.
				buffer[i] = '\0';
			
		char *sp = strchr(buffer,';');
		if(sp != NULL)
		{
			strcpy(URL,sp + 1);	
			*sp = '\0';
			strcpy(PANID,buffer);	
		}
	}
	fclose(config);
	return true;
}

void sendFrame(char *frame)
{
	xbee.printf(frame);
	while(xbee.readable())
	{
		pc.printf("%X",xbee.getc());
	}
}

void requestNetworkAcces()
{
	
}

int main ()
{
	char cmd[2] = {0x2A, 0x03};
	accel.write(0x3B,cmd,2);
	
	char PANID[10];
	char URL[20];
	readConfigFile(PANID,URL);
	pc.printf("PANID = %s\nURL = %s",PANID,URL);
	
	while(1)
	{
		if(cs == 1)
			led = 1;
		else
			led = 0;
		
		int acc = getAccReading();
		char data[64] = "";
		sendFrame(data);
	}
}