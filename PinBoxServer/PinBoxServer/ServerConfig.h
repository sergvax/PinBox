#pragma once
#ifndef _PP_SERVER_CONFIG_H__
#define _PP_SERVER_CONFIG_H__
#include <libconfig.h++>
#include <iostream>
#include "HubItem.h"
#include <vector>

class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();

	static ServerConfig*		Get();

	int							MonitorIndex = 1;
	int							CaptureFPS = 30;
	int							NetworkThread = 2;
	int							ServerPort = 1234;
	void						LoadConfig();

	std::vector<HubItem*>		HubItems;
	void						LoadHubItems();
};

#endif