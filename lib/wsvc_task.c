#include <unistd.h>

#include "wsvc.h"
#include "debug.h"

void mutex_unlock(void* arg);

void wsvc_client_task(void* arg, int sock)
{
	int lockStatus = 0;
	wsvc_t* wsvc = arg;

	LOG("enter, arg = %p, sock = %d", arg, sock);

	pthread_cleanup_push(mutex_unlock, &wsvc->mutex);

	while(1)
	{
		if(!lockStatus)
		{
			pthread_mutex_lock(&wsvc->mutex);
			lockStatus = 1;
		}

		usleep(1000000);
	}

	pthread_cleanup_pop(lockStatus);
	LOG("exit");
}

void mutex_unlock(void* arg)
{
	printf("Unlock Mutex\n");
	pthread_mutex_unlock(arg);
}

