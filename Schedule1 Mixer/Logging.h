#pragma once
#ifndef LOGGING_H
#define LOGGING_H

#include <string>

// Function declarations
void logDebug(const std::wstring& message);
void logSuccess(const std::wstring& message);
void LogError(const std::wstring& errorMessage);
void RecoverFromError();

#endif // LOGGING_H