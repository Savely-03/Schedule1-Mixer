#pragma once
#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <map>
#include <vector>

// Effect class
class Effect {
public:
    std::wstring name;

    Effect(const std::wstring& name);
    bool operator==(const Effect& other) const;
};

// Ingredient class
class Ingredient {
public:
    std::wstring name;
    double individualCost;
    double stackCost;
    std::wstring baseEffect;
    std::map<std::wstring, std::wstring> replaceableEffects;
    static const int STACK_SIZE = 20;

    Ingredient(const std::wstring& name, double individualCost, const std::wstring& baseEffect,
        const std::map<std::wstring, std::wstring>& replaceableEffects);
};

// Base class
class Base {
public:
    std::wstring name;
    double individualCost;
    double stackCost;
    std::wstring baseEffect;
    static const int STACK_SIZE = 10;
    Base(const std::wstring& name, double individualCost, const std::wstring& baseEffect);
};

#endif // MODELS_H