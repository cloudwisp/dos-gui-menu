#ifndef AppEvent_CPP
#define AppEvent_CPP

#include "AppEvent.h"
#include <queue>

struct EventDataStr {
	int data1;
	int data2;
};

struct EventHandlerStr {
	std::string event;
	EventConsumer *consumer;
	void (*callback)(EventEmitter *source, EventData data);
};

struct Event {
	std::string event;
	EventEmitter* source;
	EventConsumer* consumer;
	EventData data;
};

class EventQueue {
public:
	static std::queue<Event> Pending;
	static void AddEvent(Event event);
	static void EmitAllQueued();
};


class EventConsumer {
public:
	virtual void OnEvent(EventEmitter *source, std::string event, EventData data) {};
};

class EventEmitter {
private:
	EventHandler _handlers[255];
	int _handlerCount;

	bool AlreadyBound(std::string event, EventConsumer *consumer){
		for (int i = 0; i < _handlerCount; i++){
			if (_handlers[i].consumer == consumer && _handlers[i].event == event){
				return true;
			}
		}
		return false;
	}

public:
    EventEmitter() {
        _handlerCount = 0;
    }
	EventData CreateEventData(int data1, int data2){
		EventData d = {data1,data2};
		return d;
	}

	void UnbindAllEventsForConsumer(EventConsumer* consumer){
        for (int i = _handlerCount-1; i >= 0; i--){
            if (_handlers[i].consumer && _handlers[i].consumer == consumer){
                //remove, shift all elements ahead, back one position
                for (int x = i+1; x < _handlerCount; x++){
                    _handlers[x-1] = _handlers[x];
                }
                _handlerCount--;
            }
        }
	}

	void BindEvent(std::string event, void (*handler)(EventEmitter *source, EventData data)){
		EventHandler handlerDef;
		handlerDef.event = event;
		handlerDef.callback = handler;
		handlerDef.consumer = NULL;
		_handlers[_handlerCount] = handlerDef;
		_handlerCount++;
	}

	void BindEvent(std::string event, EventConsumer *consumer){
		if (AlreadyBound(event, consumer)){
			return;
		}
		EventHandler handlerDef;
		handlerDef.consumer = consumer;
		handlerDef.event = event;
		_handlers[_handlerCount] = handlerDef;
		_handlerCount++;
	}

	void EmitEvent(std::string event, int data1, int data2){
		EmitEvent(event, CreateEventData(data1, data2));
	}

	void EmitEvent(std::string event, int data){
		EmitEvent(event, CreateEventData(data, 0));
	}

	void EmitEvent(std::string event){
		EmitEvent(event, CreateEventData(0,0));
	}

	void EmitEvent(std::string event, EventData data){
		int i;
		for (i = 0; i < _handlerCount; i++){
			if (_handlers[i].event == event){
				if (_handlers[i].consumer == NULL){
					_handlers[i].callback(this, data);
				} else {
					Event evt;
					evt.consumer = _handlers[i].consumer;
					evt.source = this;
					evt.event = event;
					evt.data = data;

					EventQueue::AddEvent(evt);
				}
			}
		}
	}

};

std::queue<Event> EventQueue::Pending;
void EventQueue::AddEvent(Event event){
	Pending.push(event);
};
void EventQueue::EmitAllQueued(){
	while(Pending.size() > 0){
		Event evt = Pending.front();
		Pending.pop();
		evt.consumer->OnEvent(evt.source, evt.event, evt.data);
	}
};


#endif
