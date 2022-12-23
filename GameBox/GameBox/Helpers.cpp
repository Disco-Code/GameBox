#include "Helpers.h"
#include <Windows.h>

void copyToClipboard(const std::string& str) {
    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        return;
    }

    // Clear the clipboard
    EmptyClipboard();

    // Allocate memory for the string
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
    if (hMem == nullptr) {
        CloseClipboard();
        return;
    }

    // Copy the string to the allocated memory
    memcpy(GlobalLock(hMem), str.c_str(), str.size() + 1);
    GlobalUnlock(hMem);

    // Set the data in the clipboard
    SetClipboardData(CF_TEXT, hMem);

    // Close the clipboard
    CloseClipboard();
}
