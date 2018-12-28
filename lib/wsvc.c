#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wsvc.h"

#define WSVC_INIT_TIMEOUT 3000

char* wsvc_ip_def[] = {"0.0.0.0"};

char* wsvc_port_def[] = {"7500"};

char* wsvc_max_client_def[] = {"5"};

char* wsvc_dev_timeout_def[] = {"20"};

char* wsvc_wdog_timeout_def[] = {"1000"};

args_t wsvc_arg_list[] = {
    {1, "host-ip", 'I', 1, wsvc_ip_def, "Server Setting", "Host IP dddress"},
    {1, "host-port", 'P', 1, wsvc_port_def, NULL, "Host port"},
    {1, "max-client", 'M', 1, wsvc_max_client_def, NULL,
     "Maximum client connection"},
    {0, "dev-path", 'D', 1, NULL, "Wheel Device Setting", "Wheel device path"},
    {0, "dev-baud", 'B', 1, NULL, NULL, "Wheel device baudrate"},
    {1, "dev-timeout", 'T', 1, wsvc_dev_timeout_def, NULL,
     "Wheel device timeout"},
    {1, "wdog-timeout", 'W', 1, wsvc_wdog_timeout_def, NULL,
     "Watchdog timeout"},
    {0, "help", 'H', 0, NULL, "User Interface", "Print detail of arguments"},
    ARGS_TERMINATE};

int wsvc_arg_check(args_t* argList)
{
    int ret = 0;

    // Checking important arguments
    if (argList[WSVC_DEV_PATH].enable <= 0 &&
        argList[WSVC_DEV_PATH].leading == NULL)
    {
        printf("Argument \'%s\' not set!\n", argList[WSVC_DEV_PATH].name);
        ret = -1;
    }

    if (argList[WSVC_DEV_BAUD].enable <= 0 &&
        argList[WSVC_DEV_BAUD].leading == NULL)
    {
        printf("Argument \'%s\' not set!\n", argList[WSVC_DEV_BAUD].name);
        ret = -1;
    }

    return ret;
}

int wsvc_dev_open(wsvc_t* wsvc, args_t* argList)
{
    int ret = 0;
    int baud, timeout;
    char* tmpPtr;

    // Zero memory
    memset(wsvc, 0, sizeof(wsvc_t));

    // Parse baudrate
    baud = strtol(argList[WSVC_DEV_BAUD].leading[0], &tmpPtr, 10);
    if (tmpPtr == argList[WSVC_DEV_BAUD].leading[0])
    {
        printf("Failed to parse \'%s\' to baudrate!\n", tmpPtr);
        ret = -1;
        goto RET;
    }

    // Parse timeout
    timeout = strtol(argList[WSVC_DEV_TIMEOUT].leading[0], &tmpPtr, 10);
    if (tmpPtr == argList[WSVC_DEV_TIMEOUT].leading[0])
    {
        printf("Failed to parse \'%s\' to timeout!\n", tmpPtr);
        ret = -1;
        goto RET;
    }
    else
    {
        wsvc->devTimeout = timeout;
    }

    // Parse watchdog timeout
    timeout = strtol(argList[WSVC_WDOG_TIMEOUT].leading[0], &tmpPtr, 10);
    if (tmpPtr == argList[WSVC_WDOG_TIMEOUT].leading[0])
    {
        printf("Failed to parse \'%s\' to watchdog timeout!\n", tmpPtr);
        ret = -1;
        goto RET;
    }
    else
    {
        wsvc->wdogTimeout = timeout;
    }

    // Create mutex
    ret = pthread_mutex_init(&wsvc->mutex, NULL);
    if (ret < 0)
    {
        printf("Mutex creation failed!\n");
        goto RET;
    }
    else
    {
        wsvc->mutexFlag = 1;
    }

    // Open device
    ret = WCTRL_Init(&wsvc->wCtrl, argList[WSVC_DEV_PATH].leading[0], baud,
                     WSVC_INIT_TIMEOUT);
    if (ret < 0)
    {
        printf("Failed to open device with path: \'%s\', baudrate: %d\n",
               argList[WSVC_DEV_PATH].leading[0], baud);
        goto RET;
    }

    // Start watchdog task
    if (wsvc->wdogTimeout > 0)
    {
        // Set watchdog time left and flag
        wsvc->wdogTimeLeft = wsvc->wdogTimeout;
        wsvc->wdogTaskFlag = 1;

        // Start watchdog task
        ret = pthread_create(&wsvc->wdogTask, NULL, wsvc_wdog_task, wsvc);
        if (ret != 0)
        {
            ret = -1;
            printf("Failed to start watchdog task!\n");
            goto RET;
        }
        else
        {
            wsvc->wdogTaskStatus = 1;
        }
    }

    wsvc->sal = 255;
    wsvc->sar = 255;

RET:
    return ret;
}

void wsvc_dev_close(wsvc_t* wsvc)
{
    // Stop watchdog task
    if (wsvc->wdogTaskStatus > 0)
    {
        pthread_cancel(wsvc->wdogTask);
        pthread_join(wsvc->wdogTask, NULL);
    }

    // Close device
    WCTRL_Close(wsvc->wCtrl);

    // Destroy mutex
    if (wsvc->mutexFlag > 0)
    {
        pthread_mutex_destroy(&wsvc->mutex);
    }
}
