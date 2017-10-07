#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wsvc.h>
#include <tcpmgr.h>

#define BUF_SIZE	128

#define __strtol(var, str, errMsg, ...) \
{ \
	char* tmpPtr; \
	var = strtol(str, &tmpPtr, 10); \
	if(tmpPtr == str) \
	{ \
		printf(errMsg, ##__VA_ARGS__); \
		ret = -1; \
		goto RET; \
	} \
}

int main(int argc, char* argv[])
{
	int i;
	int ret;
	wsvc_t wsvc;
	tcpmgr_t mgr = NULL;

	char tmpRead;
	char buf[BUF_SIZE];

	char* hostIP;
	int hostPort, maxClient;

	// Zero memory
	memset(&wsvc, 0, sizeof(wsvc_t));

	// Process arguments
	ret = args_parse(wsvc_arg_list, argc, argv, NULL);
	if(ret < 0)
	{
		goto RET;
	}

	ret = wsvc_arg_check(wsvc_arg_list);
	if(ret < 0)
	{
		goto RET;
	}
	else
	{
		printf("Summary:\n");
		args_print_summary(wsvc_arg_list);
		printf("\n");
	}

	// Initial Wheel server
	ret = wsvc_dev_open(&wsvc, wsvc_arg_list);
	if(ret < 0)
	{
		goto RET;
	}

	// Parse server setting
	hostIP = wsvc_arg_list[WSVC_HOST_IP].leading[0];
	__strtol(hostPort, wsvc_arg_list[WSVC_HOST_PORT].leading[0],
			"Failed to convert \'%s\' to host port!\n",
			wsvc_arg_list[WSVC_HOST_PORT].leading[0]
			);
	__strtol(maxClient, wsvc_arg_list[WSVC_MAX_CLIENT].leading[0],
			"Failed to convert \'%s\' to maximum client connection!\n",
			wsvc_arg_list[WSVC_MAX_CLIENT].leading[0]
			);

	// Create tcpmgr
	ret = tcpmgr_create(&mgr, hostIP, hostPort, maxClient);
	if(ret < 0)
	{
		printf("Server initialization failed!\n");
		goto RET;
	}

	// Start tcpmgr
	ret = tcpmgr_start(mgr, wsvc_client_task, &wsvc);
	if(ret < 0)
	{
		printf("tcpmgr_start() failed with error: %d\n", ret);
		goto RET;
	}

	while(1)
	{
		for(i = 0; i < BUF_SIZE; i++)
		{
			ret = scanf("%c", &tmpRead);
			if(ret <= 0)
			{
				continue;
			}

			if(tmpRead == '\n')
			{
				buf[i] = '\0';
				break;
			}
			else
			{
				buf[i] = tmpRead;
			}
		}

		if(strcmp(buf, "stop") == 0)
		{
			break;
		}
		else if(strcmp(buf, "restart") == 0)
		{
			tcpmgr_stop(mgr);

			ret = tcpmgr_start(mgr, wsvc_client_task, &wsvc);
			if(ret < 0)
			{
				printf("tcpmgr_start() failed with error: %d\n", ret);
				goto RET;
			}
		}
	}

	// Stop tcpmgr
	tcpmgr_stop(mgr);

RET:
	// Delete tcpmgr
	tcpmgr_delete(mgr);

	// Cleanup
	wsvc_dev_close(&wsvc);

	return 0;
}
