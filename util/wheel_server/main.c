#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wsvc.h>

int main(int argc, char* argv[])
{
	int ret;
	wsvc_t wsvc;

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


RET:
	// Cleanup
	wsvc_dev_close(&wsvc);

	return 0;
}
