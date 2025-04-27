#include "Utilities.h"
#include "Logging.h" // For logDebug
#include "Mixing.h"  // For global variables like currentBase, currentMixture, etc.
#include <fstream>
#include <string>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

void SetWorkingDirectoryToExecutablePath() {
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
        std::wstring exeDir = std::wstring(exePath).substr(0, std::wstring(exePath).find_last_of(L"\\/"));
        if (SetCurrentDirectoryW(exeDir.c_str())) {
            logDebug(L"Working directory set to: " + exeDir);
        }
        else {
            logDebug(L"Failed to set working directory to: " + exeDir);
        }
    }
    else {
        logDebug(L"Failed to retrieve executable path.");
    }
}

void savePreferences() {
    std::wofstream prefsFile(L"preferences.json");
    if (prefsFile.is_open()) {
        prefsFile.imbue(std::locale("en_US.UTF-8")); // Ensure UTF-8 encoding
        prefsFile << L"{ \"darkMode\": " << (isDarkModeEnabled ? L"true" : L"false") << L" }";
        prefsFile.close();
    }
}

void loadPreferences() {
    std::wifstream prefsFile(L"preferences.json");
    if (prefsFile.is_open()) {
        prefsFile.imbue(std::locale("en_US.UTF-8")); // Ensure UTF-8 encoding
        std::wstring line;
        std::getline(prefsFile, line);
        isDarkModeEnabled = (line.find(L"\"darkMode\": true") != std::wstring::npos);
        prefsFile.close();
    }
}

void updateButtonStates(HWND hEditCustomButton, HWND hCookbookButton) {
    if (isCustomMixesFileEmpty) {
        // Disable the buttons visually
        EnableWindow(hEditCustomButton, FALSE);
        EnableWindow(hCookbookButton, FALSE);
    }
    else {
        // Enable the buttons if the file is not empty
        EnableWindow(hEditCustomButton, TRUE);
        EnableWindow(hCookbookButton, TRUE);
    }
}

void cleanupResources() {
    currentBase.reset();
    currentMixture.reset();
    selectedCustomBase.reset();
    backupMixture.reset();

    // Clear undo and redo stacks
    while (!undoStack.empty()) undoStack.pop();
    while (!redoStack.empty()) redoStack.pop();
}