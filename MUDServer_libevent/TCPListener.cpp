#include "TCPListener.h"
#include "TCPStream.h"

#include <iostream>

using namespace std;

// Constructor. Initializes parent Server pointer,
// Creates new event_base for libevent.
TCPListener::TCPListener(Server* par, int port)
{
	parent = par;
	base = event_base_new();

	if (!base) {
		perror("Error creating event base.");
	}

	this->port = port;


}

// Destructor. Frees the created libevent event base.
TCPListener::~TCPListener()
{
	if (base) {
		cout << "Shutting down event base." << '\n';
		event_base_free(base);
	}
}

// Shutdown. Kills parent server.
void TCPListener::Shutdown() {
	parent->Shutdown();
}

// Accept callback - passes libevent accept event to TCPListener instance.
void TCPListener::do_accept(evutil_socket_t listener, short event, void* arg) {
	(static_cast<TCPListener*>(arg))->accept_cb(listener, event);
}

// Listen. Initializes socket for listener, binds it, and listens on the
// specified port. Adds accept callback event to event base.
void TCPListener::Listen() {
	sockaddr_in sin;
	evutil_socket_t listener;
	event* listener_ev;
	if (!base) {
		cout << "Cannot listen on port " << port << ". No event base created." << '\n';
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);
	cout << "Creating socket" << ".\n";
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener == -1) {
		cout << "Error creating socket." << '\n';
		return;
	}

	cout << "Created socket " << listener << ".\n";

	cout << "Making socket non-blocking." << '\n';
	if (evutil_make_socket_nonblocking(listener) == -1) {
		cout << "Error making socket non-blocking." << '\n';
		ListenerClose(listener);
		return;
	}

	if (bind(listener, (sockaddr*)&sin, sizeof(sin)) < 0) {
		cout << "Error binding socket." << '\n';
		ListenerClose(listener);
		return;
	}

	if (listen(listener, 10) < 0) {
		cout << "Error listening on socket." << '\n';
		ListenerClose(listener);
		return;
	}

	cout << "Socket " << listener << " listening on port " << port << ".\n";

	cout << "Creating new accept event." << '\n';
	listener_ev = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)this);
	if (!listener_ev) {
		cout << "Error creating accept event." << '\n';
		ListenerClose(listener);
	}

	cout << "Adding accept event to event base." << '\n';
	if (event_add(listener_ev, NULL) == -1) {
		cout << "Error adding accept event to event base." << '\n';
		ListenerClose(listener);
	}

	cout << "Starting event base." << '\n';
	if (event_base_dispatch(base) == -1) {
		cout << "Error starting event base." << '\n';
		ListenerClose(listener);
	}
}

// ListenerClose. Utility function for closing sockets.
void TCPListener::ListenerClose(int socketfd) {
#ifdef _WIN32
	closesocket(socketfd);
#else
	close(listener);
#endif
	return;
}

// accept_cb. Callback function for accepting incoming connections.
void TCPListener::accept_cb(evutil_socket_t listener, short event) {
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
		TCPStream* newStream = new TCPStream(this, fd);
		streamSet.insert(newStream);
		bufferevent* bev;
		evutil_make_socket_nonblocking(fd);
		cout << "Socket " << fd << " is nonblocking." << '\n';
		bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
		cout << "Made new buffer event." << '\n';
		bufferevent_setcb(bev, newStream->do_read, NULL, newStream->do_error, (void*)newStream);
		cout << "Set read callback and error callback." << '\n';
		bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
		cout << "Set watermark." << '\n';
		bufferevent_enable(bev, EV_READ | EV_WRITE);
		cout << "Enabled buffer event." << '\n';
	}
}