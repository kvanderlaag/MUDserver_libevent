#pragma once

#include "TCPListener.h"
#include "Server.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

#include <string>

using namespace std;

class TCPListener;
class Server;

class TCPStream
{
public:
	TCPStream(TCPListener* par, int socket);
	~TCPStream();

	void read_cb(struct bufferevent *bev);
	void write_cb();
	void error_cb(struct bufferevent *bev, short error);

	static void do_error(struct bufferevent *bev, short error, void* arg);

	static void do_read(struct bufferevent *bev, void* arg);

private:
	TCPListener* parent;
	int socketfd;
	string buffer;

};

