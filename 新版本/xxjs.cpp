#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <initguid.h>
#include <knownfolders.h>
#include <gdiplus.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
namespace fs = std::filesystem;

std::string GetCurrentTimeStr() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    char buf[32];
    sprintf_s(buf, "%04d%02d%02d_%02d%02d%02d",
              st.wYear, st.wMonth, st.wDay,
              st.wHour, st.wMinute, st.wSecond);
    return std::string(buf);
}

std::string GenerateRandomString(int length) {
    const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; i++) {
        result += chars[rand() % (sizeof(chars) - 1)];
    }
    return result;
}

std::string Base64Encode(const std::string& input) {
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    const unsigned char* data = (const unsigned char*)input.data();
    int in_len = (int)input.size();
    while (in_len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; i < 4; i++) {
                result += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }
    if (i) {
        for (int j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (int j = 0; j < i + 1; j++) {
            result += base64_chars[char_array_4[j]];
        }
        while (i++ < 3) {
            result += '=';
        }
    }
    return result;
}

std::string HttpGet(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("jpssl", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }
    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return response;
}

bool WriteRegistry(const std::string& key, const std::string& value) {
    HKEY hKey;
    LONG result = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\JPFQ", 0, NULL,
                                   REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        return false;
    }
    result = RegSetValueExA(hKey, key.c_str(), 0, REG_SZ,
                            (const BYTE*)value.c_str(), (DWORD)(value.length() + 1));
    RegCloseKey(hKey);
    return result == ERROR_SUCCESS;
}

bool RegisterAccount(std::string& outUsername, std::string& outPassword, std::string& outClientId) {
    std::string username = "xmpj666" + GenerateRandomString(8);
    std::string password = "123456";
    std::string randomStr = "Device_" + GetCurrentTimeStr() + "_" + GenerateRandomString(10);
    std::string clientid = Base64Encode(randomStr);
    std::string url = "https://xu1.uk/user/reg.aspx?mode=pc&pwd=" + password +
                      "&clientid=" + clientid +
                      "&user=" + username +
                      "&username=" + username;
    std::string response = HttpGet(url);
    if (response.empty()) {
        return false;
    }
    if (response.find('|') == std::string::npos) {
        return false;
    }
    std::string regValue = username + "|" + password + "|" + clientid;
    if (!WriteRegistry("user", regValue)) {
        return false;
    }
    outUsername = username;
    outPassword = password;
    outClientId = clientid;
    return true;
}

struct CustomButton {
    Rect bounds;
    std::wstring text;
    Color color;
    Color textColor;
    bool isHovered;
    bool isOutline;
    int id;
};

CustomButton g_btns[3];
const int WIN_WIDTH = 360;
const int WIN_HEIGHT = 640;

void UpdateButtonPositions() {
    int btnW = 280;
    int btnH = 50;
    int startX = (WIN_WIDTH - btnW) / 2;
    int startY = 360;
    g_btns[0] = { Rect(startX, startY, btnW, btnH), L"一键注册", Color(255, 255, 255, 255), Color(255, 100, 149, 237), false, false, 1 };
    g_btns[1] = { Rect(startX, startY + 70, btnW, btnH), L"Github 项目地址", Color(255, 18, 174, 232), Color(255, 255, 255, 255), false, false, 2 };
    g_btns[2] = { Rect(startX, startY + 180, btnW, btnH), L"退出", Color(0, 0, 0, 0), Color(255, 255, 255, 255), false, true, 3 };
}

void DrawRoundRect(Graphics& g, const Rect& rect, Color color, int radius, bool outline, Color outlineColor) {
    GraphicsPath path;
    path.AddArc(rect.X, rect.Y, radius, radius, 180, 90);
    path.AddArc(rect.X + rect.Width - radius, rect.Y, radius, radius, 270, 90);
    path.AddArc(rect.X + rect.Width - radius, rect.Y + rect.Height - radius, radius, radius, 0, 90);
    path.AddArc(rect.X, rect.Y + rect.Height - radius, radius, radius, 90, 90);
    path.CloseFigure();
    if (!outline) {
        SolidBrush brush(color);
        g.FillPath(&brush, &path);
    } else {
        Pen pen(outlineColor, 2);
        g.DrawPath(&pen, &path);
    }
}

std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(memDC, memBitmap);

            Graphics g(memDC);
            g.SetSmoothingMode(SmoothingModeAntiAlias);
            g.SetTextRenderingHint(TextRenderingHintAntiAlias);

            LinearGradientBrush linGrBrush(Rect(0, 0, width, height), Color(255, 220, 240, 255), Color(255, 60, 60, 120), LinearGradientModeVertical);
            g.FillRectangle(&linGrBrush, 0, 0, width, height);

            FontFamily fontFamily(L"Microsoft YaHei");
            Font titleFont(&fontFamily, 42, FontStyleBold, UnitPixel);
            Font sloganFont(&fontFamily, 18, FontStyleRegular, UnitPixel);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            StringFormat format;
            format.SetAlignment(StringAlignmentCenter);

            g.DrawString(L"星星加速", -1, &titleFont, RectF(0, 120, (REAL)width, 60), &format, &whiteBrush);
            g.DrawString(L"小猫一键注册版", -1, &sloganFont, RectF(0, 190, (REAL)width, 30), &format, &whiteBrush);

            for (int i = 0; i < 3; i++) {
                Color btnColor = g_btns[i].color;
                if (g_btns[i].isHovered && !g_btns[i].isOutline) {
                    btnColor = Color(255, (BYTE)std::max(0, (int)btnColor.GetR() - 20), (BYTE)std::max(0, (int)btnColor.GetG() - 20), (BYTE)std::max(0, (int)btnColor.GetB() - 20));
                }
                DrawRoundRect(g, g_btns[i].bounds, btnColor, 40, g_btns[i].isOutline, Color(255, 255, 255, 255));
                SolidBrush tBrush(g_btns[i].textColor);
                Font btnFont(&fontFamily, 16, FontStyleRegular, UnitPixel);
                RectF textRect((REAL)g_btns[i].bounds.X, (REAL)g_btns[i].bounds.Y + 15, (REAL)g_btns[i].bounds.Width, (REAL)g_btns[i].bounds.Height);
                g.DrawString(g_btns[i].text.c_str(), -1, &btnFont, textRect, &format, &tBrush);
            }

            BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            bool changed = false;
            bool anyHover = false;
            for (int i = 0; i < 3; i++) {
                bool hover = g_btns[i].bounds.Contains(x, y);
                if (hover != g_btns[i].isHovered) {
                    g_btns[i].isHovered = hover;
                    changed = true;
                }
                if (hover) anyHover = true;
            }
            if (changed) InvalidateRect(hwnd, NULL, FALSE);
            SetCursor(LoadCursor(NULL, anyHover ? IDC_HAND : IDC_ARROW));
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            break;
        }
        case WM_MOUSELEAVE: {
            for (int i = 0; i < 3; i++) g_btns[i].isHovered = false;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (g_btns[0].bounds.Contains(x, y)) {
                std::string u, p, cid;
                if (RegisterAccount(u, p, cid)) {
                    std::wstring msg = L"注册成功！\n用户名: " + Utf8ToWide(u) + L"\n密码: " + Utf8ToWide(p) + L"\n已自动写入注册表。";
                    MessageBoxW(hwnd, msg.c_str(), L"星星加速", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBoxW(hwnd, L"注册失败！请检查网络连接或稍后再试。", L"错误", MB_OK | MB_ICONERROR);
                }
            } else if (g_btns[1].bounds.Contains(x, y)) {
                ShellExecuteW(NULL, L"open", L"https://github.com/xmzmjmmm/xxjs", NULL, NULL, SW_SHOWNORMAL);
            } else if (g_btns[2].bounds.Contains(x, y)) {
                PostQuitMessage(0);
            }
            break;
        }
        case WM_NCHITTEST: {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ScreenToClient(hwnd, &pt);
            for (int i = 0; i < 3; i++) {
                if (g_btns[i].bounds.Contains(pt.x, pt.y)) return HTCLIENT;
            }
            LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    srand((unsigned int)time(NULL));
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UpdateButtonPositions();
    const wchar_t CLASS_NAME[] = L"STAR_JS_MODERN_UI";
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"星星加速一键注册版", WS_POPUP,
                                (GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2,
                                (GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2,
                                WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    return 0;
}
