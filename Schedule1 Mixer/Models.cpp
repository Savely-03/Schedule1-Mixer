#include "Models.h"

// Effect class implementation
Effect::Effect(const std::wstring& name) : name(name) {}

bool Effect::operator==(const Effect& other) const {
    return name == other.name;
}

// Ingredient class implementation
Ingredient::Ingredient(const std::wstring& name, double individualCost, const std::wstring& baseEffect,
    const std::map<std::wstring, std::wstring>& replaceableEffects)
    : name(name),
    individualCost(individualCost),
    stackCost(individualCost* STACK_SIZE),
    baseEffect(baseEffect),
    replaceableEffects(replaceableEffects){}

// Base class implementation
Base::Base(const std::wstring& name, double individualCost, const std::wstring& baseEffect)
    : name(name),
    individualCost(individualCost),
    stackCost(individualCost* STACK_SIZE),
    baseEffect(baseEffect) {}