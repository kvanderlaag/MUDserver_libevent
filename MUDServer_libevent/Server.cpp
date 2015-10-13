#include "Server.h"
#include "TCPListener.h"
#include <map>
#include <iostream>

using namespace std;


Server::Server(int port)
{
	cout << "Creating new Server on port " << port << ".\n";
	cout << "Creating new TCPListener on port " << port << ".\n";
	listener = new TCPListener(this);
	cout << "TCPListener creation successful." << '\n';
}


Server::~Server()
{
	if (listener) {
		delete listener;
	}
}

void Server::Start() {
	listener->Listen();
}

void Server::Shutdown() {
	delete listener;
	listener = nullptr;
}