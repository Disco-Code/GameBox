#pragma once
#include <string>
#include <filesystem>

// Directory where the settings file is stored.
const std::filesystem::path G_CONFIG_PATH = "config";
// Name of the settings file.
const std::filesystem::path G_SETTINGS_FILENAME = "settings.txt";

// SETTING_STRING macro is used to define a setting that stores a string value.
// It takes three arguments:
// 1. varName: the name of the setting variable. This argument should be the name you want to use for the variable that will store the value of the setting.
// 2. displayName: the display name of the setting. This argument should be a string literal that will be used as the display name of the setting when it is saved or loaded from a file.
// 3. defaultValue: the default value for the setting. This argument should be a string literal that will be used as the default value for the setting.
// The macro defines two variables: a variable to store the value of the setting, and a variable to store the display name of the setting.
// The value variable is a std::string and is initialized with the default value provided as an argument.
// The display name variable is a const char* (a pointer to a constant character) and is initialized with the display name provided as an argument.
#define SETTING_STRING(varName, displayName, defaultValue) std::string varName = defaultValue; \
 const char* varName##_##disp = displayName;

// SETTING_INT macro is used to define a setting that stores an int value.
#define SETTING_INT(varName, displayName, defaultValue) int varName = defaultValue; \
 const char* varName##_##disp = displayName;

#define MAX_PLAYER_NAME_LENGTH 32

// Settings class represents the game's settings and provides methods for saving and loading them from a file.
class Settings {
public:
	Settings();
	~Settings();
	// Saves the current settings to a file.
	void Save();
	// Loads the settings from a file.
	void Load();

	// Returns the player's name.
	const std::string& GetPlayerName();
	// Sets the player's name.
	void SetPlayerName(const std::string& name);
	void SetPlayerName(const char* name);

	// Returns the path to the settings file.
	std::filesystem::path GetSettingsFilePath();

	void RandomizePlayerName();
private:
	// Player's name setting.
	SETTING_STRING(m_playerName, "PlayerName", ""); //This might be an to overly complicated way of creating the two variables named m_playerName and m_playerName_disp. m_playerName get assigned "player" and m_playerName_disp will be assigned "PlayerName".
};