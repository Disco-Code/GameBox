#include "MainMenuState.h"
#include "../Game.h"
#include <fstream>
#include "../Helpers.h"

MainMenuState::MainMenuState(Game* pGame) : State(States::MainMenu, pGame) {

	m_logoTexture.loadFromFile("../Resources/DiscoNDungeons.png");
	m_logoSprite.setTexture(m_logoTexture);

	m_logoSprite.setPosition(-10, 0);

	float posX = 50;
	float posY = 150;

	m_buttons.push_back(Button("Start Game", pGame->GetFont(), [&]() {printf("MainMenu: \"Start Game\" Button Pressed!\n"); m_game->SetState(States::Game); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("Multiplayer", pGame->GetFont(), [&]() {
		m_bIsServerListOpen = true;
		}));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("Settings", pGame->GetFont(), [&]() {printf("MainMenu: \"Settings\" Button Pressed!\n"); m_game->SetState(States::SettingsMenu);  }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("About (not implemented)", pGame->GetFont(), [&]() {printf("MainMenu: \"About\" Button Pressed!\n"); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons.push_back(Button("Quit", pGame->GetFont(), [&]() {printf("MainMenu: \"Exit\" Button Pressed!\n"); m_game->getWindow()->close(); }));
	m_buttons.back().setPosition(posX, posY);
	posY += 50;

	m_buttons[m_selectedButton].SetIsSelected(true);

	//Load Saved Server IPs
	loadIPs();
}

MainMenuState::~MainMenuState() {

}

void MainMenuState::update(float dt) {
	sf::RenderWindow* window = m_game->getWindow();
	window->draw(m_logoSprite);

	for (auto& i : m_buttons) {
		window->draw(i);
	}
}

void MainMenuState::processInput(float dt) {

}

void MainMenuState::handleWindowEvent(const sf::Event& windowEvent) {

	// switch statement to handle different types of window events
	switch (windowEvent.type) {

		// case for when a key is pressed
	case sf::Event::EventType::KeyPressed:
		// if the Enter key is pressed, trigger the currently selected button
		if (windowEvent.key.code == sf::Keyboard::Escape) {
			m_bIsServerListOpen = false;
		}

		if (m_bIsServerListOpen) {
			//Skip keyboard events if the ServerListWindow is open. ServerList input is handled by IMGUI.
			return;
		}

		// if the Enter key is pressed, trigger the currently selected button
		if (windowEvent.key.code == sf::Keyboard::Enter) {
			m_buttons[m_selectedButton].Trigger();
		}

		// if the W or Up arrow key is pressed, move the selection up to the previous button if it exists
		if (windowEvent.key.code == sf::Keyboard::W || windowEvent.key.code == sf::Keyboard::Up) {
			if (m_selectedButton > 0) {
				m_buttons[m_selectedButton--].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}

		// if the S or Down arrow key is pressed, move the selection down to the next button if it exists
		if (windowEvent.key.code == sf::Keyboard::S || windowEvent.key.code == sf::Keyboard::Down) {
			if (m_selectedButton < m_buttons.size() - 1) {
				m_buttons[m_selectedButton++].SetIsSelected(false);
				m_buttons[m_selectedButton].SetIsSelected(true);
			}
		}
		break;

		// case for when a mouse button is pressed
	case sf::Event::MouseButtonPressed:
	{
		// get the ID of the button currently being hovered over
		size_t buttonID = getHoveredButtonID();

		// if a button is being hovered over, trigger it
		if (buttonID != -1) {
			m_buttons[buttonID].Trigger();
		}
		break;
	}
	// case for when the mouse is moved
	case sf::Event::EventType::MouseMoved:
	{
		// get the ID of the button currently being hovered over
		size_t buttonID = getHoveredButtonID();

		// if a button is being hovered over, set it as the selected button
		if (buttonID != -1) {
			m_buttons[m_selectedButton].SetIsSelected(false);
			m_buttons[buttonID].SetIsSelected(true);
			m_selectedButton = buttonID;
		}
		break;
	}
	// default case for handling any other event types not covered in the switch statement
	default:
		break;
	}

}

size_t MainMenuState::getHoveredButtonID() {

	size_t id = 0;
	for (auto& i : m_buttons) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_game->getWindow());
		if (i.GetRectangle().contains(mousePos.x, mousePos.y)) {
			return id;
		}
		id++;
	}

	return -1;
}

void MainMenuState::renderGUI(float dt) {
	if (m_bIsServerListOpen) {
		renderServerList();
	}
}
/**
	Attempts to join a server at the specified IP address and port number.

	@param ip       -The IP address of the server, as a string. This can be in the form "x.x.x.x" or "x.x.x.x:port".
	@param port     -The port number of the server to connect to. If the 'ip' argument is in the form "x.x.x.x:port", the port number will be extracted from the 'ip' string. Otherwise, the provided 'port' argument will be used.

	@return         -None.

	If the IP address string 'ip' contains a ':' character, the function will attempt to extract the port number from the string. The port number
	is assumed to be the substring after the ':'. If 'ip' does not contain a ':', or if the port number could not be extracted, the provided
	'port' argument will be used.

	The function will then attempt to join the server using the 'm_game->m_networkConnection.Join()' function. If the join was successful, the
	game's state will be set to 'States::Lobby'. If the join was unsuccessful, the error message 'm_joinErrorMessage' will be set to
	"Could not join: x.x.x.x:port", where 'x.x.x.x' is the provided IP address and 'port' is the provided or extracted port number.

	Example input/output scenarios:
	* joinServer("192.168.0.1", 1234) -> Attempts to join the server at IP address "192.168.0.1" and port number 1234.
	* joinServer("192.168.0.1:1234", 0) -> Attempts to join the server at IP address "192.168.0.1" and port number 1234.
	* joinServer("192.168.0.1:abcd", 0) -> Attempts to join the server at IP address "192.168.0.1" and port number 0. The port number could not be extracted from the input string.
	* joinServer("192.168.0.1", 1234) -> Attempts to join the server at IP address "192.168.0.1" and port number 1234.
*/
void MainMenuState::joinServer(std::string ip, unsigned int port) {
	// Check if the IP address string contains a ':' character
	size_t seperatorIndex = ip.find_first_of(":");
	if (seperatorIndex != std::string::npos) {
		// If the ':' character was found, extract the port number from the string
		std::string port_string = ip.substr(seperatorIndex + 1);
		port = std::stoi(port_string); // Convert the port number string to an integer
		// Extract the IP address from the string
		ip = ip.substr(0, seperatorIndex);
	}

	// Attempt to join the server
	if (m_game->m_networkConnection.Join()) {
		// If the join was successful, switch to the lobby state
		m_game->SetState(States::Lobby);
	} else {
		// If the join was unsuccessful, set the error message to be displayed in the server list window
		m_joinErrorMessage = "Could not join: " + ip + ":" + std::to_string(port);
	}
}

void MainMenuState::saveIPs() {
	// Create the directories for the server list file, if they don't already exist
	std::filesystem::create_directories(m_serverListPath.parent_path());

	// Open the server list file for writing
	std::ofstream file(m_serverListPath);
	if (file.is_open()) {
		// Write each server in the list to a separate line in the file
		for (auto& i : m_savedIPs) {
			file << i << "\n";
		}
	}
}


void MainMenuState::loadIPs() {
	// Check if the server list file exists
	if (std::filesystem::exists(m_serverListPath)) {
		// Open the file for reading
		std::ifstream file(m_serverListPath);
		if (file.is_open()) {
			// Read each line of the file and add it to the list of saved servers
			std::string line;
			while (std::getline(file, line)) {
				addServerToList(line);
			}
		}
	}
}

void MainMenuState::addServerToList(std::string serverAddress) {
	bool duplicate = false;
	// Check if the server is already in the list
	for (auto& i : m_savedIPs) {
		if (i == serverAddress) {
			duplicate = true;
			break;
		}
	}
	// If the server is not a duplicate, add it to the list
	if (!duplicate) {
		m_savedIPs.push_back(serverAddress);
	}
}

void MainMenuState::renderServerList() {
	sf::Vector2u windowSize = m_game->getWindow()->getSize();

	// Set the size and position of the server list window
	ImGui::SetNextWindowSize(ImVec2(500, windowSize.y));
	ImGui::SetNextWindowPos(ImVec2(windowSize.x - 500, 0));

	// Begin the server list window, with no collapse, move, resize, or saved settings
	ImGui::Begin("Server List", &m_bIsServerListOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

	ImGui::TextUnformatted("Your Name:");
	ImGui::SameLine();
	ImGui::TextUnformatted(m_game->GetSettings().GetPlayerName().c_str());

	ImGui::Separator();
	// Create a button to join the currently selected server
	if (ImGui::Button("Join")) {
		if (m_selectedServer > -1 && m_selectedServer < m_savedIPs.size()) {
			joinServer(m_savedIPs[m_selectedServer]);
		}
	}

	// Create a button to host a new server, and switch to the lobby state if successful
	ImGui::SameLine();
	if (ImGui::Button("Host")) {
		if (m_game->m_networkConnection.Host()) {
			m_game->SetState(States::Lobby);
		} else {
			m_joinErrorMessage = "Could not host a server!";
		}
	}

	// Calculate the height of the footer, which will reserve space for the buttons and error message
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() * 2;

	// Begin a child window that will contain the server list
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), true)) {
		// If the options pop-up is open, display the "Copy" and "Remove" buttons
		if (ImGui::BeginPopup("Options")) {
			if (ImGui::Button("Copy")) {
				copyToClipboard(m_savedIPs[m_selectedServer]);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove")) {
				m_savedIPs.erase(m_savedIPs.begin() + m_selectedServer);
				m_selectedServer = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		// Display a list of saved servers that the user can select and right-click to access options
		for (size_t i = 0; i < m_savedIPs.size(); i++) {
			if (ImGui::Selectable(m_savedIPs[i].c_str(), i == m_selectedServer, ImGuiSelectableFlags_AllowDoubleClick)) {
				m_selectedServer = i;
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					joinServer(m_savedIPs[i]);
				}
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
				// If the user right-clicks on a server, open the options pop-up and select the server
				m_selectedServer = i;
				ImGui::OpenPopup("Options");
			}
		}
		ImGui::EndChild();
	}

	// Create a static buffer to hold the user's entered IP address and port number
	static char buffer[256] = "127.0.0.1:1234";

	// Create a button to manually join the entered server
	if (ImGui::Button("Join IP")) {
		joinServer(buffer);
	}

	// Create a button to save the entered server to the list of saved servers
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		addServerToList(buffer);
		saveIPs();
	}

	// Create a text input field for the user to enter an IP address and port number
	ImGui::SameLine();
	if (ImGui::InputText("IP", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		// If the user hits the enter key, attempt to join the entered server
		joinServer(buffer);
	}

	// If an error message has been set, display it in the window
	if (m_joinErrorMessage != "") {
		ImGui::TextUnformatted(m_joinErrorMessage.c_str());
	}

	// End the server list window
	ImGui::End();
}
