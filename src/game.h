#ifndef TEXTD_GAME_H
#define TEXTD_GAME_H

#include "session.h"

char *getregion(int x, int y, int w, int h);
void add(char c, int x, int y);
void adds(session *s, char c, int x, int y);

#endif
