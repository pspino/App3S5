#include "mbed.h"
#include "EthernetInterface.h"

Serial xbee(p28,p27);
Serial pc(USBTX,USBRX);



DigitalOut rst(p26);

typedef struct
{
	char *key;
	char *value;
}config_t;

void (*func_pointer[5])();

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

bool readConfigFile(char *keys, char* values)
{
	FILE *config = fopen("/local/.config","r");
	if(config == NULL)
	{
		error("no config file were found");
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
			strcpy(values,sp + 1);
			*sp = '\0';
			strcpy(keys, buffer);
		}
	}
	fclose(config);
	return true;
}

int main()
{
	//Reading config file content
	char keys[64];
	char values[128];
	config_t config;
	
	if(readConfigFile(keys,values))
	{
		config.key = keys;
		config.value = values;
	}
	else
	{
		//need to cancel the main since there is no config file and thus the PAN cannot work.
		return 1;
	}
	
	
	
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
			pc.printf("%s",readData);
		}
		if()
	}
}