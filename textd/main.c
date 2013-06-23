#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "session.h"

int serverbind(void)
{
	int s;

	s = tcplisten("12345");
	if(s == -1) {
		fprintf(stderr, "Failed to bind network socket.");
		exit(EXIT_FAILURE);
	}

	return s;
}

void acceptsession(int s)
{
	int c;

	c = accept(s, NULL, NULL);
	if(c == -1) {
		fprintf(stderr, "accept: %s.\n", strerror(errno));
		return;
	}

	makesession(c);
}

void processinput(int s)
{
	printf("input from %d\n", s);
}

void serverlisten(int s)
{
	int i, n;
	int nfds;
	fd_set rdset, wrset, erset;
	session *ses;

	for(;;) {
		FD_ZERO(&rdset);
		FD_ZERO(&wrset);
		FD_ZERO(&erset);

		nfds = s + 1;
		FD_SET(s, &rdset);

		for(i = 0; i < sessioncap; i++)
		if((ses = getsession(i)) != NULL)
			FD_SET(i, &rdset);

		if(nfds < sessioncap)
			nfds = sessioncap;

		if((n = select(nfds, &rdset, &wrset, &erset, NULL)) == -1) {
			perror("select");
			break;
		}

		if(FD_ISSET(s, &rdset))
			acceptsession(s);
		FD_CLR(s, &rdset);

		for(i = 0; i < sessioncap; i++)
		if((ses = getsession(i)) != NULL)
		if(FD_ISSET(i, &rdset))
			processinput(i);
	}
}

int main(int argc, char *argv[])
{
	int s;

	s = serverbind();
	serverlisten(s);

	close(s);
	exit(EXIT_SUCCESS);
}
