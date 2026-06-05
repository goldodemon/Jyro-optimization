#include "jyro/OptimizationCatalog.h"

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "shell32.lib")

namespace {

constexpr wchar_t kProductId[] = L"frOFk3Qp2UjzrYLSnoEVOQ==";
constexpr wchar_t kVerifyHost[] = L"api.gumroad.com";
constexpr wchar_t kVerifyPath[] = L"/v2/licenses/verify";
constexpr COLORREF kInk = RGB(238, 247, 255);
constexpr COLORREF kMuted = RGB(137, 165, 190);
constexpr COLORREF kBlue = RGB(0, 139, 255);
constexpr COLORREF kLightBlue = RGB(104, 210, 255);
constexpr COLORREF kPanel = RGB(8, 14, 24);

HWND g_licenseEdit = nullptr;
HWND g_status = nullptr;
HWND g_runLite = nullptr;
HWND g_runPremium = nullptr;
bool g_premiumUnlocked = false;
int g_animationFrame = 0;

std::wstring widen(const std::string& text) {
    if (text.empty()) {
        return L"";
    }
    const int needed = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring result(static_cast<std::size_t>(needed), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), needed);
    return result;
}

std::string narrow(const std::wstring& text) {
    if (text.empty()) {
        return "";
    }
    const int needed = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<std::size_t>(needed), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), needed, nullptr, nullptr);
    return result;
}

void setStatus(const std::wstring& message) {
    SetWindowTextW(g_status, message.c_str());
}

bool isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin == TRUE;
}

void relaunchElevatedIfNeeded() {
    if (isRunningAsAdmin()) {
        return;
    }
    wchar_t path[MAX_PATH]{};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    ShellExecuteW(nullptr, L"runas", path, nullptr, nullptr, SW_SHOWNORMAL);
    ExitProcess(0);
}

bool createRestorePoint(const std::wstring& label) {
    std::wstring command = L"powershell -NoProfile -ExecutionPolicy Bypass -Command \"Checkpoint-Computer -Description '" +
                           label + L"' -RestorePointType MODIFY_SETTINGS\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');
    if (!CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return exitCode == 0;
}

bool runCommand(const std::string& command) {
    std::wstring full = L"cmd.exe /c " + widen(command);
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> mutableCommand(full.begin(), full.end());
    mutableCommand.push_back(L'\0');
    if (!CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return exitCode == 0;
}

std::wstring urlEncode(const std::wstring& input) {
    std::wostringstream encoded;
    const wchar_t* hex = L"0123456789ABCDEF";
    for (wchar_t ch : input) {
        if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'_' || ch == L'.') {
            encoded << ch;
        } else {
            encoded << L'%' << hex[(ch >> 4) & 0xF] << hex[ch & 0xF];
        }
    }
    return encoded.str();
}

bool verifyGumroadLicense(const std::wstring& licenseKey) {
    const std::wstring body = L"product_id=" + urlEncode(kProductId) + L"&license_key=" + urlEncode(licenseKey) + L"&increment_uses_count=false";
    const std::string bodyUtf8 = narrow(body);

    HINTERNET session = WinHttpOpen(L"Jyro Optimization/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) return false;
    HINTERNET connect = WinHttpConnect(session, kVerifyHost, INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connect) {
        WinHttpCloseHandle(session);
        return false;
    }
    HINTERNET request = WinHttpOpenRequest(connect, L"POST", kVerifyPath, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request) {
        WinHttpCloseHandle(connect);
        WinHttpCloseHandle(session);
        return false;
    }

    const wchar_t headers[] = L"Content-Type: application/x-www-form-urlencoded\r\n";
    BOOL ok = WinHttpSendRequest(request, headers, -1, const_cast<char*>(bodyUtf8.data()), static_cast<DWORD>(bodyUtf8.size()), static_cast<DWORD>(bodyUtf8.size()), 0);
    ok = ok && WinHttpReceiveResponse(request, nullptr);

    std::string response;
    if (ok) {
        DWORD available = 0;
        while (WinHttpQueryDataAvailable(request, &available) && available > 0) {
            std::string chunk(available, '\0');
            DWORD read = 0;
            if (!WinHttpReadData(request, chunk.data(), available, &read)) {
                break;
            }
            chunk.resize(read);
            response += chunk;
        }
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connect);
    WinHttpCloseHandle(session);

    response.erase(std::remove_if(response.begin(), response.end(), [](unsigned char ch) { return std::isspace(ch) != 0; }), response.end());
    const bool success = response.find("\"success\":true") != std::string::npos;
    const bool refunded = response.find("\"refunded\":true") != std::string::npos;
    const bool disputed = response.find("\"disputed\":true") != std::string::npos;
    return success && !refunded && !disputed;
}

void applyPlan(bool premium) {
    const auto plan = premium ? jyro::premiumOptimizationsUnlocked() : jyro::liteOptimizations();
    setStatus(L"Creating a Windows restore point before changes...");
    if (!createRestorePoint(L"Jyro Optimization")) {
        setStatus(L"Restore point failed. No optimizations were run.");
        return;
    }

    std::size_t succeeded = 0;
    for (const auto& item : plan) {
        setStatus(L"Running: " + widen(item.name));
        if (runCommand(item.command)) {
            ++succeeded;
        }
    }

    std::wostringstream done;
    done << L"Complete: " << succeeded << L" / " << plan.size() << L" optimizations applied. Reboot recommended.";
    setStatus(done.str());
}

void paintGradientText(HDC dc, const std::wstring& text, int x, int y, int size) {
    HFONT font = CreateFontW(size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI Variable Display");
    HFONT old = static_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, kBlue);
    TextOutW(dc, x + 2, y + 2, text.c_str(), static_cast<int>(text.size()));
    SetTextColor(dc, kLightBlue);
    TextOutW(dc, x, y, text.c_str(), static_cast<int>(text.size()));
    SelectObject(dc, old);
    DeleteObject(font);
}

void paintLogo(HDC dc, int cx, int cy) {
    HPEN ring = CreatePen(PS_SOLID, 5, kBlue);
    HPEN light = CreatePen(PS_SOLID, 3, kLightBlue);
    HBRUSH empty = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HGDIOBJ oldPen = SelectObject(dc, ring);
    HGDIOBJ oldBrush = SelectObject(dc, empty);
    Ellipse(dc, cx - 50, cy - 50, cx + 50, cy + 50);
    SelectObject(dc, light);
    MoveToEx(dc, cx, cy - 46, nullptr); LineTo(dc, cx, cy + 46);
    MoveToEx(dc, cx - 46, cy, nullptr); LineTo(dc, cx + 46, cy);
    MoveToEx(dc, cx - 46, cy, nullptr); LineTo(dc, cx - 30, cy - 12);
    MoveToEx(dc, cx - 46, cy, nullptr); LineTo(dc, cx - 30, cy + 12);
    MoveToEx(dc, cx + 46, cy, nullptr); LineTo(dc, cx + 30, cy - 12);
    MoveToEx(dc, cx + 46, cy, nullptr); LineTo(dc, cx + 30, cy + 12);
    MoveToEx(dc, cx, cy + 46, nullptr); LineTo(dc, cx - 12, cy + 30);
    MoveToEx(dc, cx, cy + 46, nullptr); LineTo(dc, cx + 12, cy + 30);
    paintGradientText(dc, L"J", cx - 17, cy - 38, 72);
    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);
    DeleteObject(ring);
    DeleteObject(light);
}

void drawCard(HDC dc, RECT rect, const std::wstring& title, const std::vector<std::wstring>& lines, COLORREF accent) {
    HBRUSH brush = CreateSolidBrush(kPanel);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(25, 72, 112));
    SelectObject(dc, brush);
    SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, 20, 20);
    DeleteObject(brush);
    DeleteObject(pen);

    HFONT titleFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT bodyFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, accent);
    auto old = SelectObject(dc, titleFont);
    TextOutW(dc, rect.left + 20, rect.top + 18, title.c_str(), static_cast<int>(title.size()));
    SelectObject(dc, bodyFont);
    SetTextColor(dc, kInk);
    int y = rect.top + 58;
    for (const auto& line : lines) {
        TextOutW(dc, rect.left + 20, y, line.c_str(), static_cast<int>(line.size()));
        y += 24;
    }
    SelectObject(dc, old);
    DeleteObject(titleFont);
    DeleteObject(bodyFont);
}

void paint(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC dc = BeginPaint(hwnd, &ps);
    RECT client{};
    GetClientRect(hwnd, &client);
    HBRUSH bg = CreateSolidBrush(RGB(2, 6, 12));
    FillRect(dc, &client, bg);
    DeleteObject(bg);

    SetBkMode(dc, TRANSPARENT);
    paintLogo(dc, 120, 105);
    paintGradientText(dc, L"Jyro Optimization", 205, 48, 44);

    const wchar_t* words[] = {L"Gaming", L"Animation", L"Work", L"Streaming", L"Everyday Use"};
    std::wstring tagline = L"Optimize your PC for " + std::wstring(words[(g_animationFrame / 8) % 5]) + L".";
    HFONT sub = CreateFontW(20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    auto old = SelectObject(dc, sub);
    SetTextColor(dc, kMuted);
    TextOutW(dc, 210, 104, tagline.c_str(), static_cast<int>(tagline.size()));
    SetTextColor(dc, kLightBlue);
    const wchar_t* styleLine = L"Sleek, minimal, blue + light-blue mixed UI";
    TextOutW(dc, 210, 132, styleLine, lstrlenW(styleLine));
    SelectObject(dc, old);
    DeleteObject(sub);

    drawCard(dc, {40, 205, 430, 400}, L"Lite (Free)", {
        L"• 15 free optimizations",
        L"• 3 premium feature previews",
        L"• Game Mode, Boost-Ups, Power Mode",
        L"• Discord Support, Ad-Free UI, themes"
    }, kLightBlue);

    drawCard(dc, {460, 205, 850, 400}, L"Premium", {
        L"• Unlimited optimizations",
        L"• 30 premium exclusives",
        L"• Game, animation, and work profiles",
        L"• Restore-point manager and rollback"
    }, kBlue);

    HFONT foot = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    old = SelectObject(dc, foot);
    SetTextColor(dc, kMuted);
    const wchar_t* safetyLine = L"Jyro requests administrator privileges automatically and requires a restore point before making changes.";
    TextOutW(dc, 40, 520, safetyLine, lstrlenW(safetyLine));
    SelectObject(dc, old);
    DeleteObject(foot);
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        SetLayeredWindowAttributes(hwnd, 0, 244, LWA_ALPHA);
        SetTimer(hwnd, 1, 150, nullptr);
        CreateWindowW(L"STATIC", L"Gumroad License Key", WS_CHILD | WS_VISIBLE, 40, 420, 180, 24, hwnd, nullptr, nullptr, nullptr);
        g_licenseEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 40, 448, 520, 34, hwnd, nullptr, nullptr, nullptr);
        CreateWindowW(L"BUTTON", L"Verify Premium", WS_CHILD | WS_VISIBLE, 575, 448, 130, 34, hwnd, reinterpret_cast<HMENU>(1001), nullptr, nullptr);
        g_runLite = CreateWindowW(L"BUTTON", L"Run Lite", WS_CHILD | WS_VISIBLE, 715, 448, 90, 34, hwnd, reinterpret_cast<HMENU>(1002), nullptr, nullptr);
        g_runPremium = CreateWindowW(L"BUTTON", L"Run Premium", WS_CHILD | WS_VISIBLE | WS_DISABLED, 815, 448, 120, 34, hwnd, reinterpret_cast<HMENU>(1003), nullptr, nullptr);
        g_status = CreateWindowW(L"STATIC", L"Ready. Lite includes 15 normal optimizations and 3 premium previews.", WS_CHILD | WS_VISIBLE, 40, 488, 850, 24, hwnd, nullptr, nullptr, nullptr);
        return 0;
    case WM_TIMER:
        ++g_animationFrame;
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) == 1001) {
            wchar_t key[256]{};
            GetWindowTextW(g_licenseEdit, key, 256);
            setStatus(L"Verifying Gumroad license...");
            g_premiumUnlocked = verifyGumroadLicense(key);
            EnableWindow(g_runPremium, g_premiumUnlocked ? TRUE : FALSE);
            setStatus(g_premiumUnlocked ? L"Premium unlocked. Unlimited optimizations and exclusives enabled." : L"License was not verified. Check the Gumroad key and try again.");
        } else if (LOWORD(wp) == 1002) {
            applyPlan(false);
        } else if (LOWORD(wp) == 1003 && g_premiumUnlocked) {
            applyPlan(true);
        }
        return 0;
    case WM_PAINT:
        paint(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    relaunchElevatedIfNeeded();

    WNDCLASSW wc{};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"JyroOptimizationWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED, wc.lpszClassName, L"Jyro Optimization", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 980, 600, nullptr, nullptr, instance, nullptr);
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

#else
int main() {
    std::cout << "Jyro Optimization is a Windows desktop optimizer.\n";
    std::cout << "Normal optimizations: " << jyro::normalOptimizations().size() << "\n";
    std::cout << "Premium exclusives: " << jyro::premiumOptimizations().size() << "\n";
    std::cout << "Lite unlocks: " << jyro::liteOptimizations().size() << " (15 free + 3 premium previews)\n";
    std::cout << "Premium unlocks: " << jyro::premiumOptimizationsUnlocked().size() << " total optimizations\n";
    return 0;
}
#endif
