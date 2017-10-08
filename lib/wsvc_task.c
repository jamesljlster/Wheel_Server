#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <tcpmgr_sock.h>

#include "wsvc.h"
#include "debug.h"

#define BUF_SIZE	128
#define ASCII_LF	0x0A

#define STR_FLAG_HEAD	0x01
#define STR_FLAG_END	0x02

void mutex_unlock(void* arg);

int wsvc_client_str_recv(int sock, char* buf, int bufLen);

void wsvc_client_task(void* arg, int sock)
{
	int ret;
	int lockStatus = 0;
	wsvc_t* wsvc = arg;

	char buf[BUF_SIZE] = {0};

	LOG("enter, arg = %p, sock = %d", arg, sock);

	// Setup cleanup handler
	pthread_cleanup_push(mutex_unlock, &wsvc->mutex);

	// Loop for communication
	while(1)
	{
		ret = wsvc_client_str_recv(sock, buf, BUF_SIZE);
		if(ret < 0)
		{
			break;
		}

		LOG("Received: %s", buf);
	}

	// Cleanup
	pthread_cleanup_pop(lockStatus);

	if(lockStatus)
	{
		pthread_mutex_unlock(&wsvc->mutex);
	}

	LOG("exit");
}

int wsvc_client_str_recv(int sock, char* buf, int bufLen)
{
	int ret = 0;
	char tmpRead;
	int strFlag = 0;
	int procIndex = 0;

	LOG("enter");

	// Initialize buffer
	memset(buf, 0, sizeof(char) * bufLen);

	while(1)
	{
		// Receive a single character
		ret = recv(sock, &tmpRead, 1, 0);
		if(ret <= 0)
		{
			ret = -1;
			break;
		}

		// If reach string head
		if(tmpRead == 'W')
		{
			// Reset string pointer
			procIndex = 0;

			// Reset flag
			strFlag = STR_FLAG_HEAD;
		}
		else if(tmpRead == '\n')
		{
			strFlag |= STR_FLAG_END;
		}

		// Check flag
		if(strFlag == (STR_FLAG_HEAD | STR_FLAG_END))
		{
			break;
		}

		// Insert character
		if(procIndex < bufLen - 1)
		{
			buf[procIndex++] = tmpRead;
		}
		else
		{
			ret = -1;
			break;
		}
	}

	LOG("exit");
	return ret;
}

void mutex_unlock(void* arg)
{
	LOG("Unlock mutex: %p", arg);
	pthread_mutex_unlock(arg);
}

