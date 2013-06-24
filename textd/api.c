#include "api.h"
#include "session.h"

char *getview(session *s)
{
	
}

int setview(session *s, int w, int h)
{
	s->w = w;
	s->h = h;
	printf("%d setview(%d, %d)\n", s->sock, w, h);
}

int moveup(session *s)
{
	
}

int movedown(session *s)
{

}

int moveleft(session *s)
{

}

int moveright(session *s)
{

}
