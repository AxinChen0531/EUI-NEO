#pragma once

#include "app/app.h"

#include <glad/glad.h>

#include "3rd/stb_image.h"
#include "core/dsl_runtime.h"
#include "core/network.h"
#include "core/text.h"

#include <filesystem>
#include <string>
#include <vector>

namespace app {

struct DslAppConfig {
    const char* titleValue = "App";
    const char* pageIdValue = "app";
    core::Color clearColorValue = {0.16f, 0.18f, 0.20f, 1.0f};
    int windowWidthValue = 800;
    int windowHeightValue = 600;
    bool showFrameCountInTitleValue = false;
    double fpsValue = 0.0;
    const char* iconPathValue = "assets/icon.png";
    const char* textFontFileValue = "";
    const char* iconFontFileValue = "";
    bool trayEnabledValue = false;
    const char* trayTitleValue = "";
    const char* trayIconPathValue = "";

    DslAppConfig& title(const char* value) { titleValue = value; return *this; }
    DslAppConfig& pageId(const char* value) { pageIdValue = value; return *this; }
    DslAppConfig& clearColor(const core::Color& value) { clearColorValue = value; return *this; }
    DslAppConfig& background(const core::Color& value) { return clearColor(value); }
    DslAppConfig& windowSize(int width, int height) {
        windowWidthValue = width;
        windowHeightValue = height;
        return *this;
    }
    DslAppConfig& windowWidth(int value) { windowWidthValue = value; return *this; }
    DslAppConfig& windowHeight(int value) { windowHeightValue = value; return *this; }
    DslAppConfig& showFrameCountInTitle(bool value = true) {
        showFrameCountInTitleValue = value;
        return *this;
    }
    DslAppConfig& fps(double value) { fpsValue = value; return *this; }
    DslAppConfig& iconPath(const char* value) { iconPathValue = value; return *this; }
    DslAppConfig& textFont(const char* value) { textFontFileValue = value; return *this; }
    DslAppConfig& iconFont(const char* value) { iconFontFileValue = value; return *this; }
    DslAppConfig& fonts(const char* textFont, const char* iconFont = "") {
        textFontFileValue = textFont;
        iconFontFileValue = iconFont;
        return *this;
    }
    DslAppConfig& tray(bool value = true) {
        trayEnabledValue = value;
        return *this;
    }
    DslAppConfig& trayTitle(const char* value) {
        trayTitleValue = value;
        return *this;
    }
    DslAppConfig& trayIcon(const char* value) {
        trayIconPathValue = value;
        return *this;
    }
};

const DslAppConfig& dslAppConfig();
void compose(core::dsl::Ui& ui, const core::dsl::Screen& screen);

namespace detail {

inline core::dsl::Runtime& dslRuntime() {
    static core::dsl::Runtime runtime;
    return runtime;
}

struct DslAppState {
    bool composed = false;
    bool iconApplied = false;
    float logicalWidth = 0.0f;
    float logicalHeight = 0.0f;
};

inline DslAppState& dslAppState() {
    static DslAppState state;
    return state;
}

inline std::string resolveIconPath(const char* iconPath) {
    if (iconPath == nullptr || iconPath[0] == '\0') {
        return {};
    }

    namespace fs = std::filesystem;
    std::error_code error;
    const fs::path requested(iconPath);
    const fs::path current = fs::current_path(error);
    std::vector<fs::path> candidates;
    candidates.push_back(requested);
    if (!error) {
        candidates.push_back(current / requested);
        candidates.push_back(current / "assets" / requested.filename());
    }

    for (const fs::path& candidate : candidates) {
        error.clear();
        if (fs::exists(candidate, error) && !error) {
            return fs::absolute(candidate, error).string();
        }
    }
    return {};
}

inline void applyWindowIcon(GLFWwindow* window) {
    if (window == nullptr) {
        return;
    }

    const std::string iconPath = resolveIconPath(dslAppConfig().iconPathValue);
    if (iconPath.empty()) {
        return;
    }

    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(0);
    unsigned char* pixels = stbi_load(iconPath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (pixels == nullptr || width <= 0 || height <= 0) {
        if (pixels != nullptr) {
            stbi_image_free(pixels);
        }
        return;
    }

    GLFWimage image{};
    image.width = width;
    image.height = height;
    image.pixels = pixels;
    glfwSetWindowIcon(window, 1, &image);
    stbi_image_free(pixels);
}

} // namespace detail

const char* windowTitle() {
    return dslAppConfig().titleValue;
}

bool showFrameCountInTitle() {
    return dslAppConfig().showFrameCountInTitleValue;
}

double frameRateLimit() {
    return dslAppConfig().fpsValue;
}

int initialWindowWidth() {
    return dslAppConfig().windowWidthValue;
}

int initialWindowHeight() {
    return dslAppConfig().windowHeightValue;
}

bool trayEnabled() {
    return dslAppConfig().trayEnabledValue;
}

const char* trayTitle() {
    const DslAppConfig& config = dslAppConfig();
    return (config.trayTitleValue != nullptr && config.trayTitleValue[0] != '\0')
        ? config.trayTitleValue
        : config.titleValue;
}

const char* trayIconPath() {
    const DslAppConfig& config = dslAppConfig();
    return (config.trayIconPathValue != nullptr && config.trayIconPathValue[0] != '\0')
        ? config.trayIconPathValue
        : config.iconPathValue;
}

bool initialize(GLFWwindow* window) {
    const DslAppConfig& config = dslAppConfig();
    core::TextPrimitive::setDefaultFontFiles(
        config.textFontFileValue != nullptr ? config.textFontFileValue : "",
        config.iconFontFileValue != nullptr ? config.iconFontFileValue : "");

    detail::DslAppState& state = detail::dslAppState();
    if (!state.iconApplied) {
        detail::applyWindowIcon(window);
        state.iconApplied = true;
    }
    return detail::dslRuntime().initialize(window);
}

bool update(GLFWwindow* window, float deltaSeconds, int windowWidth, int windowHeight, float dpiScale, float pointerScale) {
    if (windowWidth <= 0 || windowHeight <= 0 || dpiScale <= 0.0f) {
        return false;
    }

    const DslAppConfig& config = dslAppConfig();
    const float logicalWidth = static_cast<float>(windowWidth) / dpiScale;
    const float logicalHeight = static_cast<float>(windowHeight) / dpiScale;
    detail::DslAppState& state = detail::dslAppState();

    const auto composeFrame = [&] {
        detail::dslRuntime().compose(config.pageIdValue, logicalWidth, logicalHeight, [](core::dsl::Ui& ui, const core::dsl::Screen& screen) {
            compose(ui, screen);
        });
        state.composed = true;
        state.logicalWidth = logicalWidth;
        state.logicalHeight = logicalHeight;
    };

    if (!state.composed || state.logicalWidth != logicalWidth || state.logicalHeight != logicalHeight) {
        composeFrame();
    }

    bool changed = false;
    if (core::network::consumeAnyTextReady()) {
        composeFrame();
        detail::dslRuntime().markFullRedraw();
        changed = true;
    }

    changed = detail::dslRuntime().update(window, deltaSeconds, pointerScale, dpiScale) || changed;
    if (detail::dslRuntime().needsCompose()) {
        composeFrame();
        changed = detail::dslRuntime().update(window, 0.0f, pointerScale, dpiScale) || changed;
        changed = true;
    }

    return changed;
}

bool isAnimating() {
    return detail::dslRuntime().isAnimating();
}

void render(int windowWidth, int windowHeight, float dpiScale) {
    if (windowWidth <= 0 || windowHeight <= 0 || dpiScale <= 0.0f) {
        return;
    }

    const core::Color clearColor = dslAppConfig().clearColorValue;
    detail::dslRuntime().render(windowWidth, windowHeight, dpiScale, clearColor);
}

void releaseGraphicsResources() {
    detail::dslRuntime().releaseGraphicsResources();
}

void shutdown() {
    detail::dslRuntime().shutdown();
    core::network::shutdown();
}

} // namespace app
