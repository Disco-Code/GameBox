#include "NetworkConnection.h"
#include "../../events/Networking/NetworkEvents.h"
#include <iostream>

NetworkConnection::NetworkConnection() {

}

NetworkConnection::~NetworkConnection() {
	if (m_pEventManager) {
		m_pEventManager->unsubscribe<ChatMessageSentEvent>(*this);
	}
}

bool NetworkConnection::Initialize(entityx::EventManager* pEventManager) {
	m_pEventManager = pEventManager;
	m_pEventManager->subscribe<ChatMessageSentEvent>(*this);

	return m_networkModule.initialize();
}

void NetworkConnection::CheckNetworkEvents() {
	m_networkModule.checkForPackages(*this);	//Will call handleNetworkEvents(NetworkEvent nEvents) for each network event not yet handled.
}

bool NetworkConnection::Host() {
	bool b = m_networkModule.host(1234);
	if (b) {
		Player player;
		player.clientID = 0;
		player.playerID = 0;
		player.playerName = "Me (Host)" + std::to_string(player.clientID);

		m_players.push_back(player);
	}
	return b;
}

bool NetworkConnection::Join() {
	bool b = m_networkModule.join("127.0.0.1", 1234);
	return b;
}

void NetworkConnection::receive(const ChatMessageSentEvent& event) {
	NETWORK_GAMELAYER_EVENT networkEvent = {};
	networkEvent.type = NETWORK_GAMELAYER_EVENT_TYPE::CHAT_MESSAGE_RECEIVED;
	int len = std::strlen(event.message);
	if (len >= MAX_CHAT_MESSAGE_LENGTH) {
		len = MAX_CHAT_MESSAGE_LENGTH - 1;
	}
	memcpy_s(networkEvent.event_data.chat_message_received.message, len, event.message, len);
	networkEvent.event_data.chat_message_received.senderID = 0;

	if (m_networkModule.isServer()) {
		m_networkModule.send(reinterpret_cast<char*>(&networkEvent), sizeof(networkEvent), -1);

		ChatMessageReceivedEvent chatMessage;
		chatMessage.message = networkEvent.event_data.chat_message_received.message;
		m_pEventManager->emit(chatMessage);
	} else {
		m_networkModule.send(reinterpret_cast<char*>(&networkEvent), sizeof(networkEvent));
	}
}

void NetworkConnection::handleNetworkEvents(NetworkEvent event) {

	switch (event.eventType) {
	case NETWORK_EVENT_TYPE::NETWORK_ERROR:
	{
		break;
	}
	case NETWORK_EVENT_TYPE::CONNECTION_ESTABLISHED:
	{
		if (m_networkModule.isServer()) {
			m_pending_connection.push_back(event.clientID);
			//Send connected players
			char id = 0;
			for (auto& i : m_players) {
				NETWORK_GAMELAYER_EVENT playerJoined = {};
				playerJoined.type = NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_JOINED;

				memcpy_s(playerJoined.event_data.player_joined.name, NETWORK_MAX_PLAYER_NAME_LENGTH, i.playerName.c_str(), i.playerName.size());
				playerJoined.event_data.player_joined.playerID = id++;

				m_networkModule.send(reinterpret_cast<char*>(&playerJoined), sizeof(NETWORK_GAMELAYER_EVENT), event.clientID);
			}
		} else {
			NETWORK_GAMELAYER_EVENT playerJoined = {};
			playerJoined.type = NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_JOINED;

			memcpy_s(playerJoined.event_data.player_joined.name, NETWORK_MAX_PLAYER_NAME_LENGTH, "Client", 6);
			playerJoined.event_data.player_joined.playerID = 0;

			m_networkModule.send(reinterpret_cast<char*>(&playerJoined), sizeof(NETWORK_GAMELAYER_EVENT));
		}
		break;
	}
	case NETWORK_EVENT_TYPE::CONNECTION_CLOSED:
	{
		//TODO: exit
		break;
	}
	case NETWORK_EVENT_TYPE::CONNECTION_RE_ESTABLISHED:
	{
		//TODO: anything?
		break;
	}
	case NETWORK_EVENT_TYPE::DATA_RECEIVED:
	{
		NETWORK_GAMELAYER_EVENT* recievedData = reinterpret_cast<NETWORK_GAMELAYER_EVENT*>(event.data->rawMsg);
		handleGameLayerNetworkEvents(*recievedData, event.clientID);
		break;
	}
	case NETWORK_EVENT_TYPE::HOST_ON_LAN_FOUND:
	{
		//TODO: move to general
		break;
	}
	default:
		break;
	}
}

void NetworkConnection::handleGameLayerNetworkEvents(NETWORK_GAMELAYER_EVENT& event, TCP_CONNECTION_ID tcp_connection_id) {

	switch (event.type) {
	case NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_JOINED:
	{
		Player player = {};
		player.playerName = event.event_data.player_joined.name;

		if (m_networkModule.isServer()) {
			//Complete the Player Info
			player.clientID = tcp_connection_id;
			player.playerID = m_players.size();

			//Inform all clients that a new player joined.
			NETWORK_GAMELAYER_EVENT playerJoined = {};
			playerJoined.type = NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_JOINED;

			memcpy_s(playerJoined.event_data.player_joined.name, NETWORK_MAX_PLAYER_NAME_LENGTH, player.playerName.c_str(), player.playerName.size());
			playerJoined.event_data.player_joined.playerID = player.playerID;

			m_networkModule.send(reinterpret_cast<char*>(&playerJoined), sizeof(NETWORK_GAMELAYER_EVENT), -1);
		} else {
			player.clientID = 0;
			player.playerID = event.event_data.player_joined.playerID;
		}

		m_players.push_back(player);
	}
	break;
	case NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_LEFT:
		break;
	case NETWORK_GAMELAYER_EVENT_TYPE::PLAYER_CHANGED_NAME:
		if (m_networkModule.isServer()) {
			for (auto& i : m_pending_connection) {
				if (i == tcp_connection_id) {
					break;
				}
			}
		} else {

		}
		break;
	case NETWORK_GAMELAYER_EVENT_TYPE::CHAT_MESSAGE_RECEIVED:
	{
		if (m_networkModule.isServer()) {
			m_networkModule.send(reinterpret_cast<char*>(&event), sizeof(event), -1);
		}

		ChatMessageReceivedEvent chatMessage;
		chatMessage.message = event.event_data.chat_message_received.message;
		m_pEventManager->emit(chatMessage);
	}
	break;
	case NETWORK_GAMELAYER_EVENT_TYPE::REQUEST_STATE_CHANGE:
		break;
	case NETWORK_GAMELAYER_EVENT_TYPE::GAME_DATA_MESSAGE:
		break;
	case NETWORK_GAMELAYER_EVENT_TYPE::UNDEFINED:
	default:
		//Error
		break;
	}
}
