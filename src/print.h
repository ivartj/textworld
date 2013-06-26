#ifndef TEXTD_PRINT_H
#define TEXTD_PRINT_H

typedef struct outputstate outputstate;

struct outputstate {
	int notfirst;
	int w, h;
	char *text;
};

#include "session.h"

void blit(session *s);
void refresh(session *s);

#endif
