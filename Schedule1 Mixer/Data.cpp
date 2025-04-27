#include "Data.h"

// Effects class implementation
const Effect Effects::antiGravity(L"Anti-Gravity");
const Effect Effects::athletic(L"Athletic");
const Effect Effects::balding(L"Balding");
const Effect Effects::brightEyed(L"Bright-Eyed");
const Effect Effects::calming(L"Calming");
const Effect Effects::calorieDense(L"Calorie-Dense");
const Effect Effects::cyclopean(L"Cyclopean");
const Effect Effects::disorienting(L"Disorienting");
const Effect Effects::electrifying(L"Electrifying");
const Effect Effects::energizing(L"Energizing");
const Effect Effects::euphoric(L"Euphoric");
const Effect Effects::explosive(L"Explosive");
const Effect Effects::focused(L"Focused");
const Effect Effects::foggy(L"Foggy");
const Effect Effects::gingeritis(L"Gingeritis");
const Effect Effects::glowing(L"Glowing");
const Effect Effects::jennerising(L"Jennerising");
const Effect Effects::laxative(L"Laxative");
const Effect Effects::lethal(L"Lethal");
const Effect Effects::longFaced(L"Long-Faced");
const Effect Effects::munchies(L"Munchies");
const Effect Effects::paranoia(L"Paranoia");
const Effect Effects::refreshing(L"Refreshing");
const Effect Effects::schizophrenic(L"Schizophrenic");
const Effect Effects::sedating(L"Sedating");
const Effect Effects::seizure(L"Seizure-Inducing");
const Effect Effects::shrinking(L"Shrinking");
const Effect Effects::slippery(L"Slippery");
const Effect Effects::smelly(L"Smelly");
const Effect Effects::sneaky(L"Sneaky");
const Effect Effects::spicy(L"Spicy");
const Effect Effects::thoughtProvoking(L"Thought-Provoking");
const Effect Effects::toxic(L"Toxic");
const Effect Effects::tropicThunder(L"Tropic Thunder");
const Effect Effects::zombifying(L"Zombifying");

// Array of all effects
const Effect allEffects[] = {
    Effects::antiGravity,
    Effects::athletic,
    Effects::balding,
    Effects::brightEyed,
    Effects::calming,
    Effects::calorieDense,
    Effects::cyclopean,
    Effects::disorienting,
    Effects::electrifying,
    Effects::energizing,
    Effects::euphoric,
    Effects::explosive,
    Effects::focused,
    Effects::foggy,
    Effects::gingeritis,
    Effects::glowing,
    Effects::jennerising,
    Effects::laxative,
    Effects::lethal,
    Effects::longFaced,
    Effects::munchies,
    Effects::paranoia,
    Effects::refreshing,
    Effects::schizophrenic,
    Effects::sedating,
    Effects::seizure,
    Effects::shrinking,
    Effects::slippery,
    Effects::smelly,
    Effects::sneaky,
    Effects::spicy,
    Effects::thoughtProvoking,
    Effects::toxic,
    Effects::tropicThunder,
    Effects::zombifying
};
constexpr size_t allEffectsSize = sizeof(allEffects) / sizeof(allEffects[0]);

// Ingredients class implementation
const Ingredient Ingredients::addy(
    L"Addy", 9.0, Effects::thoughtProvoking.name,
    {
        {Effects::explosive.name, Effects::euphoric.name},
        {Effects::foggy.name, Effects::energizing.name},
        {Effects::glowing.name, Effects::refreshing.name},
        {Effects::longFaced.name, Effects::electrifying.name},
        {Effects::sedating.name, Effects::gingeritis.name}
    });
const Ingredient Ingredients::banana(
    L"Banana", 2.0, Effects::gingeritis.name,
    {
        {Effects::calming.name, Effects::sneaky.name},
        {Effects::cyclopean.name, Effects::thoughtProvoking.name},
        {Effects::disorienting.name, Effects::focused.name},
        {Effects::energizing.name, Effects::thoughtProvoking.name},
        {Effects::focused.name, Effects::seizure.name},
        {Effects::longFaced.name, Effects::refreshing.name},
        {Effects::paranoia.name, Effects::jennerising.name},
        {Effects::smelly.name, Effects::antiGravity.name},
        {Effects::toxic.name, Effects::smelly.name}
    });
const Ingredient Ingredients::battery(
    L"Battery", 8.0, Effects::brightEyed.name,
    {
        {Effects::cyclopean.name, Effects::glowing.name},
        {Effects::electrifying.name, Effects::euphoric.name},
        {Effects::euphoric.name, Effects::zombifying.name},
        {Effects::laxative.name, Effects::calorieDense.name},
        {Effects::munchies.name, Effects::tropicThunder.name},
        {Effects::shrinking.name, Effects::munchies.name}
    });
const Ingredient Ingredients::chili(
    L"Chili", 7.0, Effects::spicy.name,
    {
        {Effects::antiGravity.name, Effects::tropicThunder.name},
        {Effects::athletic.name, Effects::longFaced.name},
        {Effects::laxative.name, Effects::longFaced.name},
        {Effects::munchies.name, Effects::toxic.name},
        {Effects::shrinking.name, Effects::refreshing.name},
        {Effects::sneaky.name, Effects::brightEyed.name}
    });
const Ingredient Ingredients::cuke(
    L"Cuke", 2.0, Effects::energizing.name,
    {
        {Effects::euphoric.name, Effects::laxative.name},
        {Effects::foggy.name, Effects::cyclopean.name},
        {Effects::gingeritis.name, Effects::thoughtProvoking.name},
        {Effects::munchies.name, Effects::athletic.name},
        {Effects::slippery.name, Effects::munchies.name},
        {Effects::sneaky.name, Effects::paranoia.name},
        {Effects::toxic.name, Effects::euphoric.name}
    });
const Ingredient Ingredients::donut(
    L"Donut", 3.0, Effects::calorieDense.name,
    {
        {Effects::antiGravity.name, Effects::slippery.name},
        {Effects::balding.name, Effects::sneaky.name},
        {Effects::calorieDense.name, Effects::explosive.name},
        {Effects::focused.name, Effects::euphoric.name},
        {Effects::jennerising.name, Effects::gingeritis.name},
        {Effects::munchies.name, Effects::calming.name},
        {Effects::shrinking.name, Effects::energizing.name}
    });
const Ingredient Ingredients::energyDrink(
    L"Energy Drink", 6.0, Effects::athletic.name,
    {
        {Effects::disorienting.name, Effects::electrifying.name},
        {Effects::euphoric.name, Effects::energizing.name},
        {Effects::focused.name, Effects::shrinking.name},
        {Effects::foggy.name, Effects::laxative.name},
        {Effects::glowing.name, Effects::disorienting.name},
        {Effects::schizophrenic.name, Effects::balding.name},
        {Effects::sedating.name, Effects::munchies.name},
        {Effects::spicy.name, Effects::euphoric.name},
        {Effects::tropicThunder.name, Effects::sneaky.name}
    });
const Ingredient Ingredients::fluMedicine(
    L"Flu Medicine", 5.0, Effects::sedating.name,
    {
        {Effects::athletic.name, Effects::munchies.name},
        {Effects::calming.name, Effects::brightEyed.name},
        {Effects::cyclopean.name, Effects::foggy.name},
        {Effects::electrifying.name, Effects::refreshing.name},
        {Effects::euphoric.name, Effects::toxic.name},
        {Effects::focused.name, Effects::calming.name},
        {Effects::laxative.name, Effects::euphoric.name},
        {Effects::munchies.name, Effects::slippery.name},
        {Effects::shrinking.name, Effects::paranoia.name},
        {Effects::thoughtProvoking.name, Effects::gingeritis.name}
    });
const Ingredient Ingredients::gasoline(
    L"Gasoline", 5.0, Effects::toxic.name,
    {
        {Effects::disorienting.name, Effects::glowing.name},
        {Effects::electrifying.name, Effects::disorienting.name},
        {Effects::energizing.name, Effects::spicy.name},
        {Effects::euphoric.name, Effects::energizing.name},
        {Effects::gingeritis.name, Effects::smelly.name},
        {Effects::jennerising.name, Effects::sneaky.name},
        {Effects::laxative.name, Effects::foggy.name},
        {Effects::munchies.name, Effects::sedating.name},
        {Effects::paranoia.name, Effects::calming.name},
        {Effects::shrinking.name, Effects::focused.name},
        {Effects::sneaky.name, Effects::tropicThunder.name}
    });
const Ingredient Ingredients::horseSemen(
    L"Horse Semen", 9.0, Effects::longFaced.name,
    {
        {Effects::antiGravity.name, Effects::calming.name},
        {Effects::gingeritis.name, Effects::refreshing.name},
        {Effects::thoughtProvoking.name, Effects::electrifying.name}
    });
const Ingredient Ingredients::iodine(
    L"Iodine", 8.0, Effects::jennerising.name,
    {
        {Effects::calming.name, Effects::balding.name},
        {Effects::calorieDense.name, Effects::gingeritis.name},
        {Effects::euphoric.name, Effects::seizure.name},
        {Effects::foggy.name, Effects::paranoia.name},
        {Effects::refreshing.name, Effects::thoughtProvoking.name},
        {Effects::toxic.name, Effects::sneaky.name}
    });
const Ingredient Ingredients::megaBean(
    L"Mega Bean", 7.0, Effects::foggy.name,
    {
        {Effects::athletic.name, Effects::laxative.name},
        {Effects::calming.name, Effects::glowing.name},
        {Effects::energizing.name, Effects::cyclopean.name},
        {Effects::focused.name, Effects::disorienting.name},
        {Effects::jennerising.name, Effects::paranoia.name},
        {Effects::seizure.name, Effects::focused.name},
        {Effects::shrinking.name, Effects::electrifying.name},
        {Effects::slippery.name, Effects::toxic.name},
        {Effects::sneaky.name, Effects::calming.name},
        {Effects::thoughtProvoking.name, Effects::cyclopean.name},
        {Effects::thoughtProvoking.name, Effects::energizing.name}
    });
const Ingredient Ingredients::motorOil(
    L"Motor Oil", 6.0, Effects::slippery.name,
    {
        {Effects::energizing.name, Effects::munchies.name},
        {Effects::euphoric.name, Effects::sedating.name},
        {Effects::foggy.name, Effects::toxic.name},
        {Effects::munchies.name, Effects::schizophrenic.name},
        {Effects::paranoia.name, Effects::antiGravity.name}
    });
const Ingredient Ingredients::mouthWash(
    L"Mouth Wash", 4.0, Effects::balding.name,
    {
        {Effects::calming.name, Effects::antiGravity.name},
        {Effects::calorieDense.name, Effects::sneaky.name},
        {Effects::explosive.name, Effects::sedating.name},
        {Effects::focused.name, Effects::jennerising.name}
    });
const Ingredient Ingredients::paracetamol(
    L"Paracetamol", 3.0, Effects::sneaky.name,
    {
        {Effects::calming.name, Effects::slippery.name},
        {Effects::electrifying.name, Effects::athletic.name},
        {Effects::energizing.name, Effects::paranoia.name},
        {Effects::focused.name, Effects::gingeritis.name},
        {Effects::foggy.name, Effects::calming.name},
        {Effects::glowing.name, Effects::toxic.name},
        {Effects::munchies.name, Effects::antiGravity.name},
        {Effects::paranoia.name, Effects::balding.name},
        {Effects::spicy.name, Effects::brightEyed.name},
        {Effects::toxic.name, Effects::tropicThunder.name}
    });
const Ingredient Ingredients::viagra(
    L"Viagra", 4.0, Effects::tropicThunder.name,
    {
        {Effects::athletic.name, Effects::sneaky.name},
        {Effects::disorienting.name, Effects::toxic.name},
        {Effects::euphoric.name, Effects::brightEyed.name},
        {Effects::laxative.name, Effects::calming.name}
    });

// Array of all ingredients
const Ingredient allIngredients[] = {
    Ingredients::addy,
    Ingredients::banana,
    Ingredients::battery,
    Ingredients::chili,
    Ingredients::cuke,
    Ingredients::donut,
    Ingredients::energyDrink,
    Ingredients::fluMedicine,
    Ingredients::gasoline,
    Ingredients::horseSemen,
    Ingredients::iodine,
    Ingredients::megaBean,
    Ingredients::motorOil,
    Ingredients::mouthWash,
    Ingredients::paracetamol,
    Ingredients::viagra
};  
extern constexpr size_t allIngredientsSize = sizeof(allIngredients) / sizeof(allIngredients[0]);

extern const Ingredient allIngredients[]; // Declaration for use in other files
extern const size_t allIngredientsSize; // Change 'constexpr' to 'const' to avoid redefinition error

// Bases class implementation
const Base Bases::ogKush(L"OG Kush", 30.0, Effects::calming.name);
const Base Bases::sourDiesel(L"Sour Diesel", 35.0, Effects::refreshing.name);
const Base Bases::greenCrack(L"Green Crack", 40.0, Effects::energizing.name);
const Base Bases::grandaddyPurple(L"Grandaddy Purple", 45.0, Effects::sedating.name);
const Base Bases::blankTemplate = Base(L"Blank Template", 0.0, L"");

// Array of all bases
const Base allBases[] = {
    Bases::ogKush,
    Bases::sourDiesel,
    Bases::greenCrack,
    Bases::grandaddyPurple
};
constexpr size_t allBasesSize = sizeof(allBases) / sizeof(allBases[0]);