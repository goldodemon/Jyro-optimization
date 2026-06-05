#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

namespace jyro {

constexpr const char* kAppName = "Jyro Optimization";
constexpr const char* kProductId = "frOFk3Qp2UjzrYLSnoEVOQ==";

struct Optimization {
    std::string name;
    std::string description;
    bool premiumOnly;
};

struct Theme {
    const char* reset = "\033[0m";
    const char* blue = "\033[38;2;0;122;255m";
    const char* lightBlue = "\033[38;2;105;220;255m";
    const char* dim = "\033[2m";
    const char* bold = "\033[1m";
};

std::vector<Optimization> buildOptimizations() {
    return {
        {"Game Mode", "Prioritize foreground games and reduce background interruptions.", false},
        {"Boost-Ups", "Clean temporary files and refresh lightweight performance caches.", false},
        {"Discord Support Preset", "Tune startup and overlay guidance for Discord-friendly gaming.", false},
        {"Power Mode", "Switch to a high-performance power posture while optimizing.", false},
        {"Game Optimizations", "Apply conservative game-focused Windows responsiveness tweaks.", false},
        {"Ad-Free Experience", "Keep the optimizer interface focused with no bundled ads.", false},
        {"Theme Customization", "Unlock color, accent, and compact layout preferences.", false},
        {"Startup App Review", "List heavy startup entries for the user to disable manually.", false},
        {"Temporary File Cleanup", "Remove safe user temp files after confirmation.", false},
        {"Recycle Bin Reminder", "Prompt for cleanup without silently deleting user files.", false},
        {"DNS Cache Flush", "Refresh stale DNS entries for cleaner network sessions.", false},
        {"Network Stack Refresh", "Queue safe network reset commands for administrator review.", false},
        {"Shader Cache Cleanup", "Clear common GPU shader cache locations where available.", false},
        {"Delivery Optimization Cleanup", "Target Windows delivery cache cleanup guidance.", false},
        {"Windows Update Cache Review", "Check update cache size and recommend cleanup steps.", false},
        {"Telemetry Settings Review", "Show privacy-sensitive Windows settings for manual review.", false},
        {"Background Apps Review", "Suggest background app reductions for work and gaming.", false},
        {"Visual Effects Balanced", "Recommend a balanced visual effects profile.", false},
        {"Storage Sense Setup", "Guide Storage Sense configuration for recurring cleanup.", false},
        {"Services Safety Audit", "Flag risky service changes and keep defaults conservative.", false},
        {"Mouse Precision Review", "Surface pointer precision settings for competitive gaming.", false},
        {"Xbox Game Bar Review", "Review capture and overlay settings that can affect FPS.", false},
        {"Notification Focus Assist", "Recommend focus mode settings for games, animation, and work.", false},
        {"GPU Preference Guide", "Guide per-app high-performance GPU preferences.", false},
        {"Disk Health Reminder", "Prompt the user to check SMART status with trusted tools.", false},
        {"RAM Usage Snapshot", "Show a quick memory usage note before heavier workloads.", false},
        {"Browser Cache Review", "Recommend browser cache cleanup without deleting sign-ins.", false},
        {"Audio Latency Tips", "Suggest safe audio settings for streaming and editing.", false},
        {"Animation Workstation Preset", "Prioritize stability-minded settings for creative apps.", false},
        {"Work Mode Preset", "Reduce distractions and conserve reliability for productivity.", false},
        {"Restore Point Verification", "Confirm a restore point exists before changes are applied.", false},
        {"Driver Update Reminder", "Encourage vendor driver updates without installing bundled tools.", false},
        {"Process Priority Helper", "Explain temporary priority changes for selected apps.", false},
        {"One-Click Lite Bundle", "Run the safest Lite optimizations in a guided batch.", false},
        {"Optimization Report", "Generate a summary of selected actions.", false},

        {"Premium Deep Cleanup", "Run a broader cleanup workflow with additional confirmations.", true},
        {"Premium FPS Stability Pack", "Apply extra low-risk stability recommendations for games.", true},
        {"Premium Latency Pack", "Guide advanced network and input latency checks.", true},
        {"Premium Creator Pack", "Tune workflow recommendations for animation and rendering.", true},
        {"Premium Workstation Pack", "Balance performance and reliability for long work sessions.", true},
        {"Premium Theme Studio", "Use expanded blue, light-blue, and minimal layout themes.", true},
        {"Premium Scheduled Maintenance", "Create a recurring maintenance checklist.", true},
        {"Premium Exportable Reports", "Save detailed optimization reports for support.", true},
        {"Premium Restore Manager", "List recent restore points and recovery reminders.", true},
        {"Premium App Profiles", "Create per-game, per-work, and per-animation presets.", true},
        {"Premium Service Profiles", "Offer reversible service profile recommendations.", true},
        {"Premium Debloat Review", "Identify optional apps for manual removal.", true},
        {"Premium Startup Profiles", "Create gaming and work startup profiles.", true},
        {"Premium Network Diagnostics", "Run expanded ping, DNS, and adapter checks.", true},
        {"Premium GPU Cache Pack", "Guide deeper GPU vendor cache cleanup.", true},
        {"Premium Render Prep", "Prepare a clean environment before rendering or exporting.", true},
        {"Premium Stream Prep", "Checklist for Discord, OBS, and game capture stability.", true},
        {"Premium Laptop Thermal Guide", "Recommend power and cooling checks for laptops.", true},
        {"Premium Benchmark Journal", "Track before-and-after notes from user benchmarks.", true},
        {"Premium Safe Registry Review", "Show registry-related recommendations without blind edits.", true},
        {"Premium Advanced Power Plan", "Create an advanced performance plan on Windows.", true},
        {"Premium Update Pause Helper", "Guide responsible update pause windows before tournaments.", true},
        {"Premium Support Bundle", "Package logs and reports for Discord support.", true},
        {"Premium Unlimited Batch Runs", "Run optimization bundles without Lite usage limits.", true},
        {"Premium Exclusive Roadmap", "Reserve future premium-only modules.", true},
    };
}

#ifdef _WIN32
bool isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                 &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin == TRUE;
}

void relaunchAsAdminIfNeeded() {
    if (isRunningAsAdmin()) {
        return;
    }

    char exePath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    SHELLEXECUTEINFOA sei = {};
    sei.cbSize = sizeof(sei);
    sei.lpVerb = "runas";
    sei.lpFile = exePath;
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExA(&sei)) {
        std::exit(0);
    }

    std::cerr << "Administrator privileges are required for Jyro Optimization.\n";
    std::exit(1);
}
#else
void relaunchAsAdminIfNeeded() {
    std::cout << "Administrator auto-elevation is available in Windows builds.\n";
}
#endif

bool runCommand(const std::string& command) {
    const int result = std::system(command.c_str());
    return result == 0;
}

bool createRestorePoint() {
#ifdef _WIN32
    std::cout << "Creating mandatory Windows restore point...\n";
    const std::string command =
        "powershell -NoProfile -ExecutionPolicy Bypass -Command "
        "\"Checkpoint-Computer -Description 'Jyro Optimization Restore Point' -RestorePointType 'MODIFY_SETTINGS'\"";
    return runCommand(command);
#else
    std::cout << "Restore point creation is a Windows-only requirement. This build will simulate it.\n";
    return true;
#endif
}

std::string trim(std::string value) {
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());
    return value;
}

bool looksLikeGumroadKey(const std::string& key) {
    const std::string cleaned = trim(key);
    if (cleaned.size() < 16) {
        return false;
    }
    return std::all_of(cleaned.begin(), cleaned.end(), [](unsigned char ch) {
        return std::isalnum(ch) || ch == '-';
    });
}

bool verifyPremiumLicense(const std::string& key) {
    if (!looksLikeGumroadKey(key)) {
        return false;
    }

#ifdef _WIN32
    std::cout << "Verifying Premium key with Gumroad...\n";
    const std::string powershell =
        "powershell -NoProfile -ExecutionPolicy Bypass -Command \""
        "$body=@{product_id='" + std::string(kProductId) + "';license_key='" + trim(key) + "'};"
        "$response=Invoke-RestMethod -Uri 'https://api.gumroad.com/v2/licenses/verify' -Method Post -Body $body;"
        "if ($response.success -eq $true) { exit 0 } else { exit 1 }\"";
    return runCommand(powershell);
#else
    std::cout << "Gumroad API verification is enabled in Windows builds; this build validates key format only.\n";
    return true;
#endif
}

void printLogo(const Theme& theme) {
    std::cout << "\n";
    std::cout << theme.blue << "        <-" << theme.lightBlue << "J" << theme.blue << "->\n";
    std::cout << theme.blue << "      /========\\\n";
    std::cout << "     |  " << theme.lightBlue << "JYRO" << theme.blue << "  |\n";
    std::cout << "      \\========/" << theme.reset << "\n\n";
    std::cout << theme.bold << theme.blue << "Jyro " << theme.lightBlue << "Optimization" << theme.reset << "\n";
    std::cout << theme.dim << "Optimize your PC for ";
    std::cout << theme.lightBlue << "Gaming" << theme.reset << theme.dim << " -> ";
    std::cout << theme.lightBlue << "Animation" << theme.reset << theme.dim << " -> ";
    std::cout << theme.lightBlue << "Work" << theme.reset << theme.dim << " -> Etc" << theme.reset << "\n\n";
}

void printTierSummary(bool premium, const Theme& theme) {
    if (premium) {
        std::cout << theme.lightBlue << "Premium unlocked" << theme.reset
                  << ": unlimited optimizations and unlimited premium exclusives.\n";
    } else {
        std::cout << theme.blue << "Lite mode" << theme.reset
                  << ": 15 free optimizations plus 3 sampled premium features.\n";
    }
}

std::vector<Optimization> visibleOptimizations(bool premium) {
    const auto all = buildOptimizations();
    std::vector<Optimization> normal;
    std::vector<Optimization> premiumItems;
    for (const auto& item : all) {
        (item.premiumOnly ? premiumItems : normal).push_back(item);
    }

    if (premium) {
        return all;
    }

    std::vector<Optimization> lite;
    lite.insert(lite.end(), normal.begin(), normal.begin() + std::min<std::size_t>(15, normal.size()));
    lite.insert(lite.end(), premiumItems.begin(), premiumItems.begin() + std::min<std::size_t>(3, premiumItems.size()));
    return lite;
}

void printOptimizations(bool premium, const Theme& theme) {
    const auto items = visibleOptimizations(premium);
    std::cout << "\n" << theme.bold << "Available features" << theme.reset << "\n";
    for (std::size_t index = 0; index < items.size(); ++index) {
        const auto& item = items[index];
        std::cout << theme.blue << (index + 1) << ". " << theme.reset << item.name;
        if (item.premiumOnly) {
            std::cout << " " << theme.lightBlue << "[Premium]" << theme.reset;
        }
        std::cout << " - " << item.description << "\n";
    }
}

void runSelectedOptimization(const Optimization& item, const Theme& theme) {
    std::cout << "\n" << theme.lightBlue << "Running: " << item.name << theme.reset << "\n";
    std::cout << item.description << "\n";
    std::cout << "Completed safe preview workflow. Review any manual steps before changing Windows settings.\n";
}

void interactiveMenu(bool premium, const Theme& theme) {
    auto items = visibleOptimizations(premium);
    printOptimizations(premium, theme);

    while (true) {
        std::cout << "\nChoose an optimization number, A for all visible, or Q to quit: ";
        std::string input;
        std::getline(std::cin, input);
        input = trim(input);
        if (input == "Q" || input == "q") {
            break;
        }
        if (input == "A" || input == "a") {
            for (const auto& item : items) {
                runSelectedOptimization(item, theme);
            }
            continue;
        }

        std::istringstream stream(input);
        std::size_t choice = 0;
        if (stream >> choice && choice >= 1 && choice <= items.size()) {
            runSelectedOptimization(items[choice - 1], theme);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

}  // namespace jyro

int main() {
    const jyro::Theme theme;
    jyro::printLogo(theme);
    jyro::relaunchAsAdminIfNeeded();

    if (!jyro::createRestorePoint()) {
        std::cerr << "Jyro Optimization will not continue because the restore point step failed.\n";
        return 1;
    }

    std::cout << "Enter Gumroad license key for Premium, or press Enter for Lite: ";
    std::string key;
    std::getline(std::cin, key);
    const bool premium = !jyro::trim(key).empty() && jyro::verifyPremiumLicense(key);

    jyro::printTierSummary(premium, theme);
    jyro::interactiveMenu(premium, theme);

    std::cout << "Thanks for using " << jyro::kAppName << ".\n";
    return 0;
}
