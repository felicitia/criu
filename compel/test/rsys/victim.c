#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <errno.h> 
#include <string.h>


int main(int argc, char **argv)
{
	int i;
	int count=0;
	ssize_t num_bytes;
	struct timeval tv;
	
	fprintf(stderr, "\033[1;31m I'm in victim.c main! :)\n\033[0m");
	
	while (1) {
		fprintf(stderr, "\033[1;31m I'm in the BEGINNING of the while loop!\n\033[0m");
		count += 1;
		fprintf(stderr, "\033[1;31m count is: %d\n\033[0m", count);
	
		i = getsid(0); // get session ID of the current process
		fprintf(stderr, "\033[1;31m just used getsid to get i=%d\n\033[0m", i);
		gettimeofday(&tv, NULL);
		fprintf(stderr, "\033[1;31m calling READ at time Seconds: %ld Microseconds: %ld\n\033[0m", tv.tv_sec, tv.tv_usec);
		if (count == 2)
		{
			fprintf(stderr, "\033[1;31m Victim is sleeping for 1s now\n\033[0m");
			sleep(1);
		}
		errno = 0;
		num_bytes = read(0, &i, sizeof(i));
		fprintf(stderr, "\033[1;31m READ value is: %d size is: %ld num_bytes is: %ld\n\033[0m", i, sizeof(i), num_bytes);
		if (num_bytes != sizeof(i)){
			fprintf(stderr, "\033[1;31m after READ, ERROR is: %s\n\033[0m", strerror(errno));
			break;
		}
		fprintf(stderr, "\033[1;31m I'm in the MIDDLE of the while loop!\n\033[0m");
		i = getsid(0); // get session ID of the current process
		fprintf(stderr, "\033[1;31m just used getsid to get i=%d\n\033[0m", i);
		if (write(1, &i, sizeof(i)) != sizeof(i)){
			fprintf(stderr, "\033[1;31m after WRITE, size is not equal, about to break!\n\033[0m");
			break;
		}else{
			fprintf(stderr, "\033[1;31m WRITE value is: %d\n\033[0m", i);
		}
			
		fprintf(stderr,"\033[1;31m I'm at the END of the while loop!\n\033[0m");	
	}

	fprintf(stderr,"\033[1;31m I'm at OUTSIDE of the while loop!\n\033[0m");	

	return 0;
}
