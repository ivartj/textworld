#ifndef NET_H
#define NET_H

#ifndef WIN32

#include <sys/socket.h>

#else

#define WIN32_LEAN_AND_MEAN

#ifndef WINVER
#define WINVER 0x501
#endif

#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>

#endif

int tcplisten(const char *port);
int tcplisten4(const char *port);
int tcplisten6(const char *port);
int tcpdial(const char *node, const char *port);

#endif
