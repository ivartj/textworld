#include "parse.h"
#include <stdint.h>
#include <arpa/inet.h>
#include "string.h"
#include "api.h"


#define IAC	255

#define SE	240
#define SB	250
#define SBPAR	-1

#define WILL	251
#define WONT	252
#define DO	253
#define DONT	254

#define NAWS	31

void parse(session *s, unsigned char c)
{
	uint16_t w, h;

	switch(s->is.state) {
	case 0:
		switch(c) {
		case IAC:
			s->is.state = IAC;
			break;
		default:
			if(isgraph(c))
				printf("%d %c\n", s->sock, c);
		}
		break;
	case IAC:
		printf("%d IAC %hhu\n", s->sock, c);
		switch(c) {
		case WILL:
			s->is.state = WILL;
			break;
		case WONT:
			s->is.state = WONT;
			break;
		case DO:
			s->is.state = DO;
			break;
		case DONT:
			s->is.state = DONT;
			break;
		case SB:
			s->is.state = SB;
			break;
		}
		break;
	case SB:
		s->is.state = SBPAR;
		memset(s->is.sbpar, 0, 256);
		s->is.sbparn = 0;
		s->is.sbiac = 0;

		switch(c) {
		case NAWS:
			s->is.sb = NAWS;
		}
		break;
	case SBPAR:
		if(s->is.sbparn == 256) {
			s->is.state = 0;
			break;
		}
		s->is.sbpar[s->is.sbparn++] = c;

		switch(s->is.sbiac) {
		case 0:
			s->is.sbiac = c == IAC;
			break;
		case 1:
			if(c == SE)
				s->is.state = 0;
			else
				s->is.sbiac = 0;
		}

		switch(s->is.sb) {
		case NAWS:
			if(s->is.sbparn == 4) {
				w = ntohs(*(uint16_t *)s->is.sbpar);
				h = ntohs(*(uint16_t *)(s->is.sbpar + 2));
				setview(s, w, h);
				s->is.sb = 0;
			}
		}
		break;
	case WONT:
		printf("%d WONT %hhu\n", s->sock, c);
		s->is.state = 0;
		break;
	case WILL:
		printf("%d WILL %hhu\n", s->sock, c);
		s->is.state = 0;
		break;
	default:
		s->is.state = 0;
		break;
	}
}
