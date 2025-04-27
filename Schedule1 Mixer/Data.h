#pragma once
#ifndef DATA_H
#define DATA_H

#include "Models.h"
#include <vector>

// Effects class
class Effects {
public:
    static const Effect antiGravity;
    static const Effect athletic;
    static const Effect balding;
    static const Effect brightEyed;
    static const Effect calming;
    static const Effect calorieDense;
    static const Effect cyclopean;
    static const Effect disorienting;
    static const Effect electrifying;
    static const Effect energizing;
    static const Effect euphoric;
    static const Effect explosive;
    static const Effect focused;
    static const Effect foggy;
    static const Effect gingeritis;
    static const Effect glowing;
    static const Effect jennerising;
    static const Effect laxative;
    static const Effect lethal;
    static const Effect longFaced;
    static const Effect munchies;
    static const Effect paranoia;
    static const Effect refreshing;
    static const Effect schizophrenic;
    static const Effect sedating;
    static const Effect seizure;
    static const Effect shrinking;
    static const Effect slippery;
    static const Effect smelly;
    static const Effect sneaky;
    static const Effect spicy;
    static const Effect thoughtProvoking;
    static const Effect toxic;
    static const Effect tropicThunder;
    static const Effect zombifying;
};

// Ingredients class
class Ingredients {
public:
    static const Ingredient addy;
    static const Ingredient banana;
    static const Ingredient battery;
    static const Ingredient chili;
    static const Ingredient cuke;
    static const Ingredient donut;
    static const Ingredient energyDrink;
    static const Ingredient fluMedicine;
    static const Ingredient gasoline;
    static const Ingredient horseSemen;
    static const Ingredient iodine;
    static const Ingredient megaBean;
    static const Ingredient motorOil;
    static const Ingredient mouthWash;
    static const Ingredient paracetamol;
    static const Ingredient viagra;
};

// Bases class
class Bases {
public:
    static const Base ogKush;
    static const Base sourDiesel;
    static const Base greenCrack;
    static const Base grandaddyPurple;
    static const Base blankTemplate;
};

// Arrays of all effects, ingredients, and bases
extern const Effect allEffects[];
extern const size_t allEffectsSize;

extern const Ingredient allIngredients[];
extern const size_t allIngredientsSize;

extern const Base allBases[];
extern const size_t allBasesSize;

#endif // DATA_H