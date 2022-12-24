#pragma once
#include "TCP-Layer/Network-Module/NetworkModule.hpp"
#include "entityx/Event.h"
#include "../events/ChatMessageEvent.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>

constexpr unsigned int NETWORK_MAX_PLAYER_NAME_LENGTH = 31;
constexpr unsigned int MAX_CHAT_MESSAGE_LENGTH = 62;

struct ConnectionInfo {
	TCP_CONNECTION_ID TCP_ID;
	std::chrono::system_clock::time_point time_joined;
};

struct ConnectedPlayer {
	TCP_CONNECTION_ID clientID;
	std::string name;
};

struct Player {
	unsigned int playerID;
	TCP_CONNECTION_ID clientID;
	std::string playerName;
};

enum NETWORK_GAMELAYER_EVENT_TYPE : char {
	UNDEFINED = 0,
	PLAYER_JOINED,
	PLAYER_LEFT,
	PLAYER_CHANGED_NAME,
	CHAT_MESSAGE_RECEIVED,
	REQUEST_STATE_CHANGE,
	GAME_DATA_MESSAGE
};

struct NETWORK_GAMELAYER_EVENT {
	NETWORK_GAMELAYER_EVENT_TYPE type;

	union {
		struct {
			char playerID;
			char name[NETWORK_MAX_PLAYER_NAME_LENGTH];
		} player_joined;
		struct {
			char playerID;
		} player_left;
		struct {
			char senderID;
			char message[MAX_CHAT_MESSAGE_LENGTH];
		} chat_message_received;
		struct {
			char stateID;
		} request_state_change;
		struct {
			char senderID;
			char name[NETWORK_MAX_PLAYER_NAME_LENGTH];
		} player_name_change;
	} event_data;
};

class NetworkConnection : public NetworkEventHandler, public entityx::Receiver<NetworkConnection> {
public:

	NetworkConnection();
	~NetworkConnection();

	bool Initialize(entityx::EventManager* pEventManager);
	void CheckNetworkEvents();

	bool Host();
	bool Join();

	std::vector<Player> m_players;

	void receive(const ChatMessageSentEvent& event);

protected:
	std::vector<TCP_CONNECTION_ID> m_pending_connection; //Players that just joined (active TCP connection) but have not been accepted by the game.
	Network m_networkModule;
	entityx::EventManager* m_pEventManager = nullptr;


	// Inherited via NetworkEventHandler
	virtual void handleNetworkEvents(NetworkEvent event) override;
	virtual void handleGameLayerNetworkEvents(NETWORK_GAMELAYER_EVENT& event, TCP_CONNECTION_ID tcp_connection_id);
};