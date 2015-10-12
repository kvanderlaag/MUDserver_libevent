#include "TCPStream.h"



TCPStream::TCPStream(TCPListener* par) 
{
	parent = par;
	buffer = "";
}


TCPStream::~TCPStream()
{
}

void TCPStream::do_error(struct bufferevent *bev, short error, void* arg) {
	(static_cast<TCPStream*>(arg))->error_cb(bev, error);
}

void TCPStream::do_read(struct bufferevent *bev, void* arg) {
	(static_cast<TCPStream*>(arg))->read_cb(bev);
}

void TCPStream::read_cb(struct bufferevent *bev, void *ctx) {

}

void TCPStream::write_cb() {

}

void TCPStream::error_cb(struct bufferevent *bev, short error, void *ctx) {
}
