#include "../config.h"
#include "net.h"

#ifndef WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#else

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#endif

static int tcplistenfamily(const char *port, int ai_family);

int tcplistenfamily(const char *port, int ai_family)
{
	int err;
	int s;
	int true = 1;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *iter = NULL;

	#ifdef WIN32
	err = net_win32_winsock_initialize();
	if(err)
		return -1;
	#endif

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = ai_family;
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(NULL, port, &hints, &res);
	if(err)
		return -1;

	if(res == NULL)
		return -1;

	for(iter = res; iter != NULL; iter = iter->ai_next) {
		s = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if(s == -1)
			continue;

		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true));
		
		err = bind(s, iter->ai_addr, iter->ai_addrlen);
		if(err) {
			close(s);
			continue;
		}
		break;
	}

	if(iter == NULL) {
		freeaddrinfo(res);
		return -1;
	}

	err = listen(s, 30);
	if(err) {
		close(s);
		freeaddrinfo(res);
		return -1;
	}

	return s;
}

int tcplisten(const char *port)
{
	return tcplistenfamily(port, AF_UNSPEC);
}

int tcplisten4(const char *port)
{
	return tcplistenfamily(port, AF_INET);
}

int tcplisten6(const char *port)
{
	return tcplistenfamily(port, AF_INET6);
}

int tcpdial(const char *node, const char *port)
{
	int err;
	int s;
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *iter = NULL;

	#ifdef WIN32
	err = net_win32_winsock_initialize();
	if(err)
		return -1;
	#endif

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(node, port, &hints, &res);
	if(err)
		return -1;

	for(iter = res; iter != NULL; iter = iter->ai_next) {
		s = socket(iter->ai_family, SOCK_STREAM, 0);
		if(s == -1)
			continue;
		err = connect(s, iter->ai_addr, iter->ai_addrlen);
		if(err) {
			close(s);
			continue;
		}
		break;
	}

	if(iter == NULL) {
		freeaddrinfo(res);
		return -1;
	}

	return s;
}
