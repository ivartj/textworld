#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "session.h"

typedef struct quad quad;

struct quad {
	char c;
	int x, y;
	quad *ne, *nw, *sw, *se;
};

static void resizebuf(int w, int h);

static char *buf = NULL;
static size_t bufcap = 0;

quad *t = NULL;


void resizebuf(int w, int h)
{
	size_t n;

	n = w * h;
	if(n > bufcap) {
		bufcap = n;
		buf = realloc(buf, bufcap);
	}
}

quad *makequad(char c, int x, int y)
{
	quad *t;

	t = calloc(1, sizeof(quad));
	t->c = c;
	t->x = x;
	t->y = y;

	return t;
}

void addchild(quad *t, char c, int x, int y)
{
	if(t->x == x && t->y == y) {
		t->c = c;
		return;
	}

	switch(y < t->y) {
	case 1: // N
		switch(x < t->x) {
		case 1: // W
			if(t->nw == NULL)
				t->nw = makequad(c, x, y);
			else
				addchild(t->nw, c, x, y);
			break;
		case 0: // E
			if(t->ne == NULL)
				t->ne = makequad(c, x, y);
			else
				addchild(t->ne, c, x, y);
			break;
		}
		break;
	case 0: // S
		switch(x < t->x) { // WEST
		case 1: // W
			if(t->sw == NULL)
				t->sw = makequad(c, x, y);
			else
				addchild(t->sw, c, x, y);
			break;
		case 0: // E
			if(t->se == NULL)
				t->se = makequad(c, x, y);
			else
				addchild(t->se, c, x, y);
			break;
		}
		break;
	}
}

void add(char c, int x, int y)
{
	int i;
	session *s;

	if(t == NULL) {
		t = makequad(c, x, y);
		return;
	}

	addchild(t, c, x, y);

	for(i = 0; i < sessioncap; i++)
	if((s = getsession(i)) != NULL)
		update(s, c, x, y);
}

void adds(session *e, char c, int x, int y)
{
	int i;
	session *s;

	if(t == NULL) {
		t = makequad(c, x, y);
		return;
	}

	addchild(t, c, x, y);

	for(i = 0; i < sessioncap; i++)
	if((s = getsession(i)) != NULL)
	if(s != e)
		update(s, c, x, y);

}

char getchild(quad *t, int x, int y)
{
	if(t->x == x && t->y == y)
		return t->c;
	switch(y < t->y) {
	case 1: // N
		switch(x < t->x) {
		case 1: // W
			if(t->nw == NULL)
				return ' ';
			else
				return getchild(t->nw, x, y);
			break;
		case 0: // E
			if(t->ne == NULL)
				return ' ';
			else
				return getchild(t->ne, x, y);
			break;
		}
		break;
	case 0: // S
		switch(x < t->x) { // WEST
		case 1: // W
			if(t->sw == NULL)
				return ' ';
			else
				return getchild(t->sw, x, y);
			break;
		case 0: // E
			if(t->se == NULL)
				return ' ';
			else
				return getchild(t->se, x, y);
			break;
		}
		break;
	}
}

char get(int x, int y)
{
	if(t == NULL)
		return ' ';
	return getchild(t, x, y);
}

char *getregion(int x, int y, int w, int h)
{
	int xx, yy;

	resizebuf(w, h);

	for(yy = 0; yy < h; yy++)
	for(xx = 0; xx < w; xx++)
		buf[yy * w + xx] = get(x + xx - w / 2, y + yy - h / 2);

	return buf;
}

