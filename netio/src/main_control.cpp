/*
 * main_control.cpp
 *
 *  Created on: 2015年12月22日
 *      Author: chenzhuo
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include<stdlib.h>

void init_daemon() {
	pid_t pid = fork();
	if ( 0 != pid ) { //父进程退出
		exit(0);
	}
	setpgid(0, 0);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	if (0 != (pid = fork())) {
		exit(0);
	}
	chdir("/");
	umask(0);
}

void CatchSignal(int iSignal) {
	switch (iSignal) {
	case SIGTERM:// Kill -15
		//do_exit
		printf("do exit...........\n");
		break;
	case SIGUSR2:// Kill -12
		//do upgrade,例如此时不清空管道
		printf("do upgrade...........\n");
		break;
	}
}

/*
int main(int argc, char** argv) {
	init_daemon();
	//do init
	signal(SIGTERM, CatchSignal);
	signal(SIGUSR2, CatchSignal);

	//do eventloop
	while (1) {
		sleep(1);
	}

	return 0;
}*/
