#pragma once
#include <vector>
#include <entityx/Event.h>
#include "events/ChatMessageEvent.h"

class Chat : public entityx::Receiver<Chat> {
public:
	Chat();
	~Chat();

	bool Initialize(entityx::EventManager* pEventManager);
	void Draw(int xPos, int yPos, int width, int height);
	void AddMessage(const char* msg);

	void receive(const ChatMessageReceivedEvent& event);
private:
	entityx::EventManager* m_eventManager = nullptr;

	//Chat
	char                  m_inputBuf[256];
	std::vector<char*>    m_items;
	bool                  m_autoScroll = true;
	bool                  m_scrollToBottom = false;
};