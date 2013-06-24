#ifndef TEXTD_PARSE_H
#define TEXTD_PARSE_H

typedef struct inputstate inputstate;

struct inputstate {
	int state;
	int sb;
	unsigned char sbpar[256];
	int sbparn;
	int sbiac;
};

#include "session.h"

void parse(session *s, unsigned char c);

#endif
