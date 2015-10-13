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
#include "Server.h"

#include <iostream>
#include <set>

#define MAX_LINE 16384

class TCPStream;

class TCPListener
{
public:
	TCPListener(Server* par, int port=9500);
	~TCPListener();
	void Listen();
	void Shutdown();
	void accept_cb(evutil_socket_t listener, short event);
	void ListenerClose(int socketfd);
	static void do_accept(evutil_socket_t listener, short event, void* arg);

private:
	Server* parent;
	std::set<TCPStream*> streamSet;
	event_base* base;
	int port;


};

