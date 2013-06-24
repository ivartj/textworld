#ifndef TEXTD_SESSION_H
#define TEXTD_SESSION_H

#include <stdint.h>
#include <stdio.h>

typedef struct session session;

#include "parse.h"

struct session {
	int x, y;
	int w, h;
	int sock;
	inputstate is;
};

session *makesession(int sock);


session *getsession(int socket);

int quitsession(int sock);

extern size_t sessioncap;

#endif
