#pragma once
#include <stdlib.h>
#include <vector>
#include "WindowsAPI.h"
/*
class TimedEvent {
public:
	int etimestamp;
	void* handler;
	TimedEvents::TE_SUBDATA extra;
};

///<summary>
///THIS IS USED TO DO SOMETHING AT
///A SPECIFIC TIME
///</summary>
class TimedEvents
{
private:
	std::vector<int> reactTime; //A list full of timestamps, wich define to check all events
public:
	struct TE_SUBDATA {
		std::string s1;
		long long n1;
		std::string s2;
		long long n2;
		std::string s3;
		long long n3;
	};
	//public delegate void HANDLER();
	
	void AddEvent(TimedEvent e);
	void Create(int when, void* handler);
	void Create(int when, void* handler, TimedEvents::TE_SUBDATA extra);
	void check(int currentTime);
	TE_SUBDATA emptySubData();
private:
	std::vector<TimedEvent> TimedEvents::events;
};
*/
