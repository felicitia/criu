#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/time.h>

#include <compel/log.h>
#include <compel/infect.h>

static void print_vmsg(unsigned int lvl, const char *fmt, va_list parms)
{
	printf("\tLC%u: ", lvl);
	vprintf(fmt, parms);
}

static int do_rsetsid(int pid)
{
#define err_and_ret(msg)              \
	do {                          \
		fprintf(stderr, msg); \
		return -1;            \
	} while (0)

	int state;
	long ret;
	struct parasite_ctl *ctl;

	compel_log_init(print_vmsg, COMPEL_LOG_DEBUG);

	printf("Stopping task\n");
	state = compel_stop_task(pid);
	if (state < 0)
		err_and_ret("Can't stop task");

	printf("Preparing parasite ctl\n");
	ctl = compel_prepare(pid);
	if (!ctl)
		err_and_ret("Can't prepare for infection");

	ret = -1000;
	if (compel_syscall(ctl, __NR_getpid, &ret, 0, 0, 0, 0, 0, 0) < 0)
		err_and_ret("Can't run rgetpid");

	printf("Remote getpid returned %ld\n", ret);
	if (ret != pid)
		err_and_ret("Pid mismatch!");

	ret = -1000;
	if (compel_syscall(ctl, __NR_setsid, &ret, 0, 0, 0, 0, 0, 0) < 0)
		err_and_ret("Can't run rsetsid");
	printf("Remote setsid returned %ld\n", ret);

	/*
	 * Done. Cure and resume the task.
	 */
	printf("Curing\n");
	if (compel_cure(ctl))
		err_and_ret("Can't cure victim");

	if (compel_resume_task(pid, state, state))
		err_and_ret("Can't unseize task");

	printf("Done\n");
	return 0;
}

static inline int chk(int fd, int val)
{
	int v = 0;
	ssize_t ret;
	
	ret = read(fd, &v, sizeof(v));
	
	if(ret == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}
	else if(ret != sizeof(v)){
		printf("read did not read expected number of bytes: %zu\n", ret);
		exit(EXIT_FAILURE);
	}

	// if (read(fd, &v, sizeof(v)) != sizeof(v)) {
	// 	fprintf(stderr, "read failed\n");
	// }
	printf("%d, want %d\n", v, val);
	return v == val;
}

int main(int argc, char **argv)
{
	int p_in[2], p_out[2], p_err[2], pid, i, pass = 1, sid;
	int status;
	struct timeval tv;
	long start_sec, start_usec, end_sec, end_usec;

	/*
	 * Prepare IO-s and fork the victim binary
	 */
	if (pipe(p_in) || pipe(p_out) || pipe(p_err)) {
		perror("Can't make pipe");
		return -1;
	}

	pid = vfork();
	if (pid == 0) { // if it's the child process
		close(p_in[1]);
		dup2(p_in[0], 0); // redirects the child's standard input to read from the pipe p_in
		close(p_in[0]);
		close(p_out[0]);
		dup2(p_out[1], 1); // redirects the child's standard output to write to the pipe p_out
		close(p_out[1]);
		close(p_err[0]);
		// dup2(p_err[1], 2); // redirects the child's standard error to write to the pipe p_err
		close(p_err[1]);
		// freopen("child_errors.txt", "w", stderr);
		execl("./victim", "victim", NULL); // replace the current process image with a new process image (running ./vimtim instead)
		exit(1);
	}

	close(p_in[0]);
	close(p_out[1]);
	close(p_err[1]);
	sid = getsid(0); // get the session ID of the current process

	/*
	 * Kick the victim once
	 */
	i = 21;
	// parent writes i to the p_in, which is read by the victim
	// victim then writes its sid to its standard output, which the parent reads from p_out shown below
	if (write(p_in[1], &i, sizeof(i)) != sizeof(i)) {
		fprintf(stderr, "write to pipe failed\n");
		return -1;
	}

	printf("Checking the victim session to be %d\n", sid);
	pass = chk(p_out[0], sid); // p_out[0] is read end of p_out, parent reads from p_out[0], child writes to p_out[1]
	if (!pass)
		return 1;
	
	/*
	 * Now do the infection with parasite.c
	 */
	printf("Setsid() the victim\n");
	gettimeofday(&tv, NULL);
	start_sec = tv.tv_sec;
	start_usec = tv.tv_usec;
	printf("Calling do_rsetsid() at Seconds: %ld Microseconds: %ld\n", start_sec, start_usec);
	if (do_rsetsid(pid))
		return 1;
	gettimeofday(&tv, NULL);
	end_sec = tv.tv_sec;
	end_usec = tv.tv_usec;
	printf("Finished do_rsetsid() at Seconds: %ld Microseconds: %ld\n", end_sec, end_usec);
	printf("Time spent in do_rsetsid(): Seconds: %ld Microseconds: %ld\n", (end_sec - start_sec), (end_usec - start_usec));
	/*
	 * Kick the victim again so it tells new session
	 */
	printf("writing i=%d to p_in[i]\n", i);
	if (write(p_in[1], &i, sizeof(i)) != sizeof(i)) {
		fprintf(stderr, "write to pipe failed\n");
		return -1;
	}

	/*
	 * Stop the victim and check the intrusion went well
	 */
	printf("Closing victim stdin\n");
	close(p_in[1]);
	printf("Waiting for victim to die\n");
	wait(&status); // wait for child to exit
	if (WIFEXITED(status)) {
    printf("Child exited with status %d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		printf("Child killed by signal %d\n", WTERMSIG(status));
	} else if (WIFSTOPPED(status)) {
		printf("Child stopped by signal %d\n", WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		printf("Child continued\n");
	} else {
		printf("Unexpected status: %d\n", status);
	}

	printf("Checking the new session to be %d\n", pid);
	pass = chk(p_out[0], pid);

	if (pass)
		printf("All OK\n");
	else
		printf("Something went WRONG\n");

	return 0;
}
