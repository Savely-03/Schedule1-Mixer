#pragma once
#ifndef UI_H
#define UI_H

#include <windows.h>
#include <vector>
#include "Mixing.h"


// UI-related functions
void EnableDarkMode(HWND hwnd, bool isDarkModeEnabled, HBRUSH hLightBrush, HBRUSH hDarkBrush, HBRUSH hControlLightBrush, HBRUSH hControlDarkBrush);
void ApplyDarkModeToControls(HWND hwnd);
void populateListView(HWND hListView, const std::vector<DynamicBase>& mixes);

#endif // UI_H