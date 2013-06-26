#ifndef TEXTD_API_H
#define TEXTD_API_H

#include "session.h"

char *getview(session *s);
int setview(session *s, int w, int h);
int setterm(session *s, char *termtype);

int moveup(session *s);
int movedown(session *s);
int moveleft(session *s);
int moveright(session *s);

int put(session *s, char c);
int back(session *s);

#endif
