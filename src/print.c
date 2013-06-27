#include "../net/net.h"
#include "print.h"
#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "session.h"

static size_t bufsize(session *s);
static void resizebuf(session *s);

static char *buf = NULL;
static size_t bufcap = 0;

size_t bufsize(session *s)
{
	return (s->w + 5) * s->h + (6 + 2 * 4) * 2;
}

void resizebuf(session *s)
{
	size_t n;

	n = bufsize(s);
	if(n > bufcap) {
		bufcap = n;
		buf = realloc(buf, bufcap);
	}
}

void update(session *s, char c, int x, int y)
{
	char *out;

	if(!inview(s, x, y))
		return;
	
	resizebuf(s);
	out = buf;

	x = x - s->x + s->w / 2 + 1;
	y = y - s->y + s->h / 2 + 1;
	printf("(%d, %d)\n", x, y);
	out += sprintf(out, "\x1b[%d;%dH%c", y, x, c);
	out += sprintf(out, "\x1b[%d;%dH", s->h / 2 + 1, s->w / 2 + 1);
	send(s->sock, buf, out - buf, 0);
}

size_t printline(char *out, char *map, int w, int r)
{
	int c;
	int cc;
	size_t inc;
	int sp;

	inc = 0;

	for(c = 0; c < w; c++)
	if(map[r * w + c] != ' ') {
		sp = 0;
		for(cc = c + 1; cc < w; cc++) {
			if(map[r * w + cc] == ' ') {
				sp++;
				if(sp == 5)
					break;
			} else
				sp = 0;
		}
		cc -= sp - 1; // not sure why
		inc += sprintf(out + inc, "\x1b[%d;%dH", r + 1, c + 1);
		memcpy(out + inc, map + r * w + c, cc - c);
		inc += cc - c;
		c = cc + 1;
	}

	return inc;
}

void refresh(session *s)
{
	int c, r;
	char *map;
	char *out;
	size_t inc;

	printf("%d refresh\n", s->sock);

	s->os.notfirst = 1;

	map = getview(s);
	resizebuf(s);
	out = buf;

	out += sprintf(out, "\x1b[1;1H\x1b[0J");

	for(r = 0; r < s->h; r++)
		out += printline(out, map, s->w, r);

	if(s->x >= 0 && s->y >= 0 && s->x < s->w && s->y < s->h);
		out += sprintf(out, "\x1b[%d;%dH", s->h / 2 + 1, s->w / 2 + 1);

	printf("refresh size = %ld\n", out - buf);
	send(s->sock, buf, out - buf, 0);
}

void clearscreen(session *s)
{
	char *out;

	resizebuf(s);
	out = buf;
	out += sprintf(out, "\x1b[1;1H\x1b[0J");
	send(s->sock, buf, out - buf, 0);
}
