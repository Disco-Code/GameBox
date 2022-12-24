#include "Settings.h"
#include <fstream>

// SAVE_SETTING is a macro that writes a setting and its value to a stream.
// It takes the name of the setting (varName) and a stream (stream) as arguments.
// The macro writes the setting's display name (varName_disp), followed by a colon and the setting's value (varName).
// It also adds a newline character at the end.
#define SAVE_SETTING(varName, stream) stream << varName##_##disp << ":" << varName << "\n"

Settings::Settings() {

}

Settings::~Settings() {

}

void Settings::Save() {
	// Create the directories for the config file if they don't exist.
	std::filesystem::create_directories(G_CONFIG_PATH);
	// Open the settings file for writing.
	std::ofstream file(GetSettingsFilePath());
	// Check if the file was successfully opened.
	if (file.is_open()) {
		// Write the player's name setting to the file using the SAVE_SETTING macro.
		SAVE_SETTING(m_playerName, file);
	}
}

void Settings::Load() {
	// Open the settings file for reading.
	std::ifstream file(GetSettingsFilePath());

	// Check if the file was successfully opened.
	if (file.is_open()) {
		// Read each line of the file.
		std::string line;
		while (std::getline(file, line)) {
			// Split the line at the first colon character.
			int split = line.find_first_of(":");

			// Check if the line was successfully split.
			if (split != std::string::npos) {
				// Extract the setting's name and value from the line.
				std::string settingName = line.substr(0, split);
				std::string value = line.substr(split + 1);

				// Check if the setting is the player's name.
				if (settingName == m_playerName_disp) {
					// Update the player's name with the value from the file.
					m_playerName = value;
				}
				// You can add additional checks for other settings here using an else if clause.
				// For example:
				/*else if (settingName == m_otherSetting_disp) {
					m_otherSetting = value;
				}*/
			}
		}
	}
}

const std::string& Settings::GetPlayerName() const {
	return m_playerName;
}

void Settings::SetPlayerName(const std::string& name) {
	m_playerName = name;
}

void Settings::SetPlayerName(const char* name) {
	m_playerName = name;
}

std::filesystem::path Settings::GetSettingsFilePath() {
	// Concatenate the directory path and file name to get the full path to the settings file.
	auto path = G_CONFIG_PATH.string() + "/" + G_SETTINGS_FILENAME.string();
	// Return the full path to the settings file.
	return path;
}
