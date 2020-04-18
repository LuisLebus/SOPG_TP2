/*=====[Module Name]===========================================================
 * Copyright YYYY Author Compelte Name <author@mail.com>
 * All rights reserved.
 * License: license text or at least name and link
         (example: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Creation Date: YYYY/MM/DD
 */

/*=====[Inclusion of own header]=============================================*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "writer.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/
#define FIFO_NAME         "../myfifo"
#define BUFFER_SIZE       256

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
static int32_t fd = -1;

/*=====[Prototypes (declarations) of private functions]======================*/
static void sigUser1(int sig);
static void sigUser2(int sig);

/*=====[Implementations of public functions]=================================*/
bool serialInterfaceInit(void)
{
	int32_t err;
	char outputBuffer[BUFFER_SIZE];
	struct sigaction sa;

	sa.sa_handler = sigUser1;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGUSR1, &sa, NULL);


	sa.sa_handler = sigUser2;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGUSR2, &sa, NULL);


	/* Create named fifo. -1 means already exists so no action if already exists */
	if ( (err = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
	{
		printf("Error creating named fifo: %d\n", err);
		return false;
	}

	/* Open named fifo. Blocks until other process opens it */
	printf("\nWaiting for readers...\n\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
	{
		printf("Error opening named fifo file: %d\n", fd);
		return false;
	}

	/* open syscalls returned without error -> other process attached to named fifo */
	printf("Got a reader, type some stuff...\n\n");


	/* Loop forever */
	while (1)
	{
		memset(outputBuffer, 0, sizeof(outputBuffer));
		strcpy(outputBuffer, "DATA:\0");

		/* Get some text from console */
		if( fgets(&outputBuffer[5], BUFFER_SIZE, stdin) != NULL )
		{
			outputBuffer[strlen(outputBuffer)] = '\n';

			/* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
			if( write(fd, outputBuffer, strlen(outputBuffer)-1) == -1)
			{
				perror("write");
			}
		}
	}
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/
static void sigUser1(int sig)
{
	if(fd > 0)
	{
		if( write(fd, "SIGN:1\n", strlen("SIGN:1\n") ) == -1)
		{
			write(1, "SIGUSR1 error\n", 14);
		}
	}
}

static void sigUser2(int sig)
{
	if(fd > 0)
	{
		if( write(fd, "SIGN:2\n", strlen("SIGN:2\n") ) == -1)
		{
			write(1, "SIGUSR2 error\n", 14);
		}
	}
}
