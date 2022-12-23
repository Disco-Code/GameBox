#pragma once

#include <sstream>
#include <iostream>
#include "State.h"
#include <functional>
#include "Common UI/Button.h"
#include <filesystem>

class MainMenuState final : public State {
public:
	MainMenuState(Game* pGame);
	~MainMenuState();

	void update(float dt) override;
	void processInput(float dt) override;
	virtual void handleWindowEvent(const sf::Event& windowEvent) override;
	virtual void renderGUI(float dt) override;
private:
	// Attempts to join a server at the specified IP address and port number.
	void joinServer(std::string ip, unsigned int port = 0);
	// Saves the list of saved servers to a file
	void saveIPs();
	// Loads a list of saved server IP addresses from a file, and adds them to the list of saved servers
	void loadIPs();
	// Adds a server to the list of saved servers, if it is not a duplicate
	void addServerToList(std::string serverAddress);
	void renderServerList();
	/*
		Returns the ID of the button currently being hovered over by the mouse, or -1 if no button is being hovered over
	*/
	size_t getHoveredButtonID();

	size_t m_selectedButton = 0;
	std::vector<Button> m_buttons;

	sf::Texture m_logoTexture;
	sf::Sprite m_logoSprite;

	//ServerList
	char m_playerName[32] = "Player";
	bool m_bIsServerListOpen = false;
	std::string m_joinErrorMessage = "";
	std::vector<std::string> m_savedIPs;
	int m_selectedServer = -1;
	const std::filesystem::path m_serverListPath = "config/serverlist.txt";
};