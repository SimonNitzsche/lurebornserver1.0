#include "TimedEvent.h"

///THIS CLASS IS BROKEN :{
/*
void TimedEvents::AddEvent(TimedEvent e) {
	events.push_back(e);
}

void TimedEvents::Create(int when, void* handler) {
	Create(when, handler, emptySubData());
}

void TimedEvents::Create(int when, void* handler, TimedEvents::TE_SUBDATA extra){
	TimedEvent s;
	s.etimestamp = when;
	s.handler = handler;
	s.extra = extra;
	AddEvent(s);
}

void TimedEvents::check(int currentTime) {
	if (events.size() > 0) {
		std::vector<TimedEvent> redone;
		for (int i = 0; i < events.size(); i++) {
			if (events.at(i).etimestamp <= currentTime) {
				(void*)events.at(i).handler;
			}
			else {
				redone.push_back(events.at(i));
			}
		}
		//This removes the executed one;
		events = redone;
	}
}

TimedEvents::TE_SUBDATA TimedEvents::emptySubData() {
	TE_SUBDATA v;
	return v;
}*/