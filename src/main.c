#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "session.h"

static int s;

int serverbind(void)
{
	int s;

	s = tcplisten4("12345");
	if(s == -1) {
		fprintf(stderr, "Failed to bind network socket.");
		exit(EXIT_FAILURE);
	}

	return s;
}

void acceptsession(int s)
{
	int c;

	unsigned char bytes[] = { 255, 253, 31,	// IAC DO NAWS
				  255, 251, 1,	// IAC WILL ECHO
				  255, 253, 34,	// IAC DO LINEMODE
				  255, 250, 34, 1, 0, 255, 240, // IAC SB LINEMODE mask=0 IAC SE
				  255, 253, 24, // IAC DO TERM
				  255, 250, 24, 1, 255, 240, // IAC SB TERM SEND IAC SE
		};

	c = accept(s, NULL, NULL);
	if(c == -1) {
		fprintf(stderr, "accept: %s.\n", strerror(errno));
		return;
	}

	send(c, bytes, sizeof(bytes), 0);

	makesession(c);
}

void recvinput(int s)
{
	unsigned char c;
	size_t n;
	session *ses;

	ses = getsession(s);
	if(ses == NULL) {
		fprintf(stderr, "Unrecognized session %d.\n", s);
		close(s);
		return;
	}

	n = recv(s, &c, 1, 0);
	if(n == 0) {
		quitsession(s);
		return;
	}

	parse(ses, c);
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
			if(errno != EINTR)
				perror("select");
			break;
		}

		if(FD_ISSET(s, &rdset))
			acceptsession(s);
		FD_CLR(s, &rdset);

		for(i = 0; i < sessioncap; i++)
		if((ses = getsession(i)) != NULL)
		if(FD_ISSET(i, &rdset))
			recvinput(i);
	}
}

void sighandle(int sig)
{
	int i;
	session *ses;

	for(i = 0; i < sessioncap; i++)
	if((ses = getsession(i)) != NULL)
		close(ses->sock);
	fprintf(stderr, "Closing server socket.\n");
	close(s);
}

int main(int argc, char *argv[])
{
	static struct sigaction sa = { sighandle, 0, 0 };
	int err;

	s = serverbind();
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	serverlisten(s);

	close(s);
	exit(EXIT_SUCCESS);
}