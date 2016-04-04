#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
int sign = -1;
void hdl(int sig){
	sign = sig;
}
int main() {
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = hdl;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	act.sa_mask = set;
	sigaction(SIGUSR1, &act, 0);
	sigaction(SIGUSR2, &act, 0);
	sleep(10);
	if (sign == SIGUSR1) {
		printf("%s%d\n", "SIGUSR1 from ", getpid());
	} else if (sign == SIGUSR2) {
		printf("%s%d\n", "SIGUSR2 from ", getpid());
	} else {
		printf("%s", "No signals were caught");
	}
	return 0;
}
