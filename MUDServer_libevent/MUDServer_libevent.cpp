// MUDServer_libevent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/util.h"
#include "event2/bufferevent.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <sys/types.h>

#include <iostream>

using namespace std;

#define MAX_LINE 16384

void killserver(event_base* base, int seconds) {
	timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = seconds;
	event_base_loopbreak(base);
}

void readcb(struct bufferevent *bev, void *ctx) {
	evbuffer *input, *output;
	char* line;
	size_t n;
	input = bufferevent_get_input(bev);
	output = bufferevent_get_output(bev);

	line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF);

	evbuffer_add(output, line, n);
}

void errorcb(struct bufferevent *bev, short error, void *ctx) {
	if (error & BEV_EVENT_EOF) {
		cout << "Connection closed.";
		closesocket(bufferevent_getfd(bev));
		bufferevent_free(bev);
	}
}

void do_accept(evutil_socket_t listener, short event, void* arg) {
	event_base* base = (event_base*) arg;
	sockaddr_storage ss;
	socklen_t slen = sizeof(ss);

	int fd = accept(listener, (sockaddr*)&ss, &slen);
	cout << "Accepted connection." << '\n';
	if (fd < 0) {
		perror("accept");
	}
	//else if (fd > FD_SETSIZE) {
	//	closesocket(fd);
	//}
	else {
		bufferevent* bev;
		evutil_make_socket_nonblocking(fd);
		cout << "Socket " << fd << " is nonblocking." << '\n';
		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		cout << "Made new buffer event." << '\n';
		bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
		cout << "Set read callback and error callback." << '\n';
		bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
		cout << "Set watermark." << '\n';
		bufferevent_enable(bev, EV_READ | EV_WRITE);
		cout << "Enabled buffer event." << '\n';
	}
}

void run() {

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		cout << "Shitty WSAStartup. Exiting.";
		return;
	}
	event_base* serverEventBase = event_base_new();
	sockaddr_in sin;
	evutil_socket_t listener;
	event* listener_ev;
	if (!serverEventBase) {
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(9500);
	listener = socket(AF_INET, SOCK_STREAM, 0);
	evutil_make_socket_nonblocking(listener);

	if (bind(listener, (sockaddr*)&sin, sizeof(sin)) < 0) {
		perror("bind");
		return;
	}

	if (listen(listener, 10) < 0) {
		perror("listen");
		return;
	}

	listener_ev = event_new(serverEventBase, listener, EV_READ | EV_PERSIST, do_accept, (void*)serverEventBase);

	event_add(listener_ev, NULL);
	event_base_dispatch(serverEventBase);

	free(serverEventBase);

}

int main()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	
	run();

    return 0;
}

