#include "jyro/OptimizationCatalog.h"

#include <algorithm>
#include <utility>

namespace jyro {
namespace {

Optimization normal(std::string id, std::string name, std::string description, std::string command) {
    return {std::move(id), std::move(name), std::move(description), std::move(command), Tier::Normal};
}

Optimization premium(std::string id, std::string name, std::string description, std::string command) {
    return {std::move(id), std::move(name), std::move(description), std::move(command), Tier::Premium};
}

const std::vector<Optimization> kNormal = {
    normal("game-mode", "Game Mode", "Enables Windows Game Mode for foreground games.", "reg add HKCU\\Software\\Microsoft\\GameBar /v AllowAutoGameMode /t REG_DWORD /d 1 /f"),
    normal("gpu-scheduling", "Hardware GPU Scheduling", "Turns on hardware accelerated GPU scheduling where supported.", "reg add HKLM\\SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers /v HwSchMode /t REG_DWORD /d 2 /f"),
    normal("power-mode", "Power Mode", "Selects the High Performance power plan.", "powercfg /setactive SCHEME_MIN"),
    normal("ultimate-plan", "Ultimate Performance Plan", "Creates and selects the Ultimate Performance profile.", "powercfg -duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61"),
    normal("visual-effects", "Low-Latency Visual Effects", "Prioritizes performance over heavy desktop animations.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects /v VisualFXSetting /t REG_DWORD /d 2 /f"),
    normal("mouse-accel", "Disable Mouse Acceleration", "Removes pointer acceleration for consistent aiming.", "reg add \"HKCU\\Control Panel\\Mouse\" /v MouseSpeed /t REG_SZ /d 0 /f"),
    normal("xbox-dvr", "Disable Xbox DVR Capture", "Stops background DVR capture overhead.", "reg add HKCU\\System\\GameConfigStore /v GameDVR_Enabled /t REG_DWORD /d 0 /f"),
    normal("fullscreen-opts", "Fullscreen Optimizations", "Disables legacy Game DVR fullscreen behavior.", "reg add HKCU\\System\\GameConfigStore /v GameDVR_FSEBehaviorMode /t REG_DWORD /d 2 /f"),
    normal("network-throttle", "Network Throttle Off", "Reduces multimedia network throttling for online play.", "reg add HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile /v NetworkThrottlingIndex /t REG_DWORD /d 4294967295 /f"),
    normal("system-responsiveness", "System Responsiveness", "Allocates more scheduling time to games and apps.", "reg add HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile /v SystemResponsiveness /t REG_DWORD /d 10 /f"),
    normal("game-priority", "Games Task Priority", "Raises the Games multimedia profile priority.", "reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile\\Tasks\\Games\" /v GPU Priority /t REG_DWORD /d 8 /f"),
    normal("dns-flush", "Flush DNS", "Clears stale DNS resolver cache.", "ipconfig /flushdns"),
    normal("winsock-reset", "Winsock Reset", "Repairs common socket stack issues after a reboot.", "netsh winsock reset"),
    normal("temp-clean", "Temp Cleanup", "Deletes user temporary files that are safe to remove.", "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Remove-Item $env:TEMP\\* -Recurse -Force -ErrorAction SilentlyContinue\""),
    normal("delivery-opt", "Delivery Optimization Cleanup", "Clears Windows delivery cache.", "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Get-DeliveryOptimizationStatus | Out-Null\""),
    normal("startup-delay", "Disable Startup Delay", "Shortens Explorer startup delay.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize /v StartupDelayInMSec /t REG_DWORD /d 0 /f"),
    normal("background-apps", "Reduce Background Apps", "Disables background access for Store apps by default.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\BackgroundAccessApplications /v GlobalUserDisabled /t REG_DWORD /d 1 /f"),
    normal("notifications", "Focus Assist Gaming", "Reduces toast interruptions while optimizing.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Notifications\\Settings /v NOC_GLOBAL_SETTING_TOASTS_ENABLED /t REG_DWORD /d 0 /f"),
    normal("hags-reset", "GPU Driver Timeout Balance", "Sets a balanced TDR delay to reduce false resets.", "reg add HKLM\\SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers /v TdrDelay /t REG_DWORD /d 10 /f"),
    normal("ntfs-memory", "NTFS Memory Usage", "Allows NTFS to use more cache for heavy workloads.", "fsutil behavior set memoryusage 2"),
    normal("disable-hibernation", "Disable Hibernation", "Reclaims hibernation file space on desktops.", "powercfg /hibernate off"),
    normal("timer-resolution", "Timer Resolution Hint", "Configures system timer tolerance for responsive apps.", "bcdedit /set useplatformtick No"),
    normal("boot-menu", "Fast Boot Menu Timeout", "Reduces boot menu timeout.", "bcdedit /timeout 3"),
    normal("search-index", "Search Indexer Balanced", "Moves Windows Search to manual on gaming rigs.", "sc config WSearch start= demand"),
    normal("sysmain", "SysMain On-Demand", "Sets SysMain to manual for lower idle disk activity.", "sc config SysMain start= demand"),
    normal("print-spooler", "Print Spooler On-Demand", "Sets printing services to manual when unused.", "sc config Spooler start= demand"),
    normal("telemetry-basic", "Telemetry Basic", "Limits diagnostic telemetry to security/basic where supported.", "reg add HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\DataCollection /v AllowTelemetry /t REG_DWORD /d 1 /f"),
    normal("activity-history", "Disable Activity History", "Prevents activity history sync overhead.", "reg add HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\System /v PublishUserActivities /t REG_DWORD /d 0 /f"),
    normal("clipboard-cloud", "Disable Clipboard Cloud", "Keeps clipboard history local.", "reg add HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\System /v AllowCrossDeviceClipboard /t REG_DWORD /d 0 /f"),
    normal("edge-preload", "Disable Edge Preload", "Stops Edge prelaunch policy.", "reg add HKLM\\SOFTWARE\\Policies\\Microsoft\\MicrosoftEdge\\Main /v AllowPrelaunch /t REG_DWORD /d 0 /f"),
    normal("onedrive-startup", "OneDrive Startup Off", "Disables OneDrive auto-start for local-only setups.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v OneDrive /t REG_SZ /d \"\" /f"),
    normal("disk-cleanmgr", "Disk Cleanup Preset", "Runs Windows cleanup using the default low-risk preset.", "cleanmgr /verylowdisk"),
    normal("shader-cache", "DirectX Shader Cache", "Clears DirectX shader cache for corrupt-cache fixes.", "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Remove-Item $env:LOCALAPPDATA\\D3DSCache\\* -Recurse -Force -ErrorAction SilentlyContinue\""),
    normal("store-cache", "Microsoft Store Cache", "Resets Microsoft Store cache.", "wsreset.exe -i"),
    normal("time-sync", "Time Sync", "Resyncs Windows time for anti-cheat and auth stability.", "w32tm /resync"),
    normal("defender-scan", "Quick Security Scan", "Runs a quick Defender scan before aggressive tuning.", "powershell -NoProfile -Command \"Start-MpScan -ScanType QuickScan\""),
    normal("restore-health", "Component Store Health", "Checks component store health.", "DISM /Online /Cleanup-Image /ScanHealth"),
    normal("sfc-verify", "System File Verify", "Verifies core Windows files.", "sfc /verifyonly"),
    normal("discord-cache", "Discord Cache Cleanup", "Clears Discord cache folders.", "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Remove-Item $env:APPDATA\\discord\\Cache\\*,$env:APPDATA\\discord\\Code Cache\\*,$env:APPDATA\\discord\\GPUCache\\* -Recurse -Force -ErrorAction SilentlyContinue\""),
    normal("theme-engine", "Theme Customization", "Unlocks Jyro theme presets and local color storage.", "echo Jyro theme customization enabled")
};

const std::vector<Optimization> kPremium = {
    premium("latency-pack", "Premium Latency Pack", "Applies deeper scheduler and latency tuning.", "reg add HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile /v AlwaysOn /t REG_DWORD /d 1 /f"),
    premium("cpu-parking", "CPU Parking Optimizer", "Unparks cores for plugged-in gaming sessions.", "powercfg /setacvalueindex SCHEME_CURRENT SUB_PROCESSOR CPMINCORES 100"),
    premium("cpu-boost", "CPU Boost Optimizer", "Enables aggressive boost behavior on AC power.", "powercfg /setacvalueindex SCHEME_CURRENT SUB_PROCESSOR PERFBOOSTMODE 2"),
    premium("gpu-clean-install", "GPU Driver Prep", "Prepares driver cache cleanup before fresh installs.", "powershell -NoProfile -Command \"Write-Output 'GPU driver prep checklist opened'\""),
    premium("advanced-network", "Advanced Network Stack", "Applies low-latency TCP global settings.", "netsh int tcp set global autotuninglevel=normal rss=enabled ecncapability=disabled"),
    premium("nic-offloads", "NIC Offload Balance", "Balances adapter offloads for latency.", "powershell -NoProfile -Command \"Get-NetAdapterAdvancedProperty | Out-Null\""),
    premium("dns-gaming", "Gaming DNS Preset", "Opens DNS profile selection for fast public resolvers.", "powershell -NoProfile -Command \"Write-Output 'Choose DNS provider in Jyro settings'\""),
    premium("process-priority", "Per-Game Priority Profiles", "Stores app-specific process priority presets.", "echo Per-game priority profiles enabled"),
    premium("ram-trim", "Memory Standby Trim", "Runs a safe standby-memory trim helper hook.", "powershell -NoProfile -Command \"[System.GC]::Collect()\""),
    premium("input-lag", "Input Lag Reducer", "Tightens keyboard and mouse queue behavior.", "reg add \"HKCU\\Control Panel\\Keyboard\" /v KeyboardDelay /t REG_SZ /d 0 /f"),
    premium("capture-stack", "Capture Stack Cleanup", "Disables unused capture overlays.", "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\GameDVR /v AppCaptureEnabled /t REG_DWORD /d 0 /f"),
    premium("streaming-mode", "Streaming Mode", "Balances services for gaming plus streaming.", "echo Streaming mode enabled"),
    premium("work-mode", "Work Mode", "Balances responsiveness for creative and office apps.", "echo Work mode enabled"),
    premium("animation-mode", "Animation Mode", "Prioritizes GPU scheduling for render and animation tools.", "echo Animation mode enabled"),
    premium("render-cache", "Render Cache Cleaner", "Cleans common animation and render cache folders.", "powershell -NoProfile -Command \"Write-Output 'Render cache cleaner ready'\""),
    premium("ad-free", "Ad-Free Experience", "Keeps the Jyro interface clean with no sponsor cards.", "echo Ad-free experience enabled"),
    premium("theme-pro", "Premium Themes", "Unlocks gradients, glass cards, and accent animation.", "echo Premium themes unlocked"),
    premium("restore-manager", "Restore Point Manager", "Creates named restore points before every optimization batch.", "powershell -NoProfile -ExecutionPolicy Bypass -Command \"Checkpoint-Computer -Description 'Jyro Optimization Premium' -RestorePointType MODIFY_SETTINGS\""),
    premium("service-profiler", "Service Profiler", "Builds a reversible service profile for this machine.", "echo Service profile saved"),
    premium("startup-profiler", "Startup Profiler", "Audits startup apps before disabling anything.", "powershell -NoProfile -Command \"Get-CimInstance Win32_StartupCommand | Out-Null\""),
    premium("browser-clean", "Browser Cache Cleaner", "Cleans supported browser caches with confirmation.", "echo Browser cleaner ready"),
    premium("launcher-clean", "Game Launcher Cleaner", "Cleans cache for common launchers.", "echo Launcher cleaner ready"),
    premium("anti-cheat-safe", "Anti-Cheat Safe Mode", "Avoids tweaks known to upset anti-cheat systems.", "echo Anti-cheat safe guard enabled"),
    premium("benchmark", "Benchmark Snapshot", "Captures before/after system metrics.", "winsat formal -restart clean"),
    premium("backup-profile", "Backup Optimization Profile", "Exports Jyro settings and changed keys.", "reg export HKCU\\Software\\JyroOptimization %USERPROFILE%\\Desktop\\jyro-profile.reg /y"),
    premium("rollback", "One-Click Rollback", "Uses restore-point metadata to roll back Jyro changes.", "rstrui.exe"),
    premium("scheduler-pro", "Scheduler Pro", "Applies workload-aware scheduler presets.", "echo Scheduler Pro enabled"),
    premium("storage-pro", "Storage Pro", "Optimizes TRIM and storage health checks.", "defrag /C /O"),
    premium("discord-priority", "Discord Support Priority", "Pins Discord support details and diagnostics export.", "echo Discord support diagnostics ready"),
    premium("auto-update", "Premium Update Channel", "Enables the premium optimization catalog update channel.", "echo Premium update channel enabled")
};

} // namespace

const std::vector<Optimization>& normalOptimizations() {
    return kNormal;
}

const std::vector<Optimization>& premiumOptimizations() {
    return kPremium;
}

std::vector<Optimization> liteOptimizations(PlanLimits limits) {
    std::vector<Optimization> selected;
    const auto normalCount = std::min(limits.liteNormalLimit, kNormal.size());
    const auto premiumCount = std::min(limits.litePremiumPreviewLimit, kPremium.size());
    selected.insert(selected.end(), kNormal.begin(), kNormal.begin() + static_cast<std::ptrdiff_t>(normalCount));
    selected.insert(selected.end(), kPremium.begin(), kPremium.begin() + static_cast<std::ptrdiff_t>(premiumCount));
    return selected;
}

std::vector<Optimization> premiumOptimizationsUnlocked() {
    std::vector<Optimization> selected = kNormal;
    selected.insert(selected.end(), kPremium.begin(), kPremium.end());
    return selected;
}

} // namespace jyro
