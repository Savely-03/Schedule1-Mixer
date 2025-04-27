// Schedule1 Mixer.cpp : Defines the entry point for the application.
//

#include "NewResource.h"
#include "framework.h"
#include "Schedule1 Mixer.h"
#include "windows.h"
#include "Logging.h"
#include "Models.h"
#include "Mixing.h"
#include "Data.h"
#include "Utilities.h"
#include "UI.h"
#include <commdlg.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <locale>
#include <CommCtrl.h>
#include <codecvt>
#include <algorithm>
#include <stack>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

HINSTANCE g_hInstance;
bool isCustomBaseManuallySelected = false; // Flag to check if a custom base is selected manually
bool isCustomMixesFileEmpty; // Flag to check if the custom mixes file is empty
bool messageBoxShown = false; // Flag to check if the message box has been displayed
static bool isCookbookDialogOpen = false;
bool isDarkModeEnabled = false;
static bool isAscending = false; // Default to descending (A-Z)
static int currentSortedColumn = 0; // Default to "Name" column

static HBRUSH hLightBrush = CreateSolidBrush(RGB(255, 255, 255)); // White for light mode Background
static HBRUSH hDarkBrush = CreateSolidBrush(RGB(37, 37, 38));      // Darker gray for Dark Mode background
static HBRUSH hControlLightBrush = CreateSolidBrush(RGB(255, 255, 255)); // White for controls in Light Mode
static HBRUSH hControlDarkBrush = CreateSolidBrush(RGB(62, 62, 66));  // Lighter gray for controls in Dark Mode

// List to store custom mixes
std::vector<std::wstring> customMixes; // Updated to std::wstring

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          FUNCTIONS START HERE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to load custom mixes from a file
std::vector<DynamicBase> loadCustomMixes() {
    logDebug(L"Loading custom mixes...");

    std::vector<DynamicBase> mixes;
    std::wifstream mixListFile(L"custom_mixes.txt"); // Updated to wide string

    if (!mixListFile.is_open()) {
        logDebug(L"Error: Unable to open custom_mixes.txt for loading.");
        MessageBox(NULL, TEXT("Failed to load custom mixes!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return mixes;
    }

    mixListFile.imbue(std::locale("en_US.UTF-8")); // Ensure the correct locale for UTF-8

    std::wstring line;
    while (std::getline(mixListFile, line)) {
        logDebug(L"Reading line: " + line);

        if (line.empty()) {
            logDebug(L"Skipping empty line.");
            continue;
        }

        std::wistringstream iss(line);
        std::wstring name, baseName, effects, ingredients;
        double cost;

        if (std::getline(iss, name, L',') &&
            std::getline(iss, baseName, L',') &&
            std::getline(iss, effects, L',') &&
            std::getline(iss, ingredients, L',') &&
            iss >> cost) {

            logDebug(L"Parsed fields: Name=" + name + L", BaseName=" + baseName + L", Cost=" + std::to_wstring(cost) +
                L", Effects=" + effects + L", Ingredients=" + ingredients);

            Base base(baseName, cost, L"");
            DynamicBase mix(base);
            mix.mixtureName = name;
            mix.totalCost = cost;

            // Parse effects
            if (!effects.empty()) {
                std::wistringstream effectsStream(effects);
                std::wstring effect;
                while (std::getline(effectsStream, effect, L';')) {
                    logDebug(L"Parsed effect: " + effect);
                    mix.currentEffects[effect] = effect;
                }
            }

            // Parse ingredients
            if (!ingredients.empty()) {
                std::wistringstream ingredientsStream(ingredients);
                std::wstring ingredient;
                while (std::getline(ingredientsStream, ingredient, L';')) {
                    logDebug(L"Parsed ingredient: " + ingredient);
                    mix.addedIngredients.push_back(Ingredient(ingredient, 0.0, L"", {}));
                }
            }

            mixes.push_back(mix);
            logDebug(L"Successfully parsed mix: " + name);
        }
        else {
            logDebug(L"Failed to parse line: " + line);
        }
    }

    mixListFile.close();
    logDebug(L"Loaded " + std::to_wstring(mixes.size()) + L" mixes.");
    return mixes;
}

// Functions to Import/Export the 'Cook'book
void importCookbook(const std::wstring& filePath) {
    logDebug(L"Importing cookbook from file: " + filePath);

    std::wifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        logDebug(L"Error: Unable to open file for importing: " + filePath);
        MessageBox(NULL, TEXT("Failed to import cookbook!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Set the locale to UTF-8
    inputFile.imbue(std::locale("en_US.UTF-8"));

    // Handle BOM (if present)
    wchar_t bom;
    inputFile.read(&bom, 1);
    if (bom != 0xFEFF) { // Check for UTF-8 BOM
        inputFile.seekg(0); // Reset to the beginning if no BOM
    }

    std::vector<DynamicBase> importedMixes;
    std::wstring line;
    while (std::getline(inputFile, line)) {
        logDebug(L"Reading line: " + line);
        std::wistringstream iss(line);
        std::wstring name, baseName, effects, ingredients;
        double cost;

        // Parse the mix details
        if (std::getline(iss, name, L',') && std::getline(iss, baseName, L',') && iss >> cost && iss.ignore() &&
            std::getline(iss, effects, L',') && std::getline(iss, ingredients)) {
            Base base(baseName, cost, L""); // Create a base with the name and cost
            DynamicBase mix(base);
            mix.mixtureName = name;
            mix.totalCost = cost;

            // Parse effects
            std::wistringstream effectsStream(effects);
            std::wstring effect;
            while (std::getline(effectsStream, effect, L';')) {
                mix.currentEffects[effect] = effect;
            }

            // Parse ingredients
            std::wistringstream ingredientsStream(ingredients);
            std::wstring ingredient;
            while (std::getline(ingredientsStream, ingredient, L';')) {
                mix.addedIngredients.push_back(Ingredient(ingredient, 0.0, L"", {}));
            }

            importedMixes.push_back(mix);
        }
        else {
            logDebug(L"Failed to parse line: " + line);
        }
    }

    inputFile.close();

    // Update loadedMixes and refresh the ListView
    loadedMixes = std::move(importedMixes);
    logDebug(L"Cookbook imported successfully with " + std::to_wstring(loadedMixes.size()) + L" mixes.");
    MessageBox(NULL, TEXT("Cookbook imported successfully!"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
}
void exportCookbook(const std::wstring& filePath) {
    logDebug(L"Exporting cookbook to file: " + filePath);

    std::wofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        logDebug(L"Error: Unable to open file for exporting: " + filePath);
        MessageBox(NULL, TEXT("Failed to export cookbook!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Set the locale to UTF-8
    outputFile.imbue(std::locale("en_US.UTF-8"));

    // Write BOM for UTF-8
    outputFile << L'\uFEFF';

    for (const auto& mix : loadedMixes) {
        outputFile << mix.mixtureName << L"," // Save mixture name
            << mix.startingBase.name << L"," // Save starting base name
            << mix.totalCost << L","; // Save total cost

        // Save effects
        for (const auto& effect : mix.currentEffects) {
            outputFile << effect.first << L";";
        }
        if (!mix.currentEffects.empty()) {
            outputFile.seekp(-1, std::ios_base::cur); // Remove trailing semicolon
        }
        outputFile << L",";

        // Save ingredients
        for (const auto& ingredient : mix.addedIngredients) {
            outputFile << ingredient.name << L";";
        }
        if (!mix.addedIngredients.empty()) {
            outputFile.seekp(-1, std::ios_base::cur); // Remove trailing semicolon
        }
        outputFile << L"\n";
    }

    outputFile.close();
    logDebug(L"Cookbook exported successfully.");
    MessageBox(NULL, TEXT("Cookbook exported successfully!"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
}

HRESULT MyDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) {
    static DwmSetWindowAttributeFunc pDwmSetWindowAttribute = nullptr;

    if (!pDwmSetWindowAttribute) {
        HMODULE hDwmApi = LoadLibrary(TEXT("dwmapi.dll"));
        if (hDwmApi) {
            pDwmSetWindowAttribute = (DwmSetWindowAttributeFunc)GetProcAddress(hDwmApi, "DwmSetWindowAttribute");
        }
    }

    if (pDwmSetWindowAttribute) {
        return pDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute);
    }

    return E_NOTIMPL; // Return an error if the function is not available
}

// Function to check if the custom mixes file is empty and show a message box if it is
void showNoCustomMixesMessage(HWND hwnd) {
    if (isCustomMixesFileEmpty && !messageBoxShown) {
        logDebug(L"MessageBox: No custom mixes found.");
        MessageBox(hwnd, TEXT("No custom mixes found. Please create a new mix to get started."), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
        messageBoxShown = true;
    }
}

// Function to log Success and Failure Messages
void AppendLogMessage(HWND hLogOutputArea, const std::wstring& message) {
    // Get the current text
    int length = GetWindowTextLengthA(hLogOutputArea);
    SendMessageA(hLogOutputArea, EM_SETSEL, (WPARAM)length, (LPARAM)length);

    // Append the new message
    std::wstring formattedMessage = message + L"\r\n";
    SendMessageA(hLogOutputArea, EM_REPLACESEL, FALSE, (LPARAM)formattedMessage.c_str());
}


//Dialog Procedure to handle the Mix-Naming dialog and Functions to Save the Custom Mix
INT_PTR CALLBACK NameDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static std::wstring* mixName = nullptr;

    switch (message) {
    case WM_INITDIALOG:
        // Store the pointer to the mix name passed via lParam
        mixName = reinterpret_cast<std::wstring*>(lParam);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            // Retrieve the entered name
            char buffer[128];
            GetDlgItemTextA(hDlg, IDC_SAVE_NAME, buffer, sizeof(buffer));
            if (mixName) {
                *mixName = std::wstring(buffer, buffer + strlen(buffer)); // Save the name to the provided string
            }
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void saveCustomMixes(HWND& hEditCustomButton, HWND& hCookbookButton) {
    logDebug(L"Saving custom mixes...");

    std::wofstream mixListFile("custom_mixes.txt");
    if (!mixListFile.is_open()) {
        logDebug(L"Error: Unable to open custom_mixes.txt for saving.");
        MessageBox(NULL, TEXT("Failed to save custom mixes!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Set the locale to UTF-8
    mixListFile.imbue(std::locale("en_US.UTF-8"));

    // Write BOM for UTF-8
    mixListFile << L'\uFEFF';

    for (const auto& mix : loadedMixes) {
        std::wostringstream line;
        line << mix.mixtureName << L"," // Save mixture name
            << mix.startingBase.name << L","; // Save starting base name

        // Save effects
        for (const auto& effect : mix.currentEffects) {
            line << effect.first << L";";
        }
        if (!mix.currentEffects.empty()) {
            line.seekp(-1, std::ios_base::cur); // Remove trailing semicolon
        }
        line << L",";

        // Save ingredients
        for (const auto& ingredient : mix.addedIngredients) {
            line << ingredient.name << L";";
        }
        if (!mix.addedIngredients.empty()) {
            line.seekp(-1, std::ios_base::cur); // Remove trailing semicolon
        }
        line << L",";

        // Save total cost at the end
        line << mix.totalCost;

        logDebug(L"Writing line: " + line.str());
        mixListFile << line.str() << L"\n";
    }

    mixListFile.close();
    isCustomMixesFileEmpty = false; // File is now populated
    lastAddedIngredient = nullptr; // Reset the last added ingredient
    updateButtonStates(hEditCustomButton, hCookbookButton); // Update button states after saving
    logSuccess(L"Custom mixes saved successfully.");
}
void saveCustomMixWithName(HWND hwnd, HWND& hEditCustomButton, HWND& hCookbookButton, HWND hCustomMixListView) {
    std::wstring mixName;

    // Show the name dialog
    if (DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_NAME_DIALOG), hwnd, NameDialogProc, reinterpret_cast<LPARAM>(&mixName)) == IDOK) {
        if (!mixName.empty()) {
            if (currentMixture) {
                currentMixture->mixtureName = mixName; // Store the mixture name
                customMixes.push_back(mixName);

                // Add the new mix to loadedMixes
                loadedMixes.push_back(*currentMixture);

                saveCustomMixes(hEditCustomButton, hCookbookButton);
                MessageBox(hwnd, TEXT("Mixture saved successfully!"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);

                // Refresh the ListView
                populateListView(hCustomMixListView, loadedMixes);

                // Clean up resources after saving
                cleanupResources();
            } else {
                MessageBox(hwnd, TEXT("No mixture to save!"), TEXT("Error"), MB_OK | MB_ICONERROR);
            }
        } else {
            MessageBox(hwnd, TEXT("Mix name cannot be empty!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        }
    }
}

// Function to update the selected custom base
void updateSelectedCustomBase(std::unique_ptr<DynamicBase> newBase) {
    logDebug(L"Updating selected custom base...");
    selectedCustomBase = std::move(newBase);
}

// Function to display mix details in the text area
void displayMixDetails(HWND detailsText, const DynamicBase& mix) {
    logDebug(L"Displaying mix details...");

    std::wostringstream oss;

    oss << "Starting Base: " << mix.startingBase.name << "\n";
    oss << "Base Effect: " << mix.startingBase.baseEffect << "\n"; // Include base effect
    oss << "Cost: $" << mix.totalCost << "\n";
    oss << "Effects:\n";
    for (const auto& effect : mix.currentEffects) {
        oss << "  - " << effect.first << "\n";
    }
    oss << "Ingredients:\n";
    for (const auto& ingredient : mix.addedIngredients) {
        oss << "  - " << ingredient.name << "\n";
    }

    std::string narrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oss.str());
    SetWindowTextA(detailsText, narrowString.c_str());
}

// Dialog procedure for the custom mix dialog
INT_PTR CALLBACK CustomMixDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hCustomMixListView, hSearchBar;
    static int dialogMode; // Store the context (MODE_EDIT or MODE_COOKBOOK)

    static bool darkModeApplied = false; // Ensure dark mode is applied only once
    if (!darkModeApplied) {
        EnableDarkMode(hDlg, isDarkModeEnabled, hLightBrush, hDarkBrush, hControlLightBrush, hControlDarkBrush);
        darkModeApplied = true;
    }

    switch (message) {
    case WM_INITDIALOG: {
        dialogMode = static_cast<int>(lParam); // Retrieve the context parameter

        // Initialize ListView
        RECT dlgRect;
        GetClientRect(hDlg, &dlgRect);

        hCustomMixListView = CreateWindow(WC_LISTVIEW, NULL,
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
            10, 40, dlgRect.right - 20, dlgRect.bottom - 90, // Adjust dimensions
            hDlg, (HMENU)IDC_CUSTOM_MIX_LIST, g_hInstance, NULL);
        if (!hCustomMixListView) {
            logDebug(L"Failed to create ListView.");
            return (INT_PTR)FALSE;
        }

        // Add columns to the ListView
        LVCOLUMN lvColumn = {};
        lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;

        // Add "Name" column
        lvColumn.pszText = const_cast<LPWSTR>(L"Name");
        lvColumn.cx = 100;
        ListView_InsertColumn(hCustomMixListView, 0, &lvColumn);

        // Add "Starting Base" column
        lvColumn.pszText = const_cast<LPWSTR>(L"Starting Base");
        lvColumn.cx = 100;
        ListView_InsertColumn(hCustomMixListView, 1, &lvColumn);

        // Add "Cost" column
        lvColumn.pszText = const_cast<LPWSTR>(L"Cost");
        lvColumn.cx = 80;
        ListView_InsertColumn(hCustomMixListView, 2, &lvColumn);

        // Add "Ingredients Added" column
        lvColumn.pszText = const_cast<LPWSTR>(L"Ingredients Added");
        lvColumn.cx = 150;
        ListView_InsertColumn(hCustomMixListView, 3, &lvColumn);

        // Add "Effects" column
        lvColumn.pszText = const_cast<LPWSTR>(L"Effects");
        lvColumn.cx = 150;
        ListView_InsertColumn(hCustomMixListView, 4, &lvColumn);

        // Initialize search bar
        hSearchBar = GetDlgItem(hDlg, IDC_SEARCH_BAR);
        if (!hSearchBar) {
            logDebug(L"Failed to retrieve search bar from resource.");
            return (INT_PTR)FALSE;
        }

        // Default sorting: Name Descending
        currentSortedColumn = 0; // Name column
        isAscending = false; // Descending order
        std::sort(loadedMixes.begin(), loadedMixes.end(), [](const DynamicBase& a, const DynamicBase& b) {
            return a.mixtureName > b.mixtureName; // Descending (A-Z)
            });

        // Populate the ListView with custom mixes
        populateListView(hCustomMixListView, loadedMixes);
        logDebug(L"ListView population finished.");

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case EN_CHANGE: { // Handle search bar input
            if ((HWND)lParam == hSearchBar) {
                wchar_t buffer[128]; // Define a wide-character buffer
                GetWindowTextW(hSearchBar, buffer, sizeof(buffer) / sizeof(buffer[0])); // Retrieve the text into the buffer

                // Directly assign the buffer to a wstring
                std::wstring searchQuery = buffer;

                // Split the query by commas for multiple criteria
                std::vector<std::wstring> criteria;
                std::wistringstream iss(searchQuery);
                std::wstring token;
                while (std::getline(iss, token, L',')) {
                    criteria.push_back(token);
                }

                // Filter the ListView based on the criteria
                std::vector<DynamicBase> filteredMixes;
                for (const auto& mix : loadedMixes) {
                    bool matches = false;

                    // Check if the name matches any criteria
                    for (const auto& criterion : criteria) {
                        if (mix.mixtureName.find(criterion) != std::wstring::npos) {
                            matches = true;
                            break;
                        }
                    }

                    // Check if any effect matches any criteria
                    for (const auto& effect : mix.currentEffects) {
                        for (const auto& criterion : criteria) {
                            if (effect.first.find(criterion) != std::wstring::npos) {
                                matches = true;
                                break;
                            }
                        }
                        if (matches) break;
                    }

                    if (matches) {
                        filteredMixes.push_back(mix);
                    }
                }

                populateListView(hCustomMixListView, filteredMixes);
            }
            break;
        }

        case IDOK: { // Handle ListView item selection
            int selectedIndex = ListView_GetNextItem(hCustomMixListView, -1, LVNI_SELECTED);
            if (selectedIndex != -1) {
                const DynamicBase& selectedMix = loadedMixes[selectedIndex];

                if (dialogMode == MODE_EDIT) {
                    // Set the selected mix as the current base for mixing
                    updateSelectedCustomBase(std::make_unique<DynamicBase>(selectedMix));
                }
            }
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }

    case WM_NOTIFY: { // Handle ListView column click for sorting
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        if (lpnmh->idFrom == IDC_CUSTOM_MIX_LIST && lpnmh->code == LVN_COLUMNCLICK) {
            NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
            int clickedColumn = pnmv->iSubItem;

            // Check if the clicked column is the same as the currently sorted column
            if (clickedColumn == currentSortedColumn) {
                // Toggle the sorting order
                isAscending = !isAscending;
            }
            else {
                // Switch to the new column, keep the current sorting order
                currentSortedColumn = clickedColumn;
            }

            // Sort the loadedMixes vector based on the current column and sorting order
            std::sort(loadedMixes.begin(), loadedMixes.end(), [clickedColumn](const DynamicBase& a, const DynamicBase& b) {
                switch (clickedColumn) {
                case 0: // Name
                    return isAscending ? a.mixtureName < b.mixtureName : a.mixtureName > b.mixtureName;
                case 1: // Starting Base
                    return isAscending ? a.startingBase.name < b.startingBase.name : a.startingBase.name > b.startingBase.name;
                case 2: // Cost
                    return isAscending ? a.totalCost < b.totalCost : a.totalCost > b.totalCost;
                case 3: // Ingredients Added
                    return isAscending ? a.addedIngredients.size() < b.addedIngredients.size() : a.addedIngredients.size() > b.addedIngredients.size();
                case 4: // Effects
                    return isAscending ? a.currentEffects.size() < b.currentEffects.size() : a.currentEffects.size() > b.currentEffects.size();
                default:
                    return true;
                }
                });

            // Repopulate the ListView
            populateListView(hCustomMixListView, loadedMixes);
        }
        break;
    }


    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)FALSE;
}

//Functions to handle all commands
void HandleBaseSelection(HWND hwnd, int commandId, HWND& hOutputArea) {
    logDebug(L"HandleBaseSelection called with commandId: " + std::to_wstring(commandId));
    std::unique_ptr<DynamicBase> selectedBase = nullptr;

    // Uncheck all radio buttons first
    SendMessage(GetDlgItem(hwnd, 101), BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(hwnd, 102), BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(hwnd, 103), BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(hwnd, 104), BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(hwnd, 105), BM_SETCHECK, BST_UNCHECKED, 0);

    // Check the selected radio button and set the base
    switch (commandId) {
    case 101:
        SendMessage(GetDlgItem(hwnd, 101), BM_SETCHECK, BST_CHECKED, 0);
        selectedBase = std::make_unique<DynamicBase>(Bases::ogKush);
        break;
    case 102:
        SendMessage(GetDlgItem(hwnd, 102), BM_SETCHECK, BST_CHECKED, 0);
        selectedBase = std::make_unique<DynamicBase>(Bases::sourDiesel);
        break;
    case 103:
        SendMessage(GetDlgItem(hwnd, 103), BM_SETCHECK, BST_CHECKED, 0);
        selectedBase = std::make_unique<DynamicBase>(Bases::greenCrack);
        break;
    case 104:
        SendMessage(GetDlgItem(hwnd, 104), BM_SETCHECK, BST_CHECKED, 0);
        selectedBase = std::make_unique<DynamicBase>(Bases::grandaddyPurple);
        break;
    case 105:
        if (selectedCustomBase) {
            SendMessage(GetDlgItem(hwnd, 105), BM_SETCHECK, BST_CHECKED, 0);
            selectedBase = std::make_unique<DynamicBase>(*selectedCustomBase);
            selectedBase->startingBase = selectedCustomBase->startingBase; // Carry over the starting base
        }
        else {
            MessageBox(hwnd, TEXT("No custom base available to edit!"), TEXT("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    case 107: // Blank Template
        SendMessage(GetDlgItem(hwnd, 107), BM_SETCHECK, BST_CHECKED, 0);
        selectedBase = std::make_unique<DynamicBase>(Bases::blankTemplate);
        break;
        break;
    }

    if (selectedBase) {
        currentBase = std::move(selectedBase);
        SetWindowTextA(hOutputArea, ""); // Clear the output area
        logDebug(L"Base selected: " + currentBase->startingBase.name);
        logSuccess(L"Ingredient added successfully.");
    }
}
void HandleEditCustomBase(HWND hwnd) {
    if (isCustomMixesFileEmpty) {
        showNoCustomMixesMessage(hwnd);
    }
    else {
        DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_CUSTOM_MIX_DIALOG), hwnd, CustomMixDialogProc, MODE_EDIT);
    }
    isCookbookDialogOpen = false;
}
void HandleMixButton(HWND hwnd, HWND& hIngredientDropdown, HWND& hOutputArea) {
    if (!currentBase) {
        MessageBox(hwnd, TEXT("No base selected! Please select a base before mixing ingredients."), TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    int selectedIndex = SendMessage(hIngredientDropdown, CB_GETCURSEL, 0, 0);
    if (selectedIndex == CB_ERR) {
        MessageBox(hwnd, TEXT("No ingredient selected!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    std::shared_ptr<const Ingredient> selectedIngredient = nullptr;
    switch (selectedIndex) {
    case 0: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::addy); break;
    case 1: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::banana); break;
    case 2: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::battery); break;
    case 3: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::chili); break;
    case 4: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::cuke); break;
    case 5: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::donut); break;
    case 6: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::energyDrink); break;
    case 7: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::fluMedicine); break;
    case 8: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::gasoline); break;
    case 9: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::horseSemen); break;
    case 10: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::iodine); break;
    case 11: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::megaBean); break;
    case 12: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::motorOil); break;
    case 13: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::mouthWash); break;
    case 14: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::paracetamol); break;
    case 15: selectedIngredient = std::make_shared<const Ingredient>(Ingredients::viagra); break;
    }

    if (selectedIngredient) {
        if (!currentMixture) {
            currentMixture = std::make_unique<DynamicBase>(*currentBase);
        }

        // Check for consecutive addition
        if (currentMixture->lastAddedIngredient && currentMixture->lastAddedIngredient->name == selectedIngredient->name) {
            MessageBox(hwnd, TEXT("You cannot add the same ingredient consecutively!"), TEXT("Error"), MB_OK | MB_ICONERROR);
            return;
        }

        // Call addIngredient with the correct arguments
        currentMixture->addIngredient(selectedIngredient, undoStack, redoStack, currentMixture);

        // Display the updated mixture details
        std::wostringstream oss;
        oss << "Starting Base: " << currentMixture->startingBase.name << "\r\n";
        oss << "Ingredients Added:\r\n";
        for (const auto& ingredient : currentMixture->addedIngredients) {
            oss << "  - " << ingredient.name << " ($" << ingredient.individualCost << ")\r\n";
        }
        oss << "Current Effects:\r\n";
        for (const auto& effect : currentMixture->currentEffects) {
            oss << "  - " << effect.first << "\r\n";
        }
        oss << "Total Cost: $" << currentMixture->totalCost << "\r\n";

        // Convert wide string to narrow string
        std::string narrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oss.str());
        SetWindowTextA(hOutputArea, narrowString.c_str());
    }
}
void HandleCookbookButton(HWND hwnd) {
    if (isCustomMixesFileEmpty) {
        showNoCustomMixesMessage(hwnd);
    }
    else {
        if (isCookbookDialogOpen) {
            logDebug(L"Cookbook dialog already open, ignoring...");
            return;
        }

        isCookbookDialogOpen = true;
        DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_CUSTOM_MIX_DIALOG), hwnd, CustomMixDialogProc, MODE_COOKBOOK);
        isCookbookDialogOpen = false;
    }
}

void HandleCommands(HWND hwnd, int commandId, HWND& hOutputArea, HWND& hIngredientDropdown, HWND& hEditCustomButton, HWND& hCookbookButton, HWND hCustomMixListView) {
    switch (commandId) {
    case 101: // OG Kush
    case 102: // Sour Diesel
    case 103: // Green Crack
    case 104: // Grandaddy Purple
    case 105: // Custom Base
        HandleBaseSelection(hwnd, commandId, hOutputArea);
        break;
    case 106: // Edit Custom Base
        if (isCustomMixesFileEmpty) {
            showNoCustomMixesMessage(hwnd);
        }
        else {
            HandleEditCustomBase(hwnd);
        }
        break;
	case 107: // Blank Template
		HandleBaseSelection(hwnd, commandId, hOutputArea);
		break;

    case ID_BUTTON_MIX: // Mix button
        HandleMixButton(hwnd, hIngredientDropdown, hOutputArea);
        break;

    case ID_BUTTON_SAVE: // Save button
        saveCustomMixWithName(hwnd, hEditCustomButton, hCookbookButton, hCustomMixListView);
        break;

    case ID_BUTTON_CLEAR: // Clear button
        logDebug(L"Clear button pressed. currentMixture is " + std::wstring(currentMixture ? L"not null" : L"null"));

        if (currentMixture) {
            int result = MessageBox(hwnd,
                TEXT("You have an unsaved mixture. Are you sure you want to clear it?"),
                TEXT("Warning"),
                MB_YESNO | MB_ICONWARNING);

            if (result == IDYES) {
                // Use reset to safely clear the smart pointers
                currentMixture.reset();
                currentBase.reset();

                logDebug(L"Current mixture and base cleared.");
                SetWindowTextA(hOutputArea, ""); // Clear the output area
            }
            else {
                logDebug(L"Clear operation canceled by the user.");
            }
        }
        else {
            MessageBox(hwnd, TEXT("No mixture to clear!"), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
        }
        break;

    case ID_BUTTON_COOKBOOK: // Cookbook button
        if (isCustomMixesFileEmpty) {
            showNoCustomMixesMessage(hwnd);
        }
        else {
            HandleCookbookButton(hwnd);
        }
        break;

    case ID_BUTTON_UNDO: {
        if (!undoStack.empty()) {
            // Move the current state to the redo stack
            redoStack.push(std::move(currentMixture));

            // Restore the last state from the undo stack
            currentMixture = std::move(undoStack.top());
            undoStack.pop();

            // Update the output area
            std::wostringstream oss;
            oss << "Starting Base: " << currentMixture->startingBase.name << "\r\n";
            oss << "Ingredients Added:\r\n";
            for (const auto& ingredient : currentMixture->addedIngredients) {
                oss << "  - " << ingredient.name << " ($" << ingredient.individualCost << ")\r\n";
            }
            oss << "Current Effects:\r\n";
            for (const auto& effect : currentMixture->currentEffects) {
                oss << "  - " << effect.first << "\r\n";
            }
            oss << "Total Cost: $" << currentMixture->totalCost << "\r\n";

            // Convert wide string to narrow string
            std::string narrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oss.str());
            SetWindowTextA(hOutputArea, narrowString.c_str());

            logSuccess(L"Undo performed.");
        }
        else {
            MessageBox(hwnd, TEXT("No actions to undo!"), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
        }
        break;
    }

    case ID_BUTTON_REDO: {
        if (!redoStack.empty()) {
            // Move the current state to the undo stack
            undoStack.push(std::move(currentMixture));

            // Restore the last state from the redo stack
            currentMixture = std::move(redoStack.top());
            redoStack.pop();

            // Update the output area
            std::wostringstream oss;
            oss << "Starting Base: " << currentMixture->startingBase.name << "\r\n";
            oss << "Ingredients Added:\r\n";
            for (const auto& ingredient : currentMixture->addedIngredients) {
                oss << "  - " << ingredient.name << " ($" << ingredient.individualCost << ")\r\n";
            }
            oss << "Current Effects:\r\n";
            for (const auto& effect : currentMixture->currentEffects) {
                oss << "  - " << effect.first << "\r\n";
            }
            oss << "Total Cost: $" << currentMixture->totalCost << "\r\n";

            // Convert wide string to narrow string
            std::string narrowString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(oss.str());
            SetWindowTextA(hOutputArea, narrowString.c_str());

            logSuccess(L"Redo performed.");
        }
        else {
            MessageBox(hwnd, TEXT("No actions to redo!"), TEXT("Info"), MB_OK | MB_ICONINFORMATION);
        }
        break;
    }

    case IDM_TOGGLE_DARK_MODE: {
        isDarkModeEnabled = !isDarkModeEnabled; // Toggle the dark mode state
        EnableDarkMode(hwnd, isDarkModeEnabled, hLightBrush, hDarkBrush, hControlLightBrush, hControlDarkBrush);
        MessageBox(hwnd, isDarkModeEnabled ? TEXT("Dark mode enabled.") : TEXT("Dark mode disabled."),
            TEXT("Dark Mode"), MB_OK | MB_ICONINFORMATION);
        break;
    }

    case IDM_IMPORT_COOKBOOK: { // Import button/menu
        OPENFILENAME ofn = {};
        wchar_t fileName[MAX_PATH] = L""; // Use wide-character array

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0"; // Wide-character string
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn)) {
            // Convert wide-character fileName to std::string for importCookbook
            std::wstring wsFileName(fileName);
            std::wstring filePath(wsFileName.begin(), wsFileName.end());
            importCookbook(filePath); // Pass the converted file path
            populateListView(hCustomMixListView, loadedMixes); // Refresh ListView
        }
        break;
    }

    case IDM_EXPORT_COOKBOOK: { // Export button/menu
        OPENFILENAME ofn = {};
        wchar_t fileName[MAX_PATH] = L""; // Use wide-character array

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0"; // Wide-character string
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;

        if (GetSaveFileName(&ofn)) {
            // Convert wide-character fileName to std::string for exportCookbook
            std::wstring wsFileName(fileName);
            std::wstring filePath(wsFileName.begin(), wsFileName.end());
            exportCookbook(filePath); // Pass the converted file path
        }
        break;
    }

    default:
        logDebug(L"Unhandled WM_COMMAND. Command ID: " + std::to_wstring(commandId));
        break;
    }


}

LRESULT CALLBACK ComboBoxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
    case WM_PAINT: {
        // Call the default painting procedure to ensure the arrow and outline are drawn
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_DESTROY: {
        // Remove the subclass when the ComboBox is destroyed
        RemoveWindowSubclass(hwnd, ComboBoxSubclassProc, uIdSubclass);
        break;
    }
    default:
        break;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

bool initializeUIComponents(HWND hwnd, HWND& hIngredientDropdown, HWND& hMixButton, HWND& hCookbookButton, HWND& hSaveButton,
	HWND& hOutputArea, HWND& hRadioButton1, HWND& hRadioButton2, HWND& hRadioButton3, HWND& hRadioButton4, HWND& hRadioButton5, HWND& hRadioButton6,
	HWND& hEditCustomButton, HWND& hClearButton, HWND& hUndoButton, HWND& hRedoButton, HWND& hLogOutputArea) {
    // Create a GroupBox for base selection
    HWND hGroupBox = CreateWindow(TEXT("BUTTON"), TEXT("Select Base"),WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        5, 5, 210, 235, hwnd, NULL, NULL, NULL);
    if (!hGroupBox) {
		logDebug(L"Failed to create GroupBox");
        return false;
    }
    // Create RadioButtons for each base
    hRadioButton1 = CreateWindow(TEXT("BUTTON"), TEXT("OG Kush"),WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_GROUP,
        20, 30, 180, 20, hwnd, (HMENU)101, NULL, NULL);
    if (!hRadioButton1) {
        logDebug(L"Failed to create OG Kush");
        return false;
    }
    hRadioButton2 = CreateWindow(TEXT("BUTTON"), TEXT("Sour Diesel"),WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
        20, 60, 180, 20, hwnd, (HMENU)102, NULL, NULL);
	if (!hRadioButton2) {
		logDebug(L"Failed to create Sour Diesel");
		return false;
	}
    hRadioButton3 = CreateWindow(TEXT("BUTTON"), TEXT("Green Crack"),WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
        20, 90, 180, 20, hwnd, (HMENU)103, NULL, NULL);
	if (!hRadioButton3) {
		logDebug(L"Failed to create Green Crack");
		return false;
	}
    hRadioButton4 = CreateWindow(TEXT("BUTTON"), TEXT("Grandaddy Purple"),WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
        20, 120, 180, 20, hwnd, (HMENU)104, NULL, NULL);
	if (!hRadioButton4) {
		logDebug(L"Failed to create Grandaddy Purple");
		return false;
	}
    hRadioButton5 = CreateWindow(TEXT("BUTTON"), TEXT("Blank Template"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
        20, 150, 180, 20, hwnd, (HMENU)107, NULL, NULL);
    if (!hRadioButton5) {
        logDebug(L"Failed to create Blank Template radio button");
        return false;
    }
    hRadioButton6 = CreateWindow(TEXT("BUTTON"), TEXT("Custom Base"),WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
        20, 180, 180, 20, hwnd, (HMENU)105, NULL, NULL);
	if (!hRadioButton6) {
		logDebug(L"Failed to create Custom Base");
		return false;
	}
	hEditCustomButton = CreateWindow(TEXT("BUTTON"), TEXT("Edit Custom Base"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
		20, 210, 180, 20, hwnd, (HMENU)106, NULL, NULL);
	if (!hEditCustomButton) {
		logDebug(L"Failed to create Edit Custom Base button");
		return false;
	}

    // Create a dropdown for selecting the Ingredient
    hIngredientDropdown = CreateWindow(
        TEXT("COMBOBOX"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL,
        10, 250, 210, 200, hwnd, (HMENU)IDC_INGREDIENT_LIST, NULL, NULL
    );
    if (!hIngredientDropdown) {
        logDebug(L"Failed to create Ingredient Dropdown");
        return false;
    }
	logDebug(L"Ingredient Dropdown created successfully.");
	if (SetWindowSubclass(hIngredientDropdown, ComboBoxSubclassProc, 1, 0) == NULL) {
		logDebug(L"Failed to set subclass for Ingredient Dropdown");
		return false;
	}

    // Populate the dropdown dynamically
    for (size_t i = 0; i < allIngredientsSize; ++i) {
        SendMessage(hIngredientDropdown, CB_ADDSTRING, 0, (LPARAM)allIngredients[i].name.c_str());
    }

    // Create buttons Outside of the Groupbox
    hMixButton = CreateWindow(TEXT("BUTTON"), TEXT("Mix"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        300, 75, 150, 30, hwnd, (HMENU)ID_BUTTON_MIX, NULL, NULL);
	if (!hMixButton) {
		logDebug(L"Failed to create Mix button");
		return false;
	}
    hSaveButton = CreateWindow(TEXT("BUTTON"), TEXT("Save Mixture as Base"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        300, 125, 150, 30, hwnd, (HMENU)ID_BUTTON_SAVE, NULL, NULL);
	if (!hSaveButton) {
		logDebug(L"Failed to create Save button");
		return false;
	}
    hCookbookButton = CreateWindow(TEXT("BUTTON"), TEXT("'Cook'book"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        300, 25, 150, 30, hwnd, (HMENU)ID_BUTTON_COOKBOOK, NULL, NULL);
	if (!hCookbookButton) {
		logDebug(L"Failed to create Cookbook button");
		return false;
	}
    hClearButton = CreateWindow(TEXT("BUTTON"), TEXT("Clear"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        560, 425, 150, 30, hwnd, (HMENU)ID_BUTTON_CLEAR, NULL, NULL);
	if (!hClearButton) {
		logDebug(L"Failed to create Clear button");
		return false;
	}
    hUndoButton = CreateWindow(TEXT("BUTTON"), TEXT("Undo"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        485, 460, 150, 30, hwnd, (HMENU)ID_BUTTON_UNDO, NULL, NULL);
	if (!hUndoButton) {
		logDebug(L"Failed to create Undo button");
		return false;
	}
    hRedoButton = CreateWindow(TEXT("BUTTON"), TEXT("Redo"),WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        640, 460, 150, 30, hwnd, (HMENU)ID_BUTTON_REDO, NULL, NULL);
	if (!hRedoButton) {
		logDebug(L"Failed to create Redo button");
		return false;
	}
    // Create an output area for displaying the mixture
    hOutputArea = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
        490, 10, 300, 400, hwnd, NULL, NULL, NULL);
    if (!hOutputArea) {
		logDebug(L"Failed to create Output Area");
        return false;
	}
    // Create an output area for logging success/failure messages
    hLogOutputArea = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
        10, 500, 780, 80, hwnd, NULL, NULL, NULL); // Positioned at the bottom
    if (!hLogOutputArea) {
        logDebug(L"Failed to create Log Output Area");
        return false;
    }

    SendMessage(hRadioButton1, BM_SETCHECK, BST_CHECKED, 0); // Set the first radio button as checked

    return true;
}


//              WINDOW CODE STARTS HERE
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hIngredientDropdown, hMixButton, hCookbookButton, hSaveButton, hOutputArea, hUndoButton, hRedoButton, hCustomMixListView;
    static HWND hRadioButton1, hRadioButton2, hRadioButton3, hRadioButton4, hRadioButton5, hRadioButton6, hEditCustomButton, hClearButton, hLogOutputArea;

    switch (uMsg) {
    case WM_CREATE: {
        if (!initializeUIComponents(hwnd, hIngredientDropdown, hMixButton, hCookbookButton, hSaveButton, hOutputArea,
			hRadioButton1, hRadioButton2, hRadioButton3, hRadioButton4, hRadioButton5, hRadioButton6,
			hEditCustomButton, hClearButton, hUndoButton, hRedoButton, hLogOutputArea)) {
            logDebug(L"Failed to initialize UI components.");
            return -1; // Abort window creation
        }

        // Ensure the first base is selected by default
        HandleBaseSelection(hwnd, 101, hOutputArea);

        logDebug(L"UI components initialized successfully.");
        return 0;
    }
    
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;

        if (lpDrawItem->CtlType == ODT_BUTTON) {
            // Determine if the button is pressed
            bool isPressed = (lpDrawItem->itemState & ODS_SELECTED) != 0;

            // Set the background color
            HBRUSH hBrush = isDarkModeEnabled ? hControlDarkBrush : hControlLightBrush;
            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);

            // Draw the border
            HPEN hPen = CreatePen(PS_SOLID, 1, isPressed ? RGB(0, 0, 0) : RGB(192, 192, 192)); // Darker for pressed
            HPEN hOldPen = (HPEN)SelectObject(lpDrawItem->hDC, hPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(lpDrawItem->hDC, GetStockObject(NULL_BRUSH));

            Rectangle(lpDrawItem->hDC, lpDrawItem->rcItem.left, lpDrawItem->rcItem.top,
                lpDrawItem->rcItem.right, lpDrawItem->rcItem.bottom);

            // Restore the old pen and brush
            SelectObject(lpDrawItem->hDC, hOldPen);
            SelectObject(lpDrawItem->hDC, hOldBrush);
            DeleteObject(hPen);

            // Set the text color
            SetTextColor(lpDrawItem->hDC, isDarkModeEnabled ? RGB(255, 255, 255) : RGB(0, 0, 0));
            SetBkMode(lpDrawItem->hDC, TRANSPARENT);

            // Retrieve the button's text
            TCHAR buttonText[128];
            GetWindowText(lpDrawItem->hwndItem, buttonText, sizeof(buttonText) / sizeof(TCHAR));

            // Adjust text position if the button is pressed
            RECT textRect = lpDrawItem->rcItem;
            if (isPressed) {
                OffsetRect(&textRect, 1, 1); // Shift text slightly down and right
            }

            // Draw the button text
            DrawText(lpDrawItem->hDC, buttonText, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            return TRUE;
        }
        break;
    }

    case IDM_TOGGLE_DARK_MODE: {
        isDarkModeEnabled = !isDarkModeEnabled; // Toggle the dark mode state
        EnableDarkMode(hwnd, isDarkModeEnabled, hLightBrush, hDarkBrush, hControlLightBrush, hControlDarkBrush);
        MessageBox(hwnd, isDarkModeEnabled ? TEXT("Dark mode enabled.") : TEXT("Dark mode disabled."),
            TEXT("Dark Mode"), MB_OK | MB_ICONINFORMATION);
        break;
    }
    
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;

        if (isDarkModeEnabled) {
            SetTextColor(hdcEdit, RGB(255, 255, 255)); // White text
            SetBkColor(hdcEdit, RGB(37, 37, 38));      // Darker gray background
            return (INT_PTR)hDarkBrush;               // Return dark brush for background
        }
        else {
            SetTextColor(hdcEdit, RGB(0, 0, 0));      // Black text
            SetBkColor(hdcEdit, RGB(255, 255, 255));  // White background
            return (INT_PTR)hLightBrush;             // Return light brush for background
        }
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;

        if (isDarkModeEnabled) {
            SetTextColor(hdcStatic, RGB(255, 255, 255)); // White text
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR)hDarkBrush;               // Return dark brush for background
        }
        else {
            SetTextColor(hdcStatic, RGB(0, 0, 0));    // Black text
            SetBkMode(hdcStatic, TRANSPARENT);
            return (INT_PTR)hLightBrush;             // Return light brush for background
        }
    }
    case WM_CTLCOLORLISTBOX: {
        HDC hdcListBox = (HDC)wParam;

        if (isDarkModeEnabled) {
            SetTextColor(hdcListBox, RGB(255, 255, 255)); // White text
            SetBkColor(hdcListBox, RGB(62, 62, 66));      // Lighter gray background
            return (INT_PTR)hControlDarkBrush;           // Return dark brush for background
        }
        else {
            SetTextColor(hdcListBox, RGB(0, 0, 0));      // Black text
            SetBkColor(hdcListBox, RGB(255, 255, 255));  // White background
            return (INT_PTR)hControlLightBrush;         // Return light brush for background
        }
    }
    case WM_COMMAND: {
        HandleCommands(hwnd, LOWORD(wParam), hOutputArea, hIngredientDropdown, hEditCustomButton, hCookbookButton, hCustomMixListView);
        return 0;
    }
    case WM_CLOSE: {
        if (currentMixture) {
            int result = MessageBox(hwnd,
                TEXT("You have an unsaved mixture. Are you sure you want to exit?"),
                TEXT("Warning"),
                MB_YESNO | MB_ICONWARNING);

            if (result != IDYES) {
                logDebug(L"Exit operation canceled by the user.");
                return 0; // Cancel close
            }
        }
        logDebug(L"WM_CLOSE received.");
        cleanupResources();
        DestroyWindow(hwnd);
        return 0;
    }
    case WM_DESTROY: {
        logDebug(L"WM_DESTROY received.");
        DeleteObject(hDarkBrush);
        DeleteObject(hControlDarkBrush);
        cleanupResources();
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Set the global locale to English (US)
    std::locale::global(std::locale("en_US.UTF-8"));
    g_hInstance = hInstance; // Assign the instance handle to the global variable

    // Set the working directory to the executable's directory
    SetWorkingDirectoryToExecutablePath();

    // Load custom mixes from file
    logDebug(L"Application Started");
    loadedMixes = loadCustomMixes();
    loadPreferences();

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES; // Initialize ListView classes
    InitCommonControlsEx(&icex);

    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = g_hInstance; // Use g_hInstance here
    wc.lpszClassName = TEXT("Schedule1MixerClass");
    logDebug(L"RegisterClass succeeded.");

    if (!RegisterClass(&wc)) {
        DWORD error = GetLastError();
        if (error == ERROR_CLASS_ALREADY_EXISTS) {
            logDebug(L"Window class already exists.");
        }
        else {
            std::wostringstream oss;
            oss << "Failed to register window class! Error code: " << error;
            logDebug(oss.str());
        }
        return 0;
    }

    // Load the menu resource
	HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_SCHEDULE1MIXER_MENU));
    if (!hMenu) {
        MessageBox(NULL, TEXT("Failed to load menu!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCHEDULE1MIXER));
    if (!hAccel) {
        MessageBox(NULL, TEXT("Failed to load accelerators!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Create the main application window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        wc.lpszClassName,               // Window class name
        TEXT("Schedule1 Mixer"),        // Window title
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Non-resizable window
        CW_USEDEFAULT, CW_USEDEFAULT,   // Position
        815, 650,                       // Size
        NULL,                           // Parent window
        NULL,                           // Menu
        g_hInstance,                    // Use g_hInstance here
        NULL                            // Additional application data
    );
    logDebug(L"CreateWindowEx succeeded.");

    if (!hwnd) {
        DWORD error = GetLastError();
        std::wostringstream oss;
        oss << "Failed to create window! Error code: " << error;
        logDebug(oss.str());
        MessageBox(NULL, TEXT("Failed to create window!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Set the menu to the main window
    if (!SetMenu(hwnd, hMenu)) {
        MessageBox(NULL, TEXT("Failed to set menu!"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return 0;
    }

    // Show the window
    logDebug(L"nCmdShow value: " + std::to_wstring(nCmdShow));
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd); // Ensure the window is redrawn

    logDebug(L"Entering message loop.");
    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    logDebug(L"Exiting message loop.");

    // Unregister the window class
    UnregisterClass(TEXT("Schedule1MixerClass"), g_hInstance);

    return (int)msg.wParam;
}