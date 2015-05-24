#include "../net/net.h"
#include "args.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "session.h"

static int s;
char *port = "12345";

void usage(FILE *out, args_option *opts)
{
	fprintf(out, ""\
		"Usage:\n"\
		"  textworld [ -p PORT_NUMBER ]\n"\
		"\n"\
		"Description:\n"\
		"  Telnet server which presents a virtually infinite, collaborative canvas\n"\
		"  of text. The default port is 12345.\n"\
		"\n"\
		"Options:\n");
	args_usage(opts, out);
	fputc('\n', out);
}

int isvalidport(char *port)
{
	long num;

	num = strtol(port, NULL, 10);
	if(num < 1 || num > 65535)
		return 0;
	return 1;
}

void parseargs(int argc, char *argv[])
{
	args_parse_state st = { 0 };
	static args_option opts[] = {
		{ 'h', 'h', "help", NULL, "Prints help message." },
		{ 301, '-', "version", NULL, "Prints version." },
		{ 'p', 'p', "port", "PORT_NUMBER", "Specifies port number to listen to. By default port 12345." },
		{ 0 },
	};
	int c;
	const char *cmd = args_get_cmd(argv[0]);

	while((c = args_parse(&st, argc, argv, opts)) != -1)
	switch(c) {
	case 'p':
		port = optarg;
		if(!isvalidport(port)) {
			fprintf(stderr, "'%s' is not a valid port number (1-65535).\n", port);
			exit(EXIT_FAILURE);
		}
		break;
	case 301: // version
		printf("%s version %s\n", cmd, PACKAGE_VERSION);
		exit(EXIT_SUCCESS);
	case 'h':
		usage(stdout, opts);
		exit(EXIT_SUCCESS);
	case '?':
		usage(stderr, opts);
		exit(EXIT_FAILURE);
	}

	switch(argc - optind) {
	case 0:
		break;
	default:
		usage(stderr, opts);
		exit(EXIT_FAILURE);
	}
}

int serverbind(void)
{
	int s;

	s = tcplisten4(port);
	if(s == -1) {
		fprintf(stderr, "Failed to bind network socket: %s.\n", strerror(errno));
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
	fd_set rdset;
	session *ses;

	for(;;) {
		FD_ZERO(&rdset);

		nfds = s + 1;
		FD_SET(s, &rdset);

		for(i = 0; i < sessioncap; i++)
		if((ses = getsession(i)) != NULL)
			FD_SET(i, &rdset);

		if(nfds < sessioncap)
			nfds = sessioncap;

		if((n = select(nfds, &rdset, NULL, NULL, NULL)) == -1) {
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

int main(int argc, char *argv[])
{
	parseargs(argc, argv);
	s = serverbind();
	serverlisten(s);

	close(s);
	exit(EXIT_FAILURE);
}
