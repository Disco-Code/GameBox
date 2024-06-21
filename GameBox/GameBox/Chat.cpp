#include "Chat.h"
#include "imgui/imgui.h"
#include <SFML/Graphics.hpp>
#include "Game.h"

// Duplicates a string using malloc and memcpy
// Returns a pointer to the duplicated string
static char* Strdup(const char* s) {
	// Assert that s is not null
	IM_ASSERT(s);

	// Calculate length of s and allocate memory for the duplicated string
	size_t len = strlen(s) + 1;
	void* buf = malloc(len);

	// Assert that memory was successfully allocated
	IM_ASSERT(buf);

	// Copy s into buf and return a pointer to buf
	return (char*)memcpy(buf, (const void*)s, len);
}


Chat::Chat() {
}

Chat::~Chat() {
	if (m_eventManager) {
		m_eventManager->unsubscribe<ChatMessageReceivedEvent>(*this);
	}

	for (auto& i : m_items) {
		delete[] i;
	}
}

bool Chat::Initialize(entityx::EventManager* pEventManager) {
	m_eventManager = pEventManager;
	m_eventManager->subscribe<ChatMessageReceivedEvent>(*this);

	return true;
}

// Render chat window and handle user input
// 
// xPos:      x-coordinate position of top-left corner of chat window
// yPos:      y-coordinate position of top-left corner of chat window
// width:     width of chat window in pixels
// height:    height of chat window in pixels
void Chat::Draw(int xPos, int yPos, int width, int height) {
	// Set window flags to prevent user interaction
	ImGuiWindowFlags chatWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

	// Set size and position of chat window
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(xPos, yPos));

	// Begin rendering chat window
	if (!ImGui::Begin("Chat", (bool*)1, chatWindowFlags)) {
		ImGui::End();
		return;
	}

	// Options menu
	if (ImGui::BeginPopup("Options")) {
		// Toggle auto-scroll
		ImGui::Checkbox("Auto-scroll", &m_autoScroll);
		ImGui::EndPopup();
	}

	// Options button
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::Separator();

	// Reserve space for input field at the bottom of the chat window
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false)) {
		// Tighten spacing between messages
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		// Display chat messages
		for (int i = 0; i < m_items.size(); i++) {
			const char* item = m_items[i];

			// Set color of message based on its content
			ImVec4 color;
			bool has_color = false;
			if (strstr(item, "[error]")) {
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true;
			} else if (strncmp(item, "# ", 2) == 0) {
				color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true;
			}
			if (has_color)
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextWrapped(item);
			if (has_color)
				ImGui::PopStyleColor();
		}

		// Scroll to bottom if necessary
		if (m_scrollToBottom || (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		m_scrollToBottom = false;

		// Reset spacing between messages
		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	// Separator between chat messages and input field
	ImGui::Separator();

	// Input field for new chat message
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
	if (ImGui::InputText("Input", m_inputBuf, IM_ARRAYSIZE(m_inputBuf), input_text_flags)) {
		// Send chat message and clear input field when "enter" is pressed
		char* s = m_inputBuf;
		if (s[0]) {
			ChatMessageSentEvent chatSent;
			chatSent.message = s;
			m_eventManager->emit(chatSent);
		}
		strcpy_s(s, IM_ARRAYSIZE(m_inputBuf), "");
		reclaim_focus = true;
	}

	// Set default focus to input field
	ImGui::SetItemDefaultFocus();
	// If input field was just cleared, give it focus again
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1);

	// End chat window rendering
	ImGui::End();
}

void Chat::AddMessage(const char* msg) {
	m_items.push_back(Strdup(msg));
	if (m_autoScroll) {
		m_scrollToBottom = true;
	}
}

void Chat::receive(const ChatMessageReceivedEvent& event) {
	AddMessage(event.message);
}
