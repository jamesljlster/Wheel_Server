#ifndef __WSVC_H__
#define __WSVC_H__

#include <Wheel.h>
#include <args.h>
#include <pthread.h>

enum WSVC_ARG
{
    WSVC_HOST_IP,
    WSVC_HOST_PORT,
    WSVC_MAX_CLIENT,
    WSVC_DEV_PATH,
    WSVC_DEV_BAUD,
    WSVC_DEV_TIMEOUT,
    WSVC_WDOG_TIMEOUT,
    WSVC_HELP
};

typedef struct WSVC
{
    WCTRL wCtrl;
    int sal;
    int sar;

    int devTimeout;

    int mutexFlag;
    pthread_mutex_t mutex;

    int wdogTimeout;
    int wdogTimeLeft;
    int wdogTaskFlag;
    int wdogTaskStatus;
    pthread_t wdogTask;
} wsvc_t;

extern args_t wsvc_arg_list[];

#ifdef __cplusplus
extern "C"
{
#endif

    int wsvc_arg_check(args_t* argList);
    int wsvc_dev_open(wsvc_t* wsvc, args_t* argList);
    void wsvc_dev_close(wsvc_t* wsvc);
    void wsvc_client_task(void* arg, int sock);

    void* wsvc_wdog_task(void* arg);

#ifdef __cplusplus
}
#endif

#endif
