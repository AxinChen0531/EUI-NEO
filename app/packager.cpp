#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#include <objbase.h>
#include <shlobj.h>
#include <shellapi.h>
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "Ole32.lib")
#endif

#include "app/dsl_app.h"

#include "components/components.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_set>
#include <vector>

#if !defined(_WIN32)
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(_WIN64)
namespace eui_box_glfw_bridge {

HMODULE glfwModule() {
    static HMODULE module = [] {
        HMODULE loaded = LoadLibraryA("glfw3.dll");
        if (loaded == nullptr) {
            MessageBoxA(nullptr,
                        "glfw3.dll was not found next to the packager. Put glfw3.dll beside packager.exe to open the GUI.",
                        "EUI Box Packager",
                        MB_ICONERROR | MB_OK);
            ExitProcess(1);
        }
        return loaded;
    }();
    return module;
}

FARPROC glfwProc(const char* name) {
    FARPROC proc = GetProcAddress(glfwModule(), name);
    if (proc == nullptr) {
        MessageBoxA(nullptr, name, "Missing GLFW export", MB_ICONERROR | MB_OK);
        ExitProcess(1);
    }
    return proc;
}

template <typename Fn>
Fn load(const char* name) {
    return reinterpret_cast<Fn>(glfwProc(name));
}

} // namespace eui_box_glfw_bridge

extern "C" {

int eui_box_glfwInit() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwInit)>("glfwInit");
    return fn();
}

void eui_box_glfwTerminate() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwTerminate)>("glfwTerminate");
    fn();
}

void eui_box_glfwWindowHint(int hint, int value) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwWindowHint)>("glfwWindowHint");
    fn(hint, value);
}

GLFWwindow* eui_box_glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwCreateWindow)>("glfwCreateWindow");
    return fn(width, height, title, monitor, share);
}

void eui_box_glfwMakeContextCurrent(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwMakeContextCurrent)>("glfwMakeContextCurrent");
    fn(window);
}

void eui_box_glfwSwapInterval(int interval) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSwapInterval)>("glfwSwapInterval");
    fn(interval);
}

double eui_box_glfwGetTime() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetTime)>("glfwGetTime");
    return fn();
}

void eui_box_glfwSetWindowTitle(GLFWwindow* window, const char* title) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowTitle)>("glfwSetWindowTitle");
    fn(window, title);
}

void eui_box_glfwSetWindowUserPointer(GLFWwindow* window, void* pointer) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowUserPointer)>("glfwSetWindowUserPointer");
    fn(window, pointer);
}

void* eui_box_glfwGetWindowUserPointer(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetWindowUserPointer)>("glfwGetWindowUserPointer");
    return fn(window);
}

GLFWframebuffersizefun eui_box_glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetFramebufferSizeCallback)>("glfwSetFramebufferSizeCallback");
    return fn(window, callback);
}

GLFWwindowrefreshfun eui_box_glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowRefreshCallback)>("glfwSetWindowRefreshCallback");
    return fn(window, callback);
}

GLFWwindowcontentscalefun eui_box_glfwSetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowContentScaleCallback)>("glfwSetWindowContentScaleCallback");
    return fn(window, callback);
}

GLFWwindowclosefun eui_box_glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowCloseCallback)>("glfwSetWindowCloseCallback");
    return fn(window, callback);
}

GLFWwindowiconifyfun eui_box_glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowIconifyCallback)>("glfwSetWindowIconifyCallback");
    return fn(window, callback);
}

GLFWglproc eui_box_glfwGetProcAddress(const char* name) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetProcAddress)>("glfwGetProcAddress");
    return fn(name);
}

int eui_box_glfwWindowShouldClose(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwWindowShouldClose)>("glfwWindowShouldClose");
    return fn(window);
}

int eui_box_glfwGetKey(GLFWwindow* window, int key) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetKey)>("glfwGetKey");
    return fn(window, key);
}

void eui_box_glfwSetWindowShouldClose(GLFWwindow* window, int value) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowShouldClose)>("glfwSetWindowShouldClose");
    fn(window, value);
}

void eui_box_glfwHideWindow(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwHideWindow)>("glfwHideWindow");
    fn(window);
}

void eui_box_glfwShowWindow(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwShowWindow)>("glfwShowWindow");
    fn(window);
}

void eui_box_glfwRestoreWindow(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwRestoreWindow)>("glfwRestoreWindow");
    fn(window);
}

void eui_box_glfwFocusWindow(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwFocusWindow)>("glfwFocusWindow");
    fn(window);
}

void eui_box_glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetFramebufferSize)>("glfwGetFramebufferSize");
    fn(window, width, height);
}

void eui_box_glfwGetWindowContentScale(GLFWwindow* window, float* xscale, float* yscale) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetWindowContentScale)>("glfwGetWindowContentScale");
    fn(window, xscale, yscale);
}

void eui_box_glfwGetWindowSize(GLFWwindow* window, int* width, int* height) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetWindowSize)>("glfwGetWindowSize");
    fn(window, width, height);
}

void eui_box_glfwWaitEvents() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwWaitEvents)>("glfwWaitEvents");
    fn();
}

void eui_box_glfwWaitEventsTimeout(double timeout) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwWaitEventsTimeout)>("glfwWaitEventsTimeout");
    fn(timeout);
}

void eui_box_glfwSwapBuffers(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSwapBuffers)>("glfwSwapBuffers");
    fn(window);
}

void eui_box_glfwPollEvents() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwPollEvents)>("glfwPollEvents");
    fn();
}

GLFWmonitor* eui_box_glfwGetWindowMonitor(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetWindowMonitor)>("glfwGetWindowMonitor");
    return fn(window);
}

void eui_box_glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetWindowPos)>("glfwGetWindowPos");
    fn(window, xpos, ypos);
}

GLFWmonitor** eui_box_glfwGetMonitors(int* count) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetMonitors)>("glfwGetMonitors");
    return fn(count);
}

GLFWmonitor* eui_box_glfwGetPrimaryMonitor() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetPrimaryMonitor)>("glfwGetPrimaryMonitor");
    return fn();
}

void eui_box_glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetMonitorPos)>("glfwGetMonitorPos");
    fn(monitor, xpos, ypos);
}

const GLFWvidmode* eui_box_glfwGetVideoMode(GLFWmonitor* monitor) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetVideoMode)>("glfwGetVideoMode");
    return fn(monitor);
}

void eui_box_glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetWindowIcon)>("glfwSetWindowIcon");
    fn(window, count, images);
}

void eui_box_glfwPostEmptyEvent() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwPostEmptyEvent)>("glfwPostEmptyEvent");
    fn();
}

GLFWwindow* eui_box_glfwGetCurrentContext() {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetCurrentContext)>("glfwGetCurrentContext");
    return fn();
}

void eui_box_glfwSetClipboardString(GLFWwindow* window, const char* string) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetClipboardString)>("glfwSetClipboardString");
    fn(window, string);
}

const char* eui_box_glfwGetClipboardString(GLFWwindow* window) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetClipboardString)>("glfwGetClipboardString");
    return fn(window);
}

GLFWcursor* eui_box_glfwCreateStandardCursor(int shape) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwCreateStandardCursor)>("glfwCreateStandardCursor");
    return fn(shape);
}

void eui_box_glfwDestroyCursor(GLFWcursor* cursor) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwDestroyCursor)>("glfwDestroyCursor");
    fn(cursor);
}

void eui_box_glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetCursor)>("glfwSetCursor");
    fn(window, cursor);
}

void eui_box_glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetCursorPos)>("glfwGetCursorPos");
    fn(window, xpos, ypos);
}

int eui_box_glfwGetMouseButton(GLFWwindow* window, int button) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwGetMouseButton)>("glfwGetMouseButton");
    return fn(window, button);
}

GLFWcharfun eui_box_glfwSetCharCallback(GLFWwindow* window, GLFWcharfun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetCharCallback)>("glfwSetCharCallback");
    return fn(window, callback);
}

GLFWkeyfun eui_box_glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetKeyCallback)>("glfwSetKeyCallback");
    return fn(window, callback);
}

GLFWscrollfun eui_box_glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback) {
    static auto fn = eui_box_glfw_bridge::load<decltype(&::glfwSetScrollCallback)>("glfwSetScrollCallback");
    return fn(window, callback);
}

decltype(&::glfwInit) __imp_glfwInit = eui_box_glfwInit;
decltype(&::glfwTerminate) __imp_glfwTerminate = eui_box_glfwTerminate;
decltype(&::glfwWindowHint) __imp_glfwWindowHint = eui_box_glfwWindowHint;
decltype(&::glfwCreateWindow) __imp_glfwCreateWindow = eui_box_glfwCreateWindow;
decltype(&::glfwMakeContextCurrent) __imp_glfwMakeContextCurrent = eui_box_glfwMakeContextCurrent;
decltype(&::glfwSwapInterval) __imp_glfwSwapInterval = eui_box_glfwSwapInterval;
decltype(&::glfwGetTime) __imp_glfwGetTime = eui_box_glfwGetTime;
decltype(&::glfwSetWindowTitle) __imp_glfwSetWindowTitle = eui_box_glfwSetWindowTitle;
decltype(&::glfwSetWindowUserPointer) __imp_glfwSetWindowUserPointer = eui_box_glfwSetWindowUserPointer;
decltype(&::glfwGetWindowUserPointer) __imp_glfwGetWindowUserPointer = eui_box_glfwGetWindowUserPointer;
decltype(&::glfwSetFramebufferSizeCallback) __imp_glfwSetFramebufferSizeCallback = eui_box_glfwSetFramebufferSizeCallback;
decltype(&::glfwSetWindowRefreshCallback) __imp_glfwSetWindowRefreshCallback = eui_box_glfwSetWindowRefreshCallback;
decltype(&::glfwSetWindowContentScaleCallback) __imp_glfwSetWindowContentScaleCallback = eui_box_glfwSetWindowContentScaleCallback;
decltype(&::glfwSetWindowCloseCallback) __imp_glfwSetWindowCloseCallback = eui_box_glfwSetWindowCloseCallback;
decltype(&::glfwSetWindowIconifyCallback) __imp_glfwSetWindowIconifyCallback = eui_box_glfwSetWindowIconifyCallback;
decltype(&::glfwGetProcAddress) __imp_glfwGetProcAddress = eui_box_glfwGetProcAddress;
decltype(&::glfwWindowShouldClose) __imp_glfwWindowShouldClose = eui_box_glfwWindowShouldClose;
decltype(&::glfwGetKey) __imp_glfwGetKey = eui_box_glfwGetKey;
decltype(&::glfwSetWindowShouldClose) __imp_glfwSetWindowShouldClose = eui_box_glfwSetWindowShouldClose;
decltype(&::glfwHideWindow) __imp_glfwHideWindow = eui_box_glfwHideWindow;
decltype(&::glfwShowWindow) __imp_glfwShowWindow = eui_box_glfwShowWindow;
decltype(&::glfwRestoreWindow) __imp_glfwRestoreWindow = eui_box_glfwRestoreWindow;
decltype(&::glfwFocusWindow) __imp_glfwFocusWindow = eui_box_glfwFocusWindow;
decltype(&::glfwGetFramebufferSize) __imp_glfwGetFramebufferSize = eui_box_glfwGetFramebufferSize;
decltype(&::glfwGetWindowContentScale) __imp_glfwGetWindowContentScale = eui_box_glfwGetWindowContentScale;
decltype(&::glfwGetWindowSize) __imp_glfwGetWindowSize = eui_box_glfwGetWindowSize;
decltype(&::glfwWaitEvents) __imp_glfwWaitEvents = eui_box_glfwWaitEvents;
decltype(&::glfwWaitEventsTimeout) __imp_glfwWaitEventsTimeout = eui_box_glfwWaitEventsTimeout;
decltype(&::glfwSwapBuffers) __imp_glfwSwapBuffers = eui_box_glfwSwapBuffers;
decltype(&::glfwPollEvents) __imp_glfwPollEvents = eui_box_glfwPollEvents;
decltype(&::glfwGetWindowMonitor) __imp_glfwGetWindowMonitor = eui_box_glfwGetWindowMonitor;
decltype(&::glfwGetWindowPos) __imp_glfwGetWindowPos = eui_box_glfwGetWindowPos;
decltype(&::glfwGetMonitors) __imp_glfwGetMonitors = eui_box_glfwGetMonitors;
decltype(&::glfwGetPrimaryMonitor) __imp_glfwGetPrimaryMonitor = eui_box_glfwGetPrimaryMonitor;
decltype(&::glfwGetMonitorPos) __imp_glfwGetMonitorPos = eui_box_glfwGetMonitorPos;
decltype(&::glfwGetVideoMode) __imp_glfwGetVideoMode = eui_box_glfwGetVideoMode;
decltype(&::glfwSetWindowIcon) __imp_glfwSetWindowIcon = eui_box_glfwSetWindowIcon;
decltype(&::glfwPostEmptyEvent) __imp_glfwPostEmptyEvent = eui_box_glfwPostEmptyEvent;
decltype(&::glfwGetCurrentContext) __imp_glfwGetCurrentContext = eui_box_glfwGetCurrentContext;
decltype(&::glfwSetClipboardString) __imp_glfwSetClipboardString = eui_box_glfwSetClipboardString;
decltype(&::glfwGetClipboardString) __imp_glfwGetClipboardString = eui_box_glfwGetClipboardString;
decltype(&::glfwCreateStandardCursor) __imp_glfwCreateStandardCursor = eui_box_glfwCreateStandardCursor;
decltype(&::glfwDestroyCursor) __imp_glfwDestroyCursor = eui_box_glfwDestroyCursor;
decltype(&::glfwSetCursor) __imp_glfwSetCursor = eui_box_glfwSetCursor;
decltype(&::glfwGetCursorPos) __imp_glfwGetCursorPos = eui_box_glfwGetCursorPos;
decltype(&::glfwGetMouseButton) __imp_glfwGetMouseButton = eui_box_glfwGetMouseButton;
decltype(&::glfwSetCharCallback) __imp_glfwSetCharCallback = eui_box_glfwSetCharCallback;
decltype(&::glfwSetKeyCallback) __imp_glfwSetKeyCallback = eui_box_glfwSetKeyCallback;
decltype(&::glfwSetScrollCallback) __imp_glfwSetScrollCallback = eui_box_glfwSetScrollCallback;

} // extern "C"
#endif

namespace app {

namespace {

namespace fs = std::filesystem;

constexpr std::array<char, 8> kPayloadMagic = {'E', 'U', 'I', 'B', 'O', 'X', '1', '\0'};
constexpr std::array<char, 16> kFooterMagic = {'E', 'U', 'I', 'B', 'O', 'X', 'F', 'O', 'O', 'T', 'E', 'R', '0', '0', '1', '\0'};
constexpr std::uint32_t kPackageVersion = 1;
constexpr std::uint32_t kFlagDeleteAfterExit = 1u << 0;
constexpr std::uint64_t kFooterSize = 8 + 8 + kFooterMagic.size();
constexpr std::uint64_t kCopyBufferSize = 1024 * 1024;
constexpr const char* kLauncherPathEnv = "EUI_BOX_LAUNCHER_PATH";

struct PackageFooter {
    bool found = false;
    std::uint64_t payloadOffset = 0;
    std::uint64_t payloadSize = 0;
};

struct BundleEntry {
    fs::path source;
    std::string packagePath;
    std::uint64_t size = 0;
};

struct BuildResult {
    bool ok = false;
    std::string message;
};

struct UiState {
    std::string entryPath;
    std::string outputPath;
    std::string resourcePath;
    std::vector<std::string> resources;
    std::string status = "Ready";
    bool deleteAfterExit = true;
    float resourceScroll = 0.0f;
};

UiState& state() {
    static UiState value;
    return value;
}

components::theme::ThemeColorTokens tokens() {
    return {
        components::theme::color(0.07f, 0.075f, 0.08f),
        components::theme::defaultPrimary(),
        components::theme::color(0.12f, 0.125f, 0.135f),
        components::theme::color(0.18f, 0.18f, 0.19f),
        components::theme::color(0.23f, 0.23f, 0.24f),
        components::theme::color(0.94f, 0.96f, 0.94f),
        components::theme::color(0.30f, 0.34f, 0.34f),
        true
    };
}

core::Color color(float r, float g, float b, float a = 1.0f) {
    return {r, g, b, a};
}

core::Color mutedText() {
    return color(0.62f, 0.67f, 0.66f, 1.0f);
}

core::Color warningText() {
    return color(0.96f, 0.74f, 0.36f, 1.0f);
}

std::string ellipsize(const std::string& value, std::size_t maxChars) {
    if (value.size() <= maxChars || maxChars <= 3) {
        return value;
    }
    const std::size_t head = (maxChars - 3) / 2;
    const std::size_t tail = maxChars - 3 - head;
    return value.substr(0, head) + "..." + value.substr(value.size() - tail);
}

std::string quoteArg(const std::string& value) {
    std::string result = "\"";
    for (char ch : value) {
        if (ch == '"') {
            result += "\\\"";
        } else {
            result.push_back(ch);
        }
    }
    result += "\"";
    return result;
}

std::string currentExecutablePath() {
#if defined(_WIN32)
    std::string buffer(MAX_PATH, '\0');
    for (;;) {
        const DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0) {
            return {};
        }
        if (length < buffer.size() - 1) {
            buffer.resize(length);
            return buffer;
        }
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__linux__)
    std::string buffer(4096, '\0');
    const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length <= 0) {
        return {};
    }
    buffer.resize(static_cast<std::size_t>(length));
    return buffer;
#else
    std::error_code error;
    return fs::absolute(fs::current_path(error), error).string();
#endif
}

std::string launcherExecutablePath() {
#if defined(_WIN32)
    DWORD length = GetEnvironmentVariableA(kLauncherPathEnv, nullptr, 0);
    if (length == 0) {
        return {};
    }
    std::string value(length, '\0');
    const DWORD copied = GetEnvironmentVariableA(kLauncherPathEnv, value.data(), length);
    if (copied == 0 || copied >= length) {
        return {};
    }
    value.resize(copied);
    return value;
#else
    const char* value = std::getenv(kLauncherPathEnv);
    return value == nullptr ? std::string{} : std::string(value);
#endif
}

bool readExact(std::istream& in, char* data, std::size_t size) {
    in.read(data, static_cast<std::streamsize>(size));
    return in.good() || static_cast<std::size_t>(in.gcount()) == size;
}

void writeU32(std::ostream& out, std::uint32_t value) {
    char data[4] = {
        static_cast<char>(value & 0xffu),
        static_cast<char>((value >> 8) & 0xffu),
        static_cast<char>((value >> 16) & 0xffu),
        static_cast<char>((value >> 24) & 0xffu)
    };
    out.write(data, sizeof(data));
}

void writeU64(std::ostream& out, std::uint64_t value) {
    char data[8] = {};
    for (int i = 0; i < 8; ++i) {
        data[i] = static_cast<char>((value >> (i * 8)) & 0xffu);
    }
    out.write(data, sizeof(data));
}

bool readU32(std::istream& in, std::uint32_t& value) {
    char data[4] = {};
    if (!readExact(in, data, sizeof(data))) {
        return false;
    }
    value = static_cast<std::uint32_t>(static_cast<unsigned char>(data[0])) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(data[1])) << 8) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(data[2])) << 16) |
            (static_cast<std::uint32_t>(static_cast<unsigned char>(data[3])) << 24);
    return true;
}

bool readU64(std::istream& in, std::uint64_t& value) {
    char data[8] = {};
    if (!readExact(in, data, sizeof(data))) {
        return false;
    }
    value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= static_cast<std::uint64_t>(static_cast<unsigned char>(data[i])) << (i * 8);
    }
    return true;
}

bool copyBytes(std::istream& in, std::ostream& out, std::uint64_t bytes, std::string& error) {
    std::vector<char> buffer(kCopyBufferSize);
    std::uint64_t remaining = bytes;
    while (remaining > 0) {
        const std::size_t chunk = static_cast<std::size_t>(std::min<std::uint64_t>(remaining, buffer.size()));
        in.read(buffer.data(), static_cast<std::streamsize>(chunk));
        if (static_cast<std::size_t>(in.gcount()) != chunk) {
            error = "Failed to read source data.";
            return false;
        }
        out.write(buffer.data(), static_cast<std::streamsize>(chunk));
        if (!out) {
            error = "Failed to write output data.";
            return false;
        }
        remaining -= chunk;
    }
    return true;
}

PackageFooter readPackageFooter(const fs::path& executablePath) {
    PackageFooter footer;
    std::error_code error;
    const std::uint64_t fileSize = fs::file_size(executablePath, error);
    if (error || fileSize < kFooterSize) {
        return footer;
    }

    std::ifstream in(executablePath, std::ios::binary);
    if (!in) {
        return footer;
    }
    in.seekg(static_cast<std::streamoff>(fileSize - kFooterSize), std::ios::beg);

    std::uint64_t offset = 0;
    std::uint64_t size = 0;
    std::array<char, kFooterMagic.size()> magic = {};
    if (!readU64(in, offset) || !readU64(in, size) || !readExact(in, magic.data(), magic.size())) {
        return footer;
    }
    if (magic != kFooterMagic || offset >= fileSize || offset + size + kFooterSize != fileSize) {
        return footer;
    }

    footer.found = true;
    footer.payloadOffset = offset;
    footer.payloadSize = size;
    return footer;
}

std::uint64_t executableTemplateSize(const fs::path& executablePath) {
    const PackageFooter footer = readPackageFooter(executablePath);
    if (footer.found) {
        return footer.payloadOffset;
    }
    std::error_code error;
    const std::uint64_t size = fs::file_size(executablePath, error);
    return error ? 0 : size;
}

bool safePackagePath(const std::string& value) {
    if (value.empty() || value[0] == '/' || value[0] == '\\' || value.find(':') != std::string::npos) {
        return false;
    }
    std::stringstream stream(value);
    std::string part;
    while (std::getline(stream, part, '/')) {
        if (part.empty() || part == "." || part == "..") {
            return false;
        }
    }
    return true;
}

std::string packagePathForFile(const fs::path& path) {
    return path.filename().generic_string();
}

std::string packagePathForChild(const fs::path& root, const fs::path& child) {
    std::error_code error;
    fs::path relative = fs::relative(child, root, error);
    if (error) {
        relative = child.filename();
    }
    fs::path base = root.filename();
    if (base.empty()) {
        base = "resources";
    }
    return (base / relative).generic_string();
}

std::string packagePathRelativeToRoot(const fs::path& root, const fs::path& child) {
    std::error_code error;
    fs::path relative = fs::relative(child, root, error);
    if (error || relative.empty()) {
        relative = child.filename();
    }
    return relative.generic_string();
}

std::string lowerAscii(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

bool isDllFile(const fs::path& path) {
    return lowerAscii(path.extension().string()) == ".dll";
}

bool addEntry(std::vector<BundleEntry>& entries,
              std::unordered_set<std::string>& seen,
              const fs::path& source,
              const std::string& packagePath,
              std::string& error) {
    if (!safePackagePath(packagePath)) {
        error = "Unsafe package path: " + packagePath;
        return false;
    }
    if (seen.find(packagePath) != seen.end()) {
        return true;
    }

    std::error_code ec;
    const std::uint64_t size = fs::file_size(source, ec);
    if (ec) {
        error = "Cannot read file size: " + source.string();
        return false;
    }

    entries.push_back({source, packagePath, size});
    seen.insert(packagePath);
    return true;
}

bool collectEntries(const fs::path& entryPath,
                    const std::vector<std::string>& resources,
                    std::vector<BundleEntry>& entries,
                    std::string& error) {
    std::unordered_set<std::string> seen;
    if (!addEntry(entries, seen, entryPath, packagePathForFile(entryPath), error)) {
        return false;
    }

    for (const std::string& raw : resources) {
        if (raw.empty()) {
            continue;
        }
        const fs::path source(raw);
        std::error_code ec;
        if (fs::is_regular_file(source, ec)) {
            if (!addEntry(entries, seen, source, packagePathForFile(source), error)) {
                return false;
            }
        } else if (fs::is_directory(source, ec)) {
            const fs::path entryDir = entryPath.parent_path();
            std::error_code eqError;
            const bool flattenAtRoot = !entryDir.empty() && fs::equivalent(source, entryDir, eqError) && !eqError;
            for (fs::recursive_directory_iterator it(source, fs::directory_options::skip_permission_denied, ec), end;
                 it != end && !ec;
                 it.increment(ec)) {
                if (!it->is_regular_file(ec)) {
                    continue;
                }
                const std::string packagePath = flattenAtRoot
                    ? packagePathRelativeToRoot(source, it->path())
                    : packagePathForChild(source, it->path());
                if (!addEntry(entries, seen, it->path(), packagePath, error)) {
                    return false;
                }
            }
            if (ec) {
                error = "Cannot enumerate directory: " + source.string();
                return false;
            }
        } else {
            error = "Resource does not exist: " + raw;
            return false;
        }
    }

    return true;
}

std::string defaultOutputPath(const std::string& entryPath) {
    if (entryPath.empty()) {
        return {};
    }
    const fs::path entry(entryPath);
    fs::path dir;
    const std::string launcherPath = launcherExecutablePath();
    if (!launcherPath.empty()) {
        dir = fs::path(launcherPath).parent_path();
    }
    if (dir.empty()) {
        dir = entry.parent_path() / "dist";
    }
    if (dir.empty()) {
        std::error_code error;
        dir = fs::current_path(error) / "dist";
        if (error) {
            dir = entry.parent_path();
        }
    }
    std::string extension = entry.extension().string();
#if defined(_WIN32)
    if (extension.empty()) {
        extension = ".exe";
    }
#endif
    return (dir / (entry.stem().string() + "-boxed" + extension)).string();
}

std::string bytesText(std::uint64_t value) {
    const char* suffixes[] = {"B", "KB", "MB", "GB"};
    double amount = static_cast<double>(value);
    int suffix = 0;
    while (amount >= 1024.0 && suffix < 3) {
        amount /= 1024.0;
        ++suffix;
    }
    std::ostringstream out;
    if (suffix == 0) {
        out << static_cast<std::uint64_t>(amount) << ' ' << suffixes[suffix];
    } else {
        out.setf(std::ios::fixed);
        out.precision(amount >= 10.0 ? 1 : 2);
        out << amount << ' ' << suffixes[suffix];
    }
    return out.str();
}

BuildResult buildPackage() {
    UiState& uiState = state();
    const fs::path entryPath(uiState.entryPath);
    fs::path outputPath(uiState.outputPath);

    std::error_code ec;
    if (uiState.entryPath.empty() || !fs::is_regular_file(entryPath, ec)) {
        return {false, "Select a valid entry executable first."};
    }
    if (uiState.outputPath.empty()) {
        return {false, "Choose an output file path."};
    }
    if (fs::equivalent(entryPath, outputPath, ec) && !ec) {
        return {false, "Output cannot overwrite the entry executable."};
    }

    const fs::path selfPath(currentExecutablePath());
    if (selfPath.empty()) {
        return {false, "Cannot locate the packager executable."};
    }
    if (fs::equivalent(selfPath, outputPath, ec) && !ec) {
        return {false, "Output cannot overwrite the running packager."};
    }

    std::vector<BundleEntry> entries;
    std::string error;
    if (!collectEntries(entryPath, uiState.resources, entries, error)) {
        return {false, error};
    }
    if (entries.empty()) {
        return {false, "No files to package."};
    }

    const std::uint64_t templateSize = executableTemplateSize(selfPath);
    if (templateSize == 0) {
        return {false, "Cannot read packager template bytes."};
    }

    if (!outputPath.parent_path().empty()) {
        fs::create_directories(outputPath.parent_path(), ec);
        if (ec) {
            return {false, "Cannot create output directory: " + outputPath.parent_path().string()};
        }
    }

    std::ifstream selfIn(selfPath, std::ios::binary);
    std::ofstream out(outputPath, std::ios::binary | std::ios::trunc);
    if (!selfIn || !out) {
        return {false, "Cannot open input template or output file."};
    }

    if (!copyBytes(selfIn, out, templateSize, error)) {
        return {false, error};
    }

    const std::uint64_t payloadOffset = static_cast<std::uint64_t>(out.tellp());
    out.write(kPayloadMagic.data(), static_cast<std::streamsize>(kPayloadMagic.size()));
    writeU32(out, kPackageVersion);
    writeU32(out, uiState.deleteAfterExit ? kFlagDeleteAfterExit : 0);
    writeU32(out, 0);
    writeU32(out, static_cast<std::uint32_t>(entries.size()));

    std::uint64_t payloadBytes = 0;
    for (const BundleEntry& entry : entries) {
        writeU32(out, static_cast<std::uint32_t>(entry.packagePath.size()));
        writeU64(out, entry.size);
        out.write(entry.packagePath.data(), static_cast<std::streamsize>(entry.packagePath.size()));

        std::ifstream fileIn(entry.source, std::ios::binary);
        if (!fileIn) {
            return {false, "Cannot open resource: " + entry.source.string()};
        }
        if (!copyBytes(fileIn, out, entry.size, error)) {
            return {false, error + " " + entry.source.string()};
        }
        payloadBytes += entry.size;
    }

    const std::uint64_t payloadEnd = static_cast<std::uint64_t>(out.tellp());
    const std::uint64_t payloadSize = payloadEnd - payloadOffset;
    writeU64(out, payloadOffset);
    writeU64(out, payloadSize);
    out.write(kFooterMagic.data(), static_cast<std::streamsize>(kFooterMagic.size()));
    out.close();

    if (!out) {
        return {false, "Failed to finish output file."};
    }

#if !defined(_WIN32)
    fs::permissions(outputPath,
                    fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                    fs::perm_options::add,
                    ec);
#endif

    return {
        true,
        "Built " + outputPath.string() + " with " + std::to_string(entries.size()) +
            " files (" + bytesText(payloadBytes) + " payload)."
    };
}

bool readPayloadHeader(std::istream& in,
                       std::uint64_t payloadOffset,
                       std::uint32_t& flags,
                       std::uint32_t& entryIndex,
                       std::uint32_t& entryCount) {
    in.seekg(static_cast<std::streamoff>(payloadOffset), std::ios::beg);
    std::array<char, kPayloadMagic.size()> magic = {};
    std::uint32_t version = 0;
    if (!readExact(in, magic.data(), magic.size()) || magic != kPayloadMagic) {
        return false;
    }
    return readU32(in, version) &&
           version == kPackageVersion &&
           readU32(in, flags) &&
           readU32(in, entryIndex) &&
           readU32(in, entryCount) &&
           entryCount > 0 &&
           entryIndex < entryCount;
}

fs::path temporaryExtractRoot() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::size_t seed = std::hash<std::string>{}(currentExecutablePath());
    std::error_code error;
    fs::path root = fs::temp_directory_path(error);
    if (error) {
        root = fs::current_path(error);
    }
    return root / ("eui-neo-box-" + std::to_string(seed) + "-" + std::to_string(now));
}

int launchEntry(const fs::path& entryPath, const fs::path& workDir) {
#if defined(_WIN32)
    std::string command = quoteArg(entryPath.string());
    std::vector<char> commandLine(command.begin(), command.end());
    commandLine.push_back('\0');
    std::string cwd = workDir.string();
    const std::string launcherPath = currentExecutablePath();

    std::string previousLauncherPath;
    bool hadPreviousLauncherPath = false;
    const DWORD previousLength = GetEnvironmentVariableA(kLauncherPathEnv, nullptr, 0);
    if (previousLength > 0) {
        previousLauncherPath.resize(previousLength, '\0');
        const DWORD copied = GetEnvironmentVariableA(kLauncherPathEnv, previousLauncherPath.data(), previousLength);
        if (copied > 0 && copied < previousLength) {
            previousLauncherPath.resize(copied);
            hadPreviousLauncherPath = true;
        }
    }
    if (!launcherPath.empty()) {
        SetEnvironmentVariableA(kLauncherPathEnv, launcherPath.c_str());
    }

    STARTUPINFOA startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    const BOOL launched = CreateProcessA(nullptr,
                                         commandLine.data(),
                                         nullptr,
                                         nullptr,
                                         FALSE,
                                         0,
                                         nullptr,
                                         cwd.c_str(),
                                         &startup,
                                         &process);
    if (hadPreviousLauncherPath) {
        SetEnvironmentVariableA(kLauncherPathEnv, previousLauncherPath.c_str());
    } else {
        SetEnvironmentVariableA(kLauncherPathEnv, nullptr);
    }
    if (!launched) {
        return 1;
    }
    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return static_cast<int>(exitCode);
#else
    const auto shellQuote = [](const std::string& value) {
        std::string result = "'";
        for (char ch : value) {
            if (ch == '\'') {
                result += "'\\''";
            } else {
                result.push_back(ch);
            }
        }
        result += "'";
        return result;
    };
    const std::string launcherPath = currentExecutablePath();
    const std::string env = launcherPath.empty() ? "" : (std::string(kLauncherPathEnv) + "=" + shellQuote(launcherPath) + " ");
    const std::string command = "cd " + shellQuote(workDir.string()) + " && " + env + shellQuote(entryPath.string());
    return std::system(command.c_str()) == 0 ? 0 : 1;
#endif
}

int extractAndRunPackedPayload(const fs::path& selfPath, const PackageFooter& footer) {
    std::ifstream in(selfPath, std::ios::binary);
    if (!in) {
        return 1;
    }

    std::uint32_t flags = 0;
    std::uint32_t entryIndex = 0;
    std::uint32_t entryCount = 0;
    if (!readPayloadHeader(in, footer.payloadOffset, flags, entryIndex, entryCount)) {
        return 1;
    }

    std::error_code ec;
    const fs::path root = temporaryExtractRoot();
    fs::create_directories(root, ec);
    if (ec) {
        return 1;
    }

    fs::path entryFile;
    for (std::uint32_t i = 0; i < entryCount; ++i) {
        std::uint32_t pathLength = 0;
        std::uint64_t fileSize = 0;
        if (!readU32(in, pathLength) || !readU64(in, fileSize) || pathLength == 0 || pathLength > 4096) {
            return 1;
        }

        std::string packagePath(pathLength, '\0');
        if (!readExact(in, packagePath.data(), packagePath.size()) || !safePackagePath(packagePath)) {
            return 1;
        }

        fs::path target = root / fs::path(packagePath);
        fs::create_directories(target.parent_path(), ec);
        if (ec) {
            return 1;
        }

        std::ofstream out(target, std::ios::binary | std::ios::trunc);
        if (!out) {
            return 1;
        }
        std::string error;
        if (!copyBytes(in, out, fileSize, error)) {
            return 1;
        }
        out.close();

#if !defined(_WIN32)
        fs::permissions(target,
                        fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                        fs::perm_options::add,
                        ec);
#endif

        if (i == entryIndex) {
            entryFile = target;
        }
    }

    if (entryFile.empty()) {
        return 1;
    }

    const int exitCode = launchEntry(entryFile, root);
    if ((flags & kFlagDeleteAfterExit) != 0) {
        fs::remove_all(root, ec);
    }
    return exitCode;
}

void runPackedPayloadIfPresent() {
    static bool checked = false;
    if (checked) {
        return;
    }
    checked = true;

    const fs::path selfPath(currentExecutablePath());
    if (selfPath.empty()) {
        return;
    }

    const PackageFooter footer = readPackageFooter(selfPath);
    if (!footer.found) {
        return;
    }

    std::exit(extractAndRunPackedPayload(selfPath, footer));
}

struct PackedPayloadStartup {
    PackedPayloadStartup() {
        runPackedPayloadIfPresent();
    }
};

const PackedPayloadStartup kPackedPayloadStartup;

#if defined(_WIN32)
std::string openFileDialog(const char* title, bool saveDialog, bool executableFilter = false) {
    char path[MAX_PATH * 4] = {};
    OPENFILENAMEA dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = nullptr;
    dialog.lpstrTitle = title;
    dialog.lpstrFile = path;
    dialog.nMaxFile = sizeof(path);
    dialog.lpstrFilter = executableFilter
        ? "Executable Files\0*.exe\0All Files\0*.*\0"
        : "All Files\0*.*\0Dynamic Libraries\0*.dll\0Executable Files\0*.exe\0";
    dialog.nFilterIndex = 1;
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!saveDialog) {
        dialog.Flags |= OFN_FILEMUSTEXIST;
    } else {
        dialog.Flags |= OFN_OVERWRITEPROMPT;
        dialog.lpstrDefExt = "exe";
    }

    const BOOL ok = saveDialog ? GetSaveFileNameA(&dialog) : GetOpenFileNameA(&dialog);
    return ok ? std::string(path) : std::string();
}

std::vector<std::string> openFileDialogMulti(const char* title) {
    std::vector<char> buffer(65536, '\0');
    OPENFILENAMEA dialog{};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = nullptr;
    dialog.lpstrTitle = title;
    dialog.lpstrFile = buffer.data();
    dialog.nMaxFile = static_cast<DWORD>(buffer.size());
    dialog.lpstrFilter = "All Files\0*.*\0Dynamic Libraries\0*.dll\0Executable Files\0*.exe\0";
    dialog.nFilterIndex = 1;
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (!GetOpenFileNameA(&dialog)) {
        return {};
    }

    std::vector<std::string> paths;
    const char* cursor = buffer.data();
    std::string first = cursor;
    cursor += first.size() + 1;
    if (*cursor == '\0') {
        paths.push_back(first);
        return paths;
    }

    while (*cursor != '\0') {
        const std::string fileName = cursor;
        paths.push_back((fs::path(first) / fileName).string());
        cursor += fileName.size() + 1;
    }
    return paths;
}

std::string narrowWidePath(const wchar_t* value) {
    if (value == nullptr || value[0] == L'\0') {
        return {};
    }
    const int size = WideCharToMultiByte(CP_ACP, 0, value, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 1) {
        return {};
    }
    std::string result(static_cast<std::size_t>(size - 1), '\0');
    WideCharToMultiByte(CP_ACP, 0, value, -1, result.data(), size, nullptr, nullptr);
    return result;
}

std::wstring widenTitle(const char* value) {
    if (value == nullptr || value[0] == '\0') {
        return {};
    }
    const int size = MultiByteToWideChar(CP_ACP, 0, value, -1, nullptr, 0);
    if (size <= 1) {
        return {};
    }
    std::wstring result(static_cast<std::size_t>(size - 1), L'\0');
    MultiByteToWideChar(CP_ACP, 0, value, -1, result.data(), size);
    return result;
}

std::vector<std::string> openFolderDialogMulti(const char* title) {
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    const bool shouldUninitialize = SUCCEEDED(init);
    if (FAILED(init) && init != RPC_E_CHANGED_MODE) {
        return {};
    }

    IFileOpenDialog* dialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
    if (FAILED(hr) || dialog == nullptr) {
        if (shouldUninitialize) {
            CoUninitialize();
        }
        return {};
    }

    DWORD options = 0;
    if (SUCCEEDED(dialog->GetOptions(&options))) {
        dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_ALLOWMULTISELECT | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
    }
    const std::wstring wideTitle = widenTitle(title);
    if (!wideTitle.empty()) {
        dialog->SetTitle(wideTitle.c_str());
    }

    std::vector<std::string> paths;
    if (SUCCEEDED(dialog->Show(nullptr))) {
        IShellItemArray* items = nullptr;
        if (SUCCEEDED(dialog->GetResults(&items)) && items != nullptr) {
            DWORD count = 0;
            if (SUCCEEDED(items->GetCount(&count))) {
                for (DWORD i = 0; i < count; ++i) {
                    IShellItem* item = nullptr;
                    if (SUCCEEDED(items->GetItemAt(i, &item)) && item != nullptr) {
                        PWSTR widePath = nullptr;
                        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &widePath)) && widePath != nullptr) {
                            paths.push_back(narrowWidePath(widePath));
                            CoTaskMemFree(widePath);
                        }
                        item->Release();
                    }
                }
            }
            items->Release();
        }
    }

    dialog->Release();
    if (shouldUninitialize) {
        CoUninitialize();
    }
    return paths;
}

std::string openFolderDialog(const char* title) {
    BROWSEINFOA browse{};
    browse.lpszTitle = title;
    browse.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    PIDLIST_ABSOLUTE item = SHBrowseForFolderA(&browse);
    if (item == nullptr) {
        return {};
    }
    char path[MAX_PATH * 4] = {};
    const BOOL ok = SHGetPathFromIDListA(item, path);
    CoTaskMemFree(item);
    return ok ? std::string(path) : std::string();
}
#else
std::string openFileDialog(const char*, bool, bool = false) {
    return {};
}

std::vector<std::string> openFileDialogMulti(const char*) {
    return {};
}

std::vector<std::string> openFolderDialogMulti(const char*) {
    return {};
}

std::string openFolderDialog(const char*) {
    return {};
}
#endif

void setStatus(std::string value) {
    state().status = std::move(value);
}

bool appendResourcePath(const std::string& path) {
    if (path.empty()) {
        return false;
    }

    std::error_code ec;
    if (!fs::exists(fs::path(path), ec) || ec) {
        return false;
    }

    UiState& uiState = state();
    if (std::find(uiState.resources.begin(), uiState.resources.end(), path) == uiState.resources.end()) {
        uiState.resources.push_back(path);
        return true;
    }
    return false;
}

void addResourcePath(const std::string& path) {
    if (path.empty()) {
        setStatus("Enter or browse a resource path first.");
        return;
    }

    std::error_code ec;
    if (!fs::exists(fs::path(path), ec) || ec) {
        setStatus("Resource path does not exist.");
        return;
    }

    if (appendResourcePath(path)) {
        state().resourcePath.clear();
        setStatus("Added resource: " + path);
    } else {
        setStatus("Resource already added.");
    }
}

void addResourcePaths(const std::vector<std::string>& paths) {
    if (paths.empty()) {
        setStatus("File dialog canceled or unavailable.");
        return;
    }

    int added = 0;
    for (const std::string& path : paths) {
        if (appendResourcePath(path)) {
            ++added;
        }
    }
    state().resourcePath.clear();
    setStatus("Added " + std::to_string(added) + " resource item(s).");
}

void removeResource(std::size_t index) {
    UiState& uiState = state();
    if (index >= uiState.resources.size()) {
        return;
    }
    uiState.resources.erase(uiState.resources.begin() + static_cast<std::ptrdiff_t>(index));
    setStatus("Removed resource.");
}

void drawLabel(core::dsl::Ui& ui, const std::string& id, const std::string& text, float width, float height) {
    ui.text(id)
        .size(width, height)
        .text(text)
        .fontSize(13.0f)
        .lineHeight(16.0f)
        .color(mutedText())
        .verticalAlign(core::VerticalAlign::Center)
        .build();
}

void drawSectionTitle(core::dsl::Ui& ui, const std::string& id, const std::string& text, float width) {
    ui.text(id)
        .size(width, 26.0f)
        .text(text)
        .fontSize(19.0f)
        .lineHeight(24.0f)
        .color(color(0.94f, 0.96f, 0.94f, 1.0f))
        .verticalAlign(core::VerticalAlign::Center)
        .build();
}

void drawFieldRow(core::dsl::Ui& ui,
                  const std::string& id,
                  const std::string& label,
                  const std::string& value,
                  const std::string& placeholder,
                  float width,
                  const std::function<void(const std::string&)>& onChange,
                  const std::function<void()>& onBrowse) {
    const float labelWidth = 104.0f;
    const float buttonWidth = 92.0f;
    const float gap = 12.0f;
    const float inputWidth = std::max(120.0f, width - labelWidth - buttonWidth - gap * 2.0f);

    ui.row(id)
        .size(width, 44.0f)
        .gap(gap)
        .alignItems(core::Align::CENTER)
        .content([&] {
            drawLabel(ui, id + ".label", label, labelWidth, 40.0f);

            components::input(ui, id + ".input")
                .theme(tokens())
                .size(inputWidth, 40.0f)
                .value(value)
                .placeholder(placeholder)
                .fontSize(14.0f)
                .onChange(onChange)
                .build();

            components::button(ui, id + ".browse")
                .theme(tokens(), false)
                .size(buttonWidth, 40.0f)
                .text("Browse")
                .fontSize(14.0f)
                .radius(10.0f)
                .onClick(onBrowse)
                .build();
        })
        .build();
}

void drawResourceList(core::dsl::Ui& ui, float width, float height) {
    UiState& uiState = state();
    const float headerHeight = 34.0f;
    const float rowHeight = 44.0f;
    const float contentHeight = headerHeight + rowHeight * static_cast<float>(std::max<std::size_t>(1, uiState.resources.size() + 1));
    const float maxScroll = std::max(0.0f, contentHeight - height);
    uiState.resourceScroll = std::clamp(uiState.resourceScroll, 0.0f, maxScroll);
    const bool scrollable = maxScroll > 0.0f;
    const float scrollbarWidth = scrollable ? 8.0f : 0.0f;
    const float listWidth = std::max(0.0f, width - scrollbarWidth - (scrollable ? 12.0f : 0.0f));

    ui.stack("resources.viewport")
        .size(width, height)
        .clip()
        .onScroll([maxScroll](const core::ScrollEvent& event) {
            state().resourceScroll = std::clamp(state().resourceScroll - static_cast<float>(event.y) * 44.0f, 0.0f, maxScroll);
        })
        .content([&] {
            ui.column("resources.content")
                .y(-uiState.resourceScroll)
                .size(listWidth, contentHeight)
                .gap(0.0f)
                .content([&] {
                    ui.row("resources.entry")
                        .size(listWidth, rowHeight)
                        .gap(10.0f)
                        .alignItems(core::Align::CENTER)
                        .content([&] {
                            ui.text("resources.entry.kind")
                                .size(72.0f, rowHeight)
                                .text("Entry")
                                .fontSize(13.0f)
                                .lineHeight(16.0f)
                                .color(tokens().primary)
                                .verticalAlign(core::VerticalAlign::Center)
                                .build();

                            ui.text("resources.entry.path")
                                .size(std::max(0.0f, listWidth - 82.0f), rowHeight)
                                .text(state().entryPath.empty() ? "No entry executable selected" : ellipsize(state().entryPath, 72))
                                .fontSize(14.0f)
                                .lineHeight(18.0f)
                                .color(state().entryPath.empty() ? warningText() : tokens().text)
                                .verticalAlign(core::VerticalAlign::Center)
                                .build();
                        })
                        .build();

                    if (uiState.resources.empty()) {
                        ui.text("resources.empty")
                            .size(listWidth, rowHeight)
                            .text("No extra files or folders")
                            .fontSize(14.0f)
                            .lineHeight(18.0f)
                            .color(mutedText())
                            .verticalAlign(core::VerticalAlign::Center)
                            .build();
                    }

                    for (std::size_t i = 0; i < uiState.resources.size(); ++i) {
                        const std::string rowId = "resources.row." + std::to_string(i);
                        ui.row(rowId)
                            .size(listWidth, rowHeight)
                            .gap(10.0f)
                            .alignItems(core::Align::CENTER)
                            .content([&, i, rowId] {
                                ui.text(rowId + ".kind")
                                    .size(72.0f, rowHeight)
                                    .text("Resource")
                                    .fontSize(13.0f)
                                    .lineHeight(16.0f)
                                    .color(mutedText())
                                    .verticalAlign(core::VerticalAlign::Center)
                                    .build();

                                ui.text(rowId + ".path")
                                    .size(std::max(0.0f, listWidth - 72.0f - 76.0f - 20.0f), rowHeight)
                                    .text(ellipsize(uiState.resources[i], 64))
                                    .fontSize(14.0f)
                                    .lineHeight(18.0f)
                                    .color(tokens().text)
                                    .verticalAlign(core::VerticalAlign::Center)
                                    .build();

                                components::button(ui, rowId + ".remove")
                                    .theme(tokens(), false)
                                    .size(76.0f, 32.0f)
                                    .text("Remove")
                                    .fontSize(13.0f)
                                    .radius(8.0f)
                                    .onClick([i] {
                                        removeResource(i);
                                    })
                                    .build();
                            })
                            .build();
                    }
                })
                .build();

            if (scrollable) {
                components::scroll(ui, "resources.scroll")
                    .theme(tokens())
                    .x(width - scrollbarWidth)
                    .size(scrollbarWidth, height)
                    .viewport(height)
                    .content(contentHeight)
                    .offset(uiState.resourceScroll)
                    .onChange([](float value) {
                        state().resourceScroll = value;
                    })
                    .build();
            }
        })
        .build();
}

void drawHeader(core::dsl::Ui& ui, float width) {
    ui.row("header")
        .size(width, 72.0f)
        .alignItems(core::Align::CENTER)
        .content([&] {
            ui.column("header.copy")
                .size(std::max(0.0f, width - 220.0f), 72.0f)
                .gap(6.0f)
                .justifyContent(core::Align::CENTER)
                .content([&] {
                    ui.text("header.title")
                        .size(std::max(0.0f, width - 220.0f), 34.0f)
                        .text("EUI Box Packager")
                        .fontSize(29.0f)
                        .lineHeight(32.0f)
                        .color(tokens().text)
                        .verticalAlign(core::VerticalAlign::Center)
                        .build();

                    ui.text("header.subtitle")
                        .size(std::max(0.0f, width - 220.0f), 24.0f)
                        .text("Self-extracting single-file distributor")
                        .fontSize(15.0f)
                        .lineHeight(20.0f)
                        .color(mutedText())
                        .verticalAlign(core::VerticalAlign::Center)
                        .build();
                })
                .build();

            ui.stack("header.badge")
                .size(220.0f, 38.0f)
                .content([&] {
                    ui.rect("header.badge.bg")
                        .size(220.0f, 38.0f)
                        .color(components::theme::withAlpha(tokens().primary, 0.20f))
                        .radius(999.0f)
                        .border(1.0f, components::theme::withAlpha(tokens().primary, 0.68f))
                        .build();

                    ui.text("header.badge.text")
                        .size(220.0f, 38.0f)
                        .text("single EXE")
                        .fontSize(15.0f)
                        .lineHeight(18.0f)
                        .color(color(0.86f, 0.91f, 1.0f, 1.0f))
                        .horizontalAlign(core::HorizontalAlign::Center)
                        .verticalAlign(core::VerticalAlign::Center)
                        .build();
                })
                .build();
        })
        .build();
}

} // namespace

const DslAppConfig& dslAppConfig() {
    runPackedPayloadIfPresent();
    static const DslAppConfig config = DslAppConfig{}
        .title("EUI Box Packager")
        .pageId("packager")
        .clearColor({0.07f, 0.075f, 0.08f, 1.0f})
        .windowSize(1440, 960);
    return config;
}

void compose(core::dsl::Ui& ui, const core::dsl::Screen& screen) {
    UiState& uiState = state();
    const float width = std::max(920.0f, screen.width);
    const float height = std::max(660.0f, screen.height);
    const float margin = 30.0f;
    const float innerWidth = width - margin * 2.0f;
    const float innerHeight = height - margin * 2.0f;
    const float headerHeight = 82.0f;
    const float gap = 20.0f;
    const float bodyHeight = innerHeight - headerHeight - gap;
    const float leftWidth = std::min(660.0f, innerWidth * 0.58f);
    const float rightWidth = std::max(300.0f, innerWidth - leftWidth - gap);
    const float panelInset = 24.0f;

    ui.stack("root")
        .size(width, height)
        .content([&] {
            ui.rect("background")
                .size(width, height)
                .color(tokens().background)
                .build();

            ui.column("shell")
                .size(innerWidth, innerHeight)
                .margin(margin)
                .gap(gap)
                .content([&] {
                    drawHeader(ui, innerWidth);

                    ui.row("body")
                        .size(innerWidth, bodyHeight)
                        .gap(gap)
                        .content([&] {
                            ui.stack("left.panel")
                                .size(leftWidth, bodyHeight)
                                .content([&] {
                                    components::panel(ui, "left.panel.bg", tokens())
                                        .size(leftWidth, bodyHeight)
                                        .radius(16.0f)
                                        .build();

                                    ui.column("left.content")
                                        .size(leftWidth - panelInset * 2.0f, bodyHeight - panelInset * 2.0f)
                                        .margin(panelInset)
                                        .gap(12.0f)
                                        .content([&] {
                                            drawSectionTitle(ui, "source.title", "Source", leftWidth - panelInset * 2.0f);

                                            drawFieldRow(
                                                ui,
                                                "entry.field",
                                                "Entry EXE",
                                                uiState.entryPath,
                                                "C:\\path\\to\\app.exe",
                                                leftWidth - panelInset * 2.0f,
                                                [](const std::string& value) {
                                                    state().entryPath = value;
                                                    if (state().outputPath.empty()) {
                                                        state().outputPath = defaultOutputPath(value);
                                                    }
                                                },
                                                [] {
                                                    const std::string path = openFileDialog("Select entry executable", false, true);
                                                    if (path.empty()) {
                                                        setStatus("File dialog canceled or unavailable.");
                                                        return;
                                                    }
                                                    state().entryPath = path;
                                                    if (state().outputPath.empty()) {
                                                        state().outputPath = defaultOutputPath(path);
                                                    }
                                                    setStatus("Entry selected: " + path);
                                                });

                                            drawFieldRow(
                                                ui,
                                                "output.field",
                                                "Output",
                                                uiState.outputPath,
                                                "dist\\app-boxed.exe",
                                                leftWidth - panelInset * 2.0f,
                                                [](const std::string& value) {
                                                    state().outputPath = value;
                                                },
                                                [] {
                                                    const std::string path = openFileDialog("Save single executable", true, true);
                                                    if (path.empty()) {
                                                        setStatus("Save dialog canceled or unavailable.");
                                                        return;
                                                    }
                                                    state().outputPath = path;
                                                    setStatus("Output selected: " + path);
                                                });

                                            drawSectionTitle(ui, "resources.title", "Resources", leftWidth - panelInset * 2.0f);

                                            components::input(ui, "resource.input")
                                                .theme(tokens())
                                                .size(leftWidth - panelInset * 2.0f, 40.0f)
                                                .value(uiState.resourcePath)
                                                .placeholder("File or folder to include")
                                                .fontSize(14.0f)
                                                .onChange([](const std::string& value) {
                                                    state().resourcePath = value;
                                                })
                                                .build();

                                            ui.row("resource.actions")
                                                .size(leftWidth - panelInset * 2.0f, 42.0f)
                                                .gap(12.0f)
                                                .content([&] {
                                                    const float buttonWidth = (leftWidth - panelInset * 2.0f - 24.0f) / 3.0f;
                                                    components::button(ui, "resource.add.file")
                                                        .theme(tokens(), false)
                                                        .size(buttonWidth, 40.0f)
                                                        .text("Add Files")
                                                        .fontSize(14.0f)
                                                        .radius(10.0f)
                                                        .onClick([] {
                                                            if (!state().resourcePath.empty()) {
                                                                addResourcePath(state().resourcePath);
                                                                return;
                                                            }
                                                            addResourcePaths(openFileDialogMulti("Add resource files"));
                                                        })
                                                        .build();

                                                    components::button(ui, "resource.add.folder")
                                                        .theme(tokens(), false)
                                                        .size(buttonWidth, 40.0f)
                                                        .text("Add Folders")
                                                        .fontSize(14.0f)
                                                        .radius(10.0f)
                                                        .onClick([] {
                                                            if (!state().resourcePath.empty()) {
                                                                addResourcePath(state().resourcePath);
                                                                return;
                                                            }
                                                            addResourcePaths(openFolderDialogMulti("Add resource folders"));
                                                        })
                                                        .build();

                                                    components::button(ui, "resource.clear")
                                                        .theme(tokens(), false)
                                                        .size(buttonWidth, 40.0f)
                                                        .text("Clear")
                                                        .fontSize(14.0f)
                                                        .radius(10.0f)
                                                        .onClick([] {
                                                            state().resources.clear();
                                                            state().resourceScroll = 0.0f;
                                                            setStatus("Resource list cleared.");
                                                        })
                                                        .build();
                                                })
                                                .build();

                                            components::checkbox(ui, "option.delete")
                                                .theme(tokens())
                                                .size(leftWidth - panelInset * 2.0f, 30.0f)
                                                .checked(uiState.deleteAfterExit)
                                                .text("Delete extracted temp files after the app exits")
                                                .fontSize(14.0f)
                                                .onChange([](bool value) {
                                                    state().deleteAfterExit = value;
                                                })
                                                .build();

                                            ui.row("build.row")
                                                .size(leftWidth - panelInset * 2.0f, 54.0f)
                                                .gap(14.0f)
                                                .alignItems(core::Align::CENTER)
                                                .content([&] {
                                                    components::button(ui, "build.button")
                                                        .theme(tokens(), true)
                                                        .size(190.0f, 48.0f)
                                                        .text("Build Single EXE")
                                                        .fontSize(15.0f)
                                                        .radius(12.0f)
                                                        .onClick([] {
                                                            setStatus("Building package...");
                                                            const BuildResult result = buildPackage();
                                                            setStatus(result.message);
                                                        })
                                                        .build();

                                                    ui.text("build.note")
                                                        .size(std::max(0.0f, leftWidth - panelInset * 2.0f - 204.0f), 48.0f)
                                                        .text("Output uses this executable as the runtime shell.")
                                                        .fontSize(13.0f)
                                                        .lineHeight(17.0f)
                                                        .color(mutedText())
                                                        .wrap(true)
                                                        .verticalAlign(core::VerticalAlign::Center)
                                                        .build();
                                                })
                                                .build();

                                            ui.stack("status.box")
                                                .size(leftWidth - panelInset * 2.0f, 62.0f)
                                                .content([&] {
                                                    ui.rect("status.bg")
                                                        .size(leftWidth - panelInset * 2.0f, 62.0f)
                                                        .color(color(0.08f, 0.09f, 0.095f, 1.0f))
                                                        .radius(12.0f)
                                                        .border(1.0f, color(0.22f, 0.26f, 0.25f, 1.0f))
                                                        .build();

                                                    ui.text("status.text")
                                                        .size(leftWidth - panelInset * 2.0f - 24.0f, 62.0f)
                                                        .margin(12.0f, 0.0f)
                                                        .text(ellipsize(uiState.status, 190))
                                                        .fontSize(13.0f)
                                                        .lineHeight(17.0f)
                                                        .color(color(0.78f, 0.86f, 0.82f, 1.0f))
                                                        .wrap(true)
                                                        .verticalAlign(core::VerticalAlign::Center)
                                                        .build();
                                                })
                                                .build();
                                        })
                                        .build();
                                })
                                .build();

                            ui.stack("right.panel")
                                .size(rightWidth, bodyHeight)
                                .content([&] {
                                    components::panel(ui, "right.panel.bg", tokens())
                                        .size(rightWidth, bodyHeight)
                                        .radius(16.0f)
                                        .build();

                                    ui.column("right.content")
                                        .size(rightWidth - panelInset * 2.0f, bodyHeight - panelInset * 2.0f)
                                        .margin(panelInset)
                                        .gap(16.0f)
                                        .content([&] {
                                            ui.row("contents.header")
                                                .size(rightWidth - panelInset * 2.0f, 28.0f)
                                                .alignItems(core::Align::CENTER)
                                                .content([&] {
                                                    drawSectionTitle(ui, "contents.title", "Package Contents", rightWidth - panelInset * 2.0f - 90.0f);

                                                    ui.text("contents.count")
                                                        .size(90.0f, 26.0f)
                                                        .text(std::to_string(uiState.resources.size() + 1) + " items")
                                                        .fontSize(13.0f)
                                                        .lineHeight(16.0f)
                                                        .color(mutedText())
                                                        .horizontalAlign(core::HorizontalAlign::Right)
                                                        .verticalAlign(core::VerticalAlign::Center)
                                                        .build();
                                                })
                                                .build();

                                            ui.stack("contents.list")
                                                .size(rightWidth - panelInset * 2.0f, bodyHeight - panelInset * 2.0f - 44.0f)
                                                .content([&] {
                                                    ui.rect("contents.list.bg")
                                                        .size(rightWidth - panelInset * 2.0f, bodyHeight - panelInset * 2.0f - 44.0f)
                                                        .color(color(0.08f, 0.085f, 0.09f, 1.0f))
                                                        .radius(12.0f)
                                                        .border(1.0f, color(0.22f, 0.25f, 0.25f, 1.0f))
                                                        .build();

                                                    ui.stack("contents.list.inner")
                                                        .size(rightWidth - panelInset * 2.0f - 24.0f, bodyHeight - panelInset * 2.0f - 68.0f)
                                                        .margin(12.0f, 0.0f)
                                                        .content([&] {
                                                            drawResourceList(ui,
                                                                             rightWidth - panelInset * 2.0f - 24.0f,
                                                                             bodyHeight - panelInset * 2.0f - 68.0f);
                                                        })
                                                        .build();
                                                })
                                                .build();
                                        })
                                        .build();
                                })
                                .build();
                        })
                        .build();
                })
                .build();
        })
        .build();
}

} // namespace app
