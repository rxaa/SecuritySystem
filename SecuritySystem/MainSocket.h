#pragma once

#include "../../df/IocpSocket.h"

class MainSocket
	: public df::IocpSocket
{
public:
	MainSocket(USHORT port);
	~MainSocket();
};

