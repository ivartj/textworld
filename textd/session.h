#ifndef TEXTD_SESSION_H
#define TEXTD_SESSION_H

#include <stdint.h>
#include <stdio.h>

typedef struct session session;

struct session {
	int x, y;
	int w, h;
	int sock;
};

session *makesession(int sock);


session *getsession(int socket);

int quitsession(session *s);

extern size_t sessioncap;

#endif
