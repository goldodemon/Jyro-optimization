#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace jyro {

enum class Tier {
    Normal,
    Premium
};

struct Optimization {
    std::string id;
    std::string name;
    std::string description;
    std::string command;
    Tier tier;
};

struct PlanLimits {
    std::size_t liteNormalLimit = 15;
    std::size_t litePremiumPreviewLimit = 3;
};

const std::vector<Optimization>& normalOptimizations();
const std::vector<Optimization>& premiumOptimizations();
std::vector<Optimization> liteOptimizations(PlanLimits limits = {});
std::vector<Optimization> premiumOptimizationsUnlocked();

} // namespace jyro
