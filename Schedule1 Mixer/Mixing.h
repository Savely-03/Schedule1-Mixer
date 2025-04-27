#pragma once
#ifndef MIXING_H
#define MIXING_H

#include "Models.h"
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <string>

// DynamicBase class
class DynamicBase {
public:
    std::wstring mixtureName; // Name of the mixture
    Base startingBase;       // The initial base
    std::vector<Ingredient> addedIngredients; // List of added ingredients, in order
    std::map<std::wstring, std::wstring> currentEffects; // Current effects in the mixture
    double totalCost;        // Total cost of the mixture
    std::shared_ptr<const Ingredient> lastAddedIngredient; // Track the last added ingredient

    // Constructor to initialize with a base
    DynamicBase(const Base& base);

    // Method to add an ingredient to the mixture
    void addIngredient(
        const std::shared_ptr<const Ingredient>& ingredient,
        std::stack<std::unique_ptr<DynamicBase>>& undoStack,
        std::stack<std::unique_ptr<DynamicBase>>& redoStack,
        std::unique_ptr<DynamicBase>& currentMixture
    );
};

// Global variables related to mixing
extern std::unique_ptr<DynamicBase> currentBase;
extern std::shared_ptr<const Ingredient> currentIngredient;
extern std::unique_ptr<DynamicBase> selectedCustomBase;
extern std::vector<DynamicBase> loadedMixes;
extern std::unique_ptr<DynamicBase> currentMixture;
extern std::unique_ptr<DynamicBase> backupMixture;
extern std::stack<std::unique_ptr<DynamicBase>> undoStack;
extern std::stack<std::unique_ptr<DynamicBase>> redoStack;
extern std::shared_ptr<const Ingredient> lastAddedIngredient;

// Global flags
extern bool isDarkModeEnabled; // Flag for dark mode
extern bool isCustomMixesFileEmpty; // Flag for custom mixes file state

#endif // MIXING_H