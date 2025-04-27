#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <windows.h>

// Typedef for the DwmSetWindowAttribute function pointer
typedef HRESULT(WINAPI* DwmSetWindowAttributeFunc)(HWND, DWORD, LPCVOID, DWORD);

// Utility functions
void SetWorkingDirectoryToExecutablePath();
void savePreferences();
void loadPreferences();
void updateButtonStates(HWND hEditCustomButton, HWND hCookbookButton);
void cleanupResources();

#ifndef MY_DWM_SET_WINDOW_ATTRIBUTE_H
#define MY_DWM_SET_WINDOW_ATTRIBUTE_H
// Declare the function as extern
extern HRESULT MyDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
#endif // MY_DWM_SET_WINDOW_ATTRIBUTE_H

#endif // UTILITIES_H