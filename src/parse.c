#include "parse.h"
#include <stdint.h>
#include "string.h"
#include "api.h"
#include "print.h"

static void parsetelnet(session *s, unsigned char c);
static void parseansi(session *s, unsigned char c);
static uint16_t int_ntohs(uint16_t num);

#define IAC	255

#define SE	240
#define SB	250
#define SBPAR	-1

#define WILL	251
#define WONT	252
#define DO	253
#define DONT	254

#define NAWS	31
#define TERM	24


#define CSI1	27
#define CSI2	91


#define MOVING	0
#define WRITING	1

void parse(session *s, unsigned char c)
{
	parsetelnet(s, c);
}

void parseascii(session *s, unsigned char c)
{
	switch(c) {
	case 0x08: // BACKSPACE
	case 0x7f: // DELETE
		back(s);
		break;
	case 0x0d: // RETURN
		newline(s);
		break;
	default:
		if(isgraph(c) || c == ' ')
			put(s, c);
		break;
	}
}

void parseansi(session *s, unsigned char c)
{
	printf("%d %hhu\n", s->sock, c);
	switch(s->is.ansistate) {
	case 0:
		switch(c) {
		case CSI1:
			s->is.ansistate = CSI1;
			break;
		default:
			parseascii(s, c);
			break;
		}
		break;
	case CSI1:
		switch(c) {
		case CSI1: // ESC ESC
			clearscreen(s);
			quitsession(s->sock);
			break;
		case CSI2:
			s->is.ansistate = CSI2;
			s->is.ansiopt = 0;
			break;
		}
		break;
	case CSI2:
		switch(c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			s->is.ansiopt *= 10;
			s->is.ansiopt += c - '0';
			break;
		case 'A':
			s->is.ansistate = 0;
			moveup(s);
			break;
		case 'B':
			s->is.ansistate = 0;
			movedown(s);
			break;
		case 'C':
			s->is.ansistate = 0;
			moveright(s);
			break;
		case 'D':
			s->is.ansistate = 0;
			moveleft(s);
			break;
		default:
			s->is.ansistate = 0;
		}
	}
}

void parsetelnet(session *s, unsigned char c)
{
	uint16_t w, h;

	switch(s->is.state) {
	case 0:
		switch(c) {
		case IAC:
			s->is.state = IAC;
			break;
		default:
			parseansi(s, c);
			break;
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
			break;
		case TERM:
			s->is.sb = TERM;
			break;
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
			if(c == SE) {
				switch(s->is.sb) {
				case TERM:
					if(s->is.sbparn - 3 > 0) {
						s->is.sbpar[s->is.sbparn - 2] = '\0';
						setterm(s, s->is.sbpar + 1);
					}
					break;
				}
				s->is.state = 0;
			} else
				s->is.sbiac = 0;
			break;
		}

		switch(s->is.sb) {
		case NAWS:
			if(s->is.sbparn == 4) {
				w = int_ntohs(*(uint16_t *)s->is.sbpar);
				h = int_ntohs(*(uint16_t *)(s->is.sbpar + 2));
				setview(s, w, h);
				s->is.sb = 0;
			}
			break;
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

uint16_t int_ntohs(uint16_t num)
{
	uint16_t val;
	int i;
	unsigned char *b;

	b = (unsigned char *)&val;

	for(i = 0; i < sizeof(num); i++)
		b[sizeof(num) - i - 1] = (num >> 8 * i) & 0xFF;

	return val;
}
