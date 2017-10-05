#include <stdlib.h>
#include <stdio.h>

#include "wsvc.h"

char* wsvc_ip_def[] = {
	"0.0.0.0"
};

char* wsvc_port_def[] = {
	"7500"
};

char* wsvc_max_client_def[] = {
	"5"
};

char* wsvc_dev_timeout_def[] = {
	"20"
};

args_t wsvc_arg_list[] = {
	{1, "host-ip", 'I', 1, wsvc_ip_def, "Server Setting", "Host IP dddress"},
	{1, "host-port", 'P', 1, wsvc_port_def, "Host port"},
	{1, "max-client", 'M', 1, wsvc_max_client_def, "Maximum client connection"},
	{0, "dev-path", 'D', 1, NULL, "Wheel Device Setting", "Wheel device path"},
	{0, "dev-baud", 'B', 1, NULL, "Wheel device baudrate"},
	{1, "dev-timeout", 'T', 1, wsvc_dev_timeout_def, "Wheel device timeout"},
	{0, "help", 'H', 0, NULL, "User Interface", "Print detail of arguments"},
	ARGS_TERMINATE
};

int wsvc_arg_check(args_t* argList)
{
	int ret = 0;

	// Checking important arguments
	if(argList[WSVC_DEV_PATH].enable <= 0 && argList[WSVC_DEV_PATH].leading == NULL)
	{
		printf("Argument \'%s\' not set!", argList[WSVC_DEV_PATH].name);
		ret = -1;
		goto RET;
	}

	if(argList[WSVC_DEV_BAUD].enable <= 0 && argList[WSVC_DEV_BAUD].leading == NULL)
	{
		printf("Argument \'%s\' not set!", argList[WSVC_DEV_BAUD].name);
		ret = -1;
		goto RET;
	}

RET:
	return ret;
}

int wsvc_dev_open(wsvc_t* wsvc, args_t* argList)
{
	int ret = 0;
	int tmp;
	const char* tmpPtr;

	// Parse baudrate

	return ret;
}
