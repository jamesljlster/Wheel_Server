#ifndef __WSVC_H__
#define __WSVC_H__

#include <pthread.h>
#include <Wheel.h>
#include <args.h>

enum WSVC_ARG
{
	WSVC_HOST_IP,
	WSVC_HOST_PORT,
	WSVC_MAX_CLIENT,
	WSVC_DEV_PATH,
	WSVC_DEV_BAUD,
	WSVC_DEV_TIMEOUT
};

typedef struct WSVC
{
	WCTRL wCtrl;
	int devTimeout;

	int mutexFlag;
	pthread_mutex_t mutex;
} wsvc_t;

extern args_t wsvc_arg_list[];

#ifdef __cplusplus
extern "C" {
#endif

int wsvc_arg_check(args_t* argList);
int wsvc_dev_open(wsvc_t* wsvc, args_t* argList);
void wsvc_dev_close(wsvc_t* wsvc);

#ifdef __cplusplus
}
#endif

#endif
