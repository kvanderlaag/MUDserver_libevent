#pragma once

#include "TCPListener.h"
#include <map>

using namespace std;

class TCPListener;
class TCPStream;

class Server
{
public:
	Server(int port=9500);
	~Server();

	void Start();
	void Shutdown();

private:
	TCPListener* listener;
	map<int, TCPStream*> connections;


};

