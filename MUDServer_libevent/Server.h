#pragma once

#include "TCPListener.h"

class Server
{
public:
	Server();
	~Server();

private:
	TCPListener* listener;


};

