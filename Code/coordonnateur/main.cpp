#include "XbeeCommands.h"
#include "EthernetInterface.h"
#include "Websocket.h"

Serial xbee(p28,p27);
Serial pc(USBTX,USBRX);

LocalFileSystem local("local");

DigitalOut rst(p26);

void (*func_pointer[5])();

typedef struct
{
	char key[10];
	char value[20];
}config_t;

enum xbeeFrames
{
	START_DELIMITER = 0x7E,
	AT_COMMAND = 0x08,
	TRANSMIT_REQUEST = 0x10,
	CREATE_ROUTE = 0x21
};

void configMode()
{
	int data;
	wait(2);
	xbee.printf("+++");
	while(data != 75)
		if(xbee.readable())
			data = xbee.getc();
}

void createFrame(char* data)
{
	char checksum = 0;
	
	char frame[64] = "";
	
	frame[0] = 0x7E;					//Start frame char
	frame[1] = 0x00;					//MSB of length is always 0.
	frame[2] = 0x04; 					//The lenght of data that we are given.
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

void setPANId()
{
	int panId = 0;
	xbee.printf("ATID %i\r",panId);
}

void allowNodeJoin()
{
	
}

void readXbee(char *data, int length)
{
	int count = 0;
	if(length == 0)
	{ length = 200;}		// this is the max length of writeData.
	
	while(length != count)
	{
		*data = xbee.getc();
		data += 1;
		count++;
	}
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

int main()
{
	//Reading config file content
	char PANID[10];
	char URL[20];
	readConfigFile(PANID,URL);
	pc.printf("PANID = %s\nURL = %s",PANID,URL);
	
	//Reseting Xbee to allow reset of the PAN.
	rst = 0;
	wait(0.4);
	rst = 1;
	
	func_pointer[0] = &setPANId;
	func_pointer[1] = &allowNodeJoin;
	func_pointer[2] = &configMode;
	
	char writeData[200];
	char readData[200];
	int idx = 0;
	while(1)
	{
		if(xbee.readable())
		{
			readXbee(readData,sizeof(readData));
			//pc.printf("%s",readData);
		}
	}
}