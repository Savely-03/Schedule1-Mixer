#include "Utilities.h"
#include "UI.h"
#include "Logging.h" // For logDebug
#include <CommCtrl.h>
#include <Richedit.h>
#include <codecvt>
#include <string>
#include <sstream>
#include <uxtheme.h>  // For SetWindowTheme
#pragma comment(lib, "dwmapi.lib")  // Link with dwmapi.lib
#pragma comment(lib, "uxtheme.lib") // Link with uxtheme.lib

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

void EnableDarkMode(HWND hwnd, bool enable, HBRUSH hLightBrush, HBRUSH hDarkBrush, HBRUSH hControlLightBrush, HBRUSH hControlDarkBrush) {
    logDebug(L"EnableDarkMode called. Enable: " + std::wstring(enable ? L"true" : L"false"));

    // Enable dark mode for the window
    BOOL darkMode = enable ? TRUE : FALSE;
    HRESULT hr = MyDwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
    if (FAILED(hr)) {
        logDebug(L"Failed to set dark mode attribute. HRESULT: " + std::to_wstring(hr));
    }

    // Set the background brush
    HBRUSH hBrush = enable ? hDarkBrush : hLightBrush;
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

    // Update child controls
    HWND child = GetWindow(hwnd, GW_CHILD);
    while (child) {
        wchar_t className[256];
        GetClassName(child, className, sizeof(className) / sizeof(wchar_t));

        if (wcscmp(className, L"SysListView32") == 0) {
            static bool listViewDarkModeApplied = false;
            if (!listViewDarkModeApplied) {
                SetWindowTheme(child, enable ? L"DarkMode_Explorer" : NULL, NULL);
                ListView_SetBkColor(child, enable ? RGB(37, 37, 38) : RGB(255, 255, 255));
                ListView_SetTextBkColor(child, enable ? RGB(37, 37, 38) : RGB(255, 255, 255));
                ListView_SetTextColor(child, enable ? RGB(255, 255, 255) : RGB(0, 0, 0));
                listViewDarkModeApplied = true;
            }
        }
        else if (wcscmp(className, L"Edit") == 0) {
            // Apply dark mode to Edit controls
            SetWindowTheme(child, enable ? L"DarkMode_Explorer" : NULL, NULL);
            SendMessage(child, EM_SETBKGNDCOLOR, 0, enable ? RGB(37, 37, 38) : RGB(255, 255, 255));
        }
        else if (wcscmp(className, L"Button") == 0) {
            // Apply dark mode to Buttons
            SetWindowTheme(child, enable ? L"DarkMode_Explorer" : NULL, NULL);
        }

        child = GetWindow(child, GW_HWNDNEXT);
    }

    // Redraw the window to apply changes
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

void clearListView(HWND hListView) {
    ListView_DeleteAllItems(hListView);
}
void addListViewItem(HWND hListView, int index, const std::wstring& name, const std::wstring& startingBase,
    const std::wstring& cost, const std::wstring& ingredients, const std::wstring& effects) {
    LVITEM lvItem = {};
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = index;

    // Add the name
    lvItem.pszText = const_cast<wchar_t*>(name.c_str());
    lvItem.iSubItem = 0; // First column
    ListView_InsertItem(hListView, &lvItem);

    // Add the starting base
    ListView_SetItemText(hListView, index, 1, const_cast<wchar_t*>(startingBase.c_str()));

    // Add the cost
    ListView_SetItemText(hListView, index, 2, const_cast<wchar_t*>(cost.c_str()));

    // Add the ingredients
    ListView_SetItemText(hListView, index, 3, const_cast<wchar_t*>(ingredients.c_str()));

    // Add the effects
    ListView_SetItemText(hListView, index, 4, const_cast<wchar_t*>(effects.c_str()));
}
std::wstring convertIngredientsToString(const std::vector<Ingredient>& ingredients) {
    std::wostringstream ingredientsStream;
    for (const auto& ingredient : ingredients) {
        ingredientsStream << ingredient.name << L", ";
    }
    std::wstring ingredientsString = ingredientsStream.str();
    if (!ingredientsString.empty()) {
        ingredientsString.pop_back(); // Remove trailing space
        ingredientsString.pop_back(); // Remove trailing comma
    }
    return ingredientsString;
}
std::wstring convertEffectsToString(const std::map<std::wstring, std::wstring>& effects) {
    std::wostringstream effectsStream;
    for (const auto& effect : effects) {
        effectsStream << effect.first << L", ";
    }
    std::wstring effectsString = effectsStream.str();
    if (!effectsString.empty()) {
        effectsString.pop_back(); // Remove trailing space
        effectsString.pop_back(); // Remove trailing comma
    }
    return effectsString;
}

void populateListView(HWND hListView, const std::vector<DynamicBase>& mixes) {
    ListView_DeleteAllItems(hListView); // Clear existing items

    LVITEM lvItem = {};
    lvItem.mask = LVIF_TEXT;

    for (size_t i = 0; i < mixes.size(); ++i) {
        const auto& mix = mixes[i];

        // Add "Name" column data
        lvItem.iItem = static_cast<int>(i);
        lvItem.iSubItem = 0;
        lvItem.pszText = const_cast<LPWSTR>(mix.mixtureName.c_str());
        ListView_InsertItem(hListView, &lvItem);

        // Add "Starting Base" column data
        lvItem.iSubItem = 1;
        lvItem.pszText = const_cast<LPWSTR>(mix.startingBase.name.c_str());
        ListView_SetItem(hListView, &lvItem);

        // Add "Cost" column data
        lvItem.iSubItem = 2;
        std::wstring cost = std::to_wstring(mix.totalCost);
        lvItem.pszText = const_cast<LPWSTR>(cost.c_str());
        ListView_SetItem(hListView, &lvItem);

        // Add "Ingredients Added" column data
        lvItem.iSubItem = 3;
        std::wstring ingredients = convertIngredientsToString(mix.addedIngredients);
        lvItem.pszText = const_cast<LPWSTR>(ingredients.c_str());
        ListView_SetItem(hListView, &lvItem);

        // Add "Effects" column data
        lvItem.iSubItem = 4;
        std::wstring effects = convertEffectsToString(mix.currentEffects);
        lvItem.pszText = const_cast<LPWSTR>(effects.c_str());
        ListView_SetItem(hListView, &lvItem);
    }
}