#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else 
#include <sys/socket.h>
#endif

#include <sys/types.h>


#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "TCPStream.h"

#include <iostream>
#include <set>

#define MAX_LINE 16284

class TCPStream;

class TCPListener
{
public:
	TCPListener(int port=9500);
	~TCPListener();
	void Listen();
	void accept_cb(evutil_socket_t listener, short event, void* arg);
	static void do_accept(evutil_socket_t listener, short event, void* arg);

private:
	std::set<TCPStream*> streamSet;
	event_base* base;
	int port;


};

