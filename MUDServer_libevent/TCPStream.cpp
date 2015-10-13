#include "TCPStream.h"

#include <iostream>



TCPStream::TCPStream(TCPListener* par, int socket) 
{
	parent = par;
	buffer = "";
	socketfd = socket;
}


TCPStream::~TCPStream()
{
#ifdef _WIN32
	closesocket(socketfd);
#else
	close(socketfd);
#endif
	cout << "Closing socket " << socketfd << '\n';
}

void TCPStream::do_error(struct bufferevent *bev, short error, void* arg) {
	(static_cast<TCPStream*>(arg))->error_cb(bev, error);
}

void TCPStream::do_read(struct bufferevent *bev, void* arg) {
	(static_cast<TCPStream*>(arg))->read_cb(bev);
}

void TCPStream::read_cb(struct bufferevent *bev) {
	evbuffer *input, *output;
	char* line;
	size_t n;
	input = bufferevent_get_input(bev);
	output = bufferevent_get_output(bev);

	line = evbuffer_readln(input, &n, EVBUFFER_EOL_CRLF);
	if (line) {
		//evbuffer_add(output, line, n);
		//const char* nl = "\n\r";
		//evbuffer_add(output, nl, 1);
		for (int i = 0; i < n; ++i) {
			buffer += line[i];
		}

		// This is where you would send the buffer back to the server/game engine
		// for processing, but instead for now, we're just going to echo it to the console.
		if (buffer.compare("") != 0) {
			cout << "Socket " << socketfd << ": " << buffer << '\n';
			if (buffer.compare("quit") == 0) {
				error_cb(bev, BEV_EVENT_EOF);
				delete this;
			}
			else if (buffer.compare("shutdown") == 0) {
				parent->Shutdown();
			}
			else {
				buffer = "";
			}
		}
	}
}

void TCPStream::write_cb() {

}

void TCPStream::error_cb(struct bufferevent *bev, short error) {
	if (error & BEV_EVENT_EOF) {
		cout << "Connection closed." << '\n';
#ifdef _WIN32
		closesocket(bufferevent_getfd(bev));
#else
		close(bufferevent_getfd(bev));
#endif
		bufferevent_free(bev);
	}
}
