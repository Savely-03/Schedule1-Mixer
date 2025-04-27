#include "Logging.h"
#include <fstream>
#include <iostream>
#include <windows.h>

void logDebug(const std::wstring& message) {
    static bool isFileCleared = false; // Ensure the file is cleared only once per run

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath).substr(0, std::wstring(exePath).find_last_of(L"\\/"));

    std::wstring debugFilePath = exeDir + L"\\debug_log.txt";

    // Clear the file on the first call
    if (!isFileCleared) {
        std::wofstream debugLog(debugFilePath, std::ios::trunc);
        if (debugLog.is_open()) {
            debugLog.imbue(std::locale("en_US.UTF-8")); // Set UTF-8 encoding
            debugLog.close();
        }
        isFileCleared = true;
    }

    // Append the new message
    std::wofstream debugLog(debugFilePath, std::ios::app);
    if (debugLog.is_open()) {
        debugLog.imbue(std::locale("en_US.UTF-8")); // Set UTF-8 encoding
        debugLog << message << std::endl;
        debugLog.close();
    }
    else {
        std::wcerr << L"Failed to open debug_log.txt for writing." << std::endl;
    }
}

void logSuccess(const std::wstring& message) {
    logDebug(L"SUCCESS: " + message); // Reuse logDebug for simplicity
}

void LogError(const std::wstring& errorMessage) {
    std::wofstream logFile(L"error_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile.imbue(std::locale("en_US.UTF-8")); // Set UTF-8 encoding
        logFile << L"Error: " << errorMessage << std::endl;
        logFile.close();
    }
}

void RecoverFromError() {
    MessageBox(NULL, TEXT("An error occurred. The program will attempt to recover."), TEXT("Error"), MB_OK | MB_ICONERROR);
}