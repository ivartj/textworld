#ifndef TEXTD_SESSION_H
#define TEXTD_SESSION_H

#include <stdint.h>
#include <stdio.h>

typedef struct session session;

#include "parse.h"
#include "print.h"

struct session {
	int x, y;
	int lx;
	int w, h;
	int sock;
	inputstate is;
	outputstate os;
	char term[257];
};

session *makesession(int sock);


session *getsession(int socket);

int quitsession(int sock);

extern size_t sessioncap;

int inview(session *s, int x, int y);

#endif
