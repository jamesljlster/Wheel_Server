#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
	int tmpSal, tmpSar;
	int lockStatus = 0;
	wsvc_t* wsvc = arg;
	struct timespec timeTmp;

	char buf[BUF_SIZE] = {0};

	LOG("enter, arg = %p, sock = %d", arg, sock);

	// Setup cleanup handler
	pthread_cleanup_push(mutex_unlock, &wsvc->mutex);

	// Loop for communication
	while(1)
	{
		// Receive a string
		ret = wsvc_client_str_recv(sock, buf, BUF_SIZE);
		if(ret < 0)
		{
			break;
		}

		LOG("Received: %s", buf);

		// Run task
		ret = 0;
		if(strcmp(buf, "WLOCK") == 0)
		{
			if(!lockStatus)
			{
				timeTmp.tv_sec = 0;
				timeTmp.tv_nsec = wsvc->devTimeout * 10000;
				ret = pthread_mutex_timedlock(&wsvc->mutex, &timeTmp);
				if(ret == 0)
				{
					lockStatus = 1;
				}
			}
		}
		else if(strcmp(buf, "WUNLOCK") == 0)
		{
			if(lockStatus)
			{
				ret = pthread_mutex_unlock(&wsvc->mutex);
				if(ret == 0)
				{
					lockStatus = 0;
				}
			}
		}
		else if(strcmp(buf, "WGET") == 0)
		{
			sprintf(buf, "W%03d%03d\x0A", wsvc->sal, wsvc->sar);
			ret = send(sock, buf, strlen(buf), 0);
			if(ret <= 0)
			{
				break;
			}
		}
		else if(strlen(buf) == 7)
		{
			// Lock device
			if(!lockStatus)
			{
				timeTmp.tv_sec = 0;
				timeTmp.tv_nsec = wsvc->devTimeout * 10000;
				ret = pthread_mutex_timedlock(&wsvc->mutex, &timeTmp);
				if(ret == 0)
				{
					lockStatus = 1;
				}
			}

			// Decode and control
			if(lockStatus)
			{
				tmpSal = (buf[1] - '0') * 100 + (buf[2] - '0') * 10 + (buf[3] - '0');
				tmpSar = (buf[4] - '0') * 100 + (buf[5] - '0') * 10 + (buf[6] - '0');
				ret = WCTRL_Control(wsvc->wCtrl, tmpSal, tmpSar, 0);
				if(ret == WCTRL_NO_ERROR)
				{
					wsvc->sal = tmpSal;
					wsvc->sar = tmpSar;
				}

				// Unlock device
				pthread_mutex_unlock(&wsvc->mutex);
				lockStatus = 0;
			}
		}
		else
		{
			ret = -1;
		}

		// Send response
		if(ret == 0)
		{
			ret = send(sock, "WOK\x0A", strlen("WOK\x0A"), 0);
			if(ret <= 0)
			{
				break;
			}
		}
		else if(ret < 0)
		{
			ret = send(sock, "WERR\x0A", strlen("WERR\x0A"), 0);
			if(ret <= 0)
			{
				break;
			}
		}
	}

	// Cleanup
	if(lockStatus)
	{
		pthread_mutex_unlock(&wsvc->mutex);
		lockStatus = 0;
	}

	pthread_cleanup_pop(lockStatus);
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

