#include "Mixing.h"
#include "Logging.h" // For logDebug
#include <iostream>

// DynamicBase class implementation
DynamicBase::DynamicBase(const Base& base)
    : startingBase(base), totalCost(base.individualCost) {
    // Start with the base's initial effect
    currentEffects[base.baseEffect] = base.baseEffect;
}

void DynamicBase::addIngredient(
    const std::shared_ptr<const Ingredient>& ingredient,
    std::stack<std::unique_ptr<DynamicBase>>& undoStack,
    std::stack<std::unique_ptr<DynamicBase>>& redoStack,
    std::unique_ptr<DynamicBase>& currentMixture
) {
    if (!ingredient) return;

    // Prevent consecutive additions of the same ingredient
    if (lastAddedIngredient && lastAddedIngredient->name == ingredient->name) {
        logDebug(L"Ingredient " + ingredient->name + L" cannot be added consecutively.");
        return;
    }

    // Push the current state to the undo stack
    if (currentMixture) {
        undoStack.push(std::make_unique<DynamicBase>(*currentMixture));
    }

    // Clear the redo stack on a new action
    while (!redoStack.empty()) redoStack.pop();

    addedIngredients.push_back(*ingredient);
    totalCost += ingredient->individualCost;

    // Add the base's base-effect if not already present
    if (currentEffects.find(startingBase.baseEffect) == currentEffects.end()) {
        currentEffects[startingBase.baseEffect] = startingBase.baseEffect;
    }

    // Check for replacements using the ingredient's replaceableEffects map
    for (auto& effect : currentEffects) {
        auto it = ingredient->replaceableEffects.find(effect.first);
        if (it != ingredient->replaceableEffects.end()) {
            effect.second = it->second;
        }
    }

    // Add the ingredient's base-effect
    currentEffects[ingredient->baseEffect] = ingredient->baseEffect;

    // Update the last added ingredient
    lastAddedIngredient = ingredient;
}

// Global variables related to mixing
std::unique_ptr<DynamicBase> currentBase = nullptr;
std::shared_ptr<const Ingredient> currentIngredient = nullptr;
std::unique_ptr<DynamicBase> selectedCustomBase = nullptr;
std::vector<DynamicBase> loadedMixes;
std::unique_ptr<DynamicBase> currentMixture = nullptr;
std::unique_ptr<DynamicBase> backupMixture = nullptr;
std::stack<std::unique_ptr<DynamicBase>> undoStack;
std::stack<std::unique_ptr<DynamicBase>> redoStack;
std::shared_ptr<const Ingredient> lastAddedIngredient = nullptr;