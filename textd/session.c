#include "session.h"
#include <stdio.h>
#include <stdlib.h>

static session **array = NULL;
size_t sessioncap = 0;

session *getsession(int sock)
{
	if(sock >= sessioncap)
		return NULL;

	return array[sock];
}

int quitsession(session *s)
{
	close(s->sock);
	array[s->sock] = NULL;
	free(s);
}

// TODO set view dimension
// TODO set position
session *makesession(int sock)
{
	session *s;

	s = calloc(1, sizeof(session));

	s->sock = sock;
	s->w = 80;
	s->h = 24;

	if(sessioncap <= sock) {
		if(sessioncap == 0)
			sessioncap = 1;
		while(sessioncap <= sock)
			sessioncap <<= 1;
		array = realloc(array, sizeof(session *) * sessioncap);
	}
	printf("%d\n", sock);
	array[sock] = s;

	return s;
}
