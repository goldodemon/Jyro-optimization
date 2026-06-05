#include "jyro/OptimizationCatalog.h"

#include <cassert>
#include <iostream>

int main() {
    const auto& normal = jyro::normalOptimizations();
    const auto& premium = jyro::premiumOptimizations();
    const auto lite = jyro::liteOptimizations();
    const auto unlocked = jyro::premiumOptimizationsUnlocked();

    assert(normal.size() == 40);
    assert(premium.size() == 30);
    assert(lite.size() == 18);
    assert(unlocked.size() == 70);
    assert(lite.front().id == "game-mode");
    assert(lite.at(15).tier == jyro::Tier::Premium);

    std::cout << "Catalog counts and Lite/Premium limits are correct.\n";
    return 0;
}
