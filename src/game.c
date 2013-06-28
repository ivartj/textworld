#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "session.h"

typedef struct binar binar;

struct binar {
	char c;
	int x, y;
	binar *left;
	binar *right;
	int depth;
};

static void resizebuf(int w, int h);

static char *buf = NULL;
static size_t bufcap = 0;

binar *t = NULL;


void resizebuf(int w, int h)
{
	size_t n;

	n = w * h;
	if(n > bufcap) {
		bufcap = n;
		buf = realloc(buf, bufcap);
	}
}

binar *makebinar(char c, int x, int y)
{
	binar *t;

	t = calloc(1, sizeof(binar));
	t->c = c;
	t->x = x;
	t->y = y;
	t->depth = 0;

	return t;
}

int compare(binar *t, int x, int y)
{
	int val;

	val = (y > t->y) - (y < t->y);
	if(val != 0)
		return val;
	val = (x > t->x) - (x < t->x);
	return val;
}

int depth(binar *t)
{
	int d;

	d = 0;

	if(t->left != NULL)
	if(t->left->depth + 1 > d)
		d = t->left->depth + 1;

	if(t->right != NULL)
	if(t->right->depth + 1 > d)
		d = t->right->depth + 1;

	return d;
}

void rightrotate(binar **tp)
{
	binar *t, *tmp;

	t = *tp;
	tmp = t->left;
	t->left = tmp->right;
	tmp->right = t;
	*tp = tmp;
	t->depth = depth(t);
}

void leftrotate(binar **tp)
{
	binar *t, *tmp;

	t = *tp;
	tmp = t->right;
	t->right = tmp->left;
	tmp->left = t;
	*tp = tmp;
	t->depth = depth(t);
}

void balance(binar **tp)
{
	int ldep, rdep;	
	binar *t;

	t = *tp;
	ldep = rdep = 0;
	if(t->left != NULL)
		ldep = t->left->depth;
	if(t->right != NULL)
		rdep = t->right->depth;

	if(ldep > rdep + 1) {
		rightrotate(tp);
		return;
	}

	if(rdep > ldep + 1) {
		leftrotate(tp);
		return;
	}
}

void addchild(binar **tp, char c, int x, int y)
{
	binar *t;

	if(*tp == NULL) {
		*tp = makebinar(c, x, y);
		return;
	}

	t = *tp;

	switch(compare(t, x, y)) {
	case 0:
		t->c = c;
		break;
	case 1:
		addchild(&t->right, c, x, y);
		balance(tp);
		break;
	case -1:
		addchild(&t->left, c, x, y);
		balance(tp);
		break;
	}
	(*tp)->depth = depth(*tp);
}

void add(char c, int x, int y)
{
	int i;
	session *s;

	addchild(&t, c, x, y);

	for(i = 0; i < sessioncap; i++)
	if((s = getsession(i)) != NULL)
		update(s, c, x, y);
}

void adds(session *e, char c, int x, int y)
{
	int i;
	session *s;

	if(t == NULL) {
		t = makebinar(c, x, y);
		return;
	}

	addchild(&t, c, x, y);

	for(i = 0; i < sessioncap; i++)
	if((s = getsession(i)) != NULL)
	if(s != e)
		update(s, c, x, y);

}

int count;

char getchild(binar *t, int x, int y)
{
	count++;
	if(t == NULL)
		return ' ';

	switch(compare(t, x, y)) {
	case 0:
		return t->c;
	case 1:
		return getchild(t->right, x, y);
	case -1:
		return getchild(t->left, x, y);
	}
}

char get(int x, int y)
{
	char c;

	count = 0;
	if(t == NULL)
		return ' ';
	c = getchild(t, x, y);

	printf("count = %d\n", count);
	return c;
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

