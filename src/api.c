#include "api.h"
#include "session.h"
#include "game.h"
#include <string.h>

char *getview(session *s)
{
	return getregion(s->x, s->y, s->w, s->h);
}

int setview(session *s, int w, int h)
{
	s->w = w;
	s->h = h;
	printf("%d setview(%d, %d)\n", s->sock, w, h);
	refresh(s);
}

int setterm(session *s, char *termtype)
{
	strcpy(s->term, termtype);
	printf("%d setterm(%s)\n", s->sock, termtype);
}

int moveup(session *s)
{
	s->y--;
	s->lx = s->x;
	refresh(s);
	printf("%d moveup\n", s->sock);
}

int movedown(session *s)
{
	s->y++;
	s->lx = s->x;
	refresh(s);
	printf("%d movedown\n", s->sock);
}

int moveleft(session *s)
{
	s->x--;
	s->lx = s->x;
	refresh(s);
	printf("%d moveleft\n", s->sock);
}

int moveright(session *s)
{
	s->x++;
	s->lx = s->x;
	refresh(s);
	printf("%d moveright\n", s->sock);
}

int newline(session *s)
{
	s->x = s->lx;
	s->y++;
	refresh(s);
	printf("%d newline\n", s->sock);
}

int put(session *s, char c)
{
	s->x++;
	adds(s, c, s->x - 1, s->y);
	refresh(s);
}

int back(session *s)
{
	s->x--;
	if(s->x < s->lx)
		s->lx = s->x;
	adds(s, ' ', s->x, s->y);
	refresh(s);
}
