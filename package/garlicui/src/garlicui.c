#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int forward_to_retroarch()
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}

	if (pid == 0)
	{
		// Child process
		execlp("retroarch", "retroarch", NULL);
		perror("execlp");
		return 1;
	}

	// Parent process
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
	{
		printf("Child process exited with status %d\n", WEXITSTATUS(status));
		return WEXITSTATUS(status);
	}

	printf("Child process exited abnormally\n");
	return 1;
}

int main()
{
	return forward_to_retroarch();
}
