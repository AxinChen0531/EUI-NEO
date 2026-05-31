#include "eui_neo.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace app {

namespace {

constexpr eui::Color kBackground{0.055f, 0.062f, 0.075f, 1.0f};
constexpr eui::Color kPanel{0.095f, 0.110f, 0.135f, 1.0f};
constexpr eui::Color kBorder{0.26f, 0.31f, 0.38f, 0.82f};
constexpr eui::Color kShadow{0.0f, 0.0f, 0.0f, 0.30f};

void checker(eui::Ui& ui, float width, float height) {
    ui.rect("bg")
        .size(width, height)
        .gradient({0.050f, 0.060f, 0.075f, 1.0f},
                  {0.090f, 0.105f, 0.125f, 1.0f})
        .build();

    constexpr float cell = 48.0f;
    const int columns = static_cast<int>(std::ceil(width / cell));
    const int rows = static_cast<int>(std::ceil(height / cell));
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            if (((x + y) % 2) == 0) {
                ui.rect("grid." + std::to_string(x) + "." + std::to_string(y))
                    .x(static_cast<float>(x) * cell)
                    .y(static_cast<float>(y) * cell)
                    .size(cell, cell)
                    .color({1.0f, 1.0f, 1.0f, 0.022f})
                    .build();
            }
        }
    }
}

void roundedCard(eui::Ui& ui,
                 const std::string& id,
                 float x,
                 float y,
                 float width,
                 float height,
                 const eui::Color& color,
                 float radius) {
    ui.rect(id)
        .x(x)
        .y(y)
        .size(width, height)
        .color(color)
        .radius(radius)
        .border(1.0f, kBorder)
        .shadow(22.0f, 0.0f, 10.0f, kShadow)
        .build();
}

void gradientCard(eui::Ui& ui,
                  const std::string& id,
                  float x,
                  float y,
                  float width,
                  float height,
                  eui::GradientDirection direction) {
    ui.rect(id)
        .x(x)
        .y(y)
        .size(width, height)
        .gradient({0.23f, 0.46f, 0.88f, 1.0f},
                  {0.90f, 0.40f, 0.58f, 1.0f},
                  direction)
        .radius(24.0f)
        .border(2.0f, {0.94f, 0.98f, 1.0f, 0.34f})
        .shadow(24.0f, 0.0f, 12.0f, {0.02f, 0.02f, 0.04f, 0.32f})
        .build();
}

void polygonSamples(eui::Ui& ui, float x, float y) {
    ui.polygon("poly.triangle")
        .x(x)
        .y(y)
        .size(150.0f, 130.0f)
        .points({{75.0f, 0.0f}, {150.0f, 120.0f}, {0.0f, 120.0f}})
        .color({0.36f, 0.76f, 0.62f, 0.95f})
        .opacity(0.96f)
        .rotate(-0.14f)
        .transformOrigin(0.5f, 0.5f)
        .build();

    ui.polygon("poly.diamond")
        .x(x + 172.0f)
        .y(y + 4.0f)
        .size(136.0f, 136.0f)
        .points({{68.0f, 0.0f}, {136.0f, 68.0f}, {68.0f, 136.0f}, {0.0f, 68.0f}})
        .color({0.86f, 0.64f, 0.28f, 0.92f})
        .opacity(0.92f)
        .scale(0.94f, 1.08f)
        .transformOrigin(0.5f, 0.5f)
        .build();

    ui.polygon("poly.hex")
        .x(x + 330.0f)
        .y(y + 2.0f)
        .size(160.0f, 138.0f)
        .points({{40.0f, 0.0f}, {120.0f, 0.0f}, {160.0f, 69.0f}, {120.0f, 138.0f}, {40.0f, 138.0f}, {0.0f, 69.0f}})
        .color({0.55f, 0.48f, 0.92f, 0.88f})
        .opacity(0.88f)
        .rotate(0.10f)
        .transformOrigin(0.5f, 0.5f)
        .build();
}

void blurSample(eui::Ui& ui, float x, float y) {
    ui.rect("blur.backdrop.base")
        .x(x)
        .y(y)
        .size(246.0f, 132.0f)
        .gradient({0.16f, 0.22f, 0.34f, 1.0f},
                  {0.42f, 0.24f, 0.54f, 1.0f},
                  eui::GradientDirection::Horizontal)
        .radius(26.0f)
        .border(1.0f, {0.90f, 0.94f, 1.0f, 0.20f})
        .build();

    for (int i = 0; i < 7; ++i) {
        const float stripeX = x + 18.0f + static_cast<float>(i) * 32.0f;
        ui.rect("blur.backdrop.stripe." + std::to_string(i))
            .x(stripeX)
            .y(y + 14.0f)
            .size(14.0f, 104.0f)
            .color(i % 2 == 0 ? eui::Color{0.98f, 0.76f, 0.30f, 0.88f}
                              : eui::Color{0.30f, 0.82f, 0.92f, 0.88f})
            .radius(7.0f)
            .rotate(i % 2 == 0 ? 0.14f : -0.12f)
            .transformOrigin(0.5f, 0.5f)
            .build();
    }

    ui.rect("blur.glass")
        .x(x + 48.0f)
        .y(y + 28.0f)
        .size(150.0f, 76.0f)
        .color({0.78f, 0.92f, 1.0f, 0.24f})
        .radius(22.0f)
        .border(1.0f, {0.96f, 0.99f, 1.0f, 0.44f})
        .shadow(18.0f, 0.0f, 8.0f, {0.0f, 0.0f, 0.0f, 0.22f})
        .blur(18.0f)
        .build();
}

void composePrimitiveFixture(eui::Ui& ui, float width, float height) {
    checker(ui, width, height);

    ui.rect("safe-frame")
        .x(32.0f)
        .y(32.0f)
        .size(width - 64.0f, height - 64.0f)
        .color(kPanel)
        .radius(30.0f)
        .border(1.0f, {0.40f, 0.48f, 0.58f, 0.52f})
        .shadow(30.0f, 0.0f, 16.0f, {0.0f, 0.0f, 0.0f, 0.34f})
        .build();

    roundedCard(ui, "solid.rounded", 74.0f, 78.0f, 220.0f, 150.0f, {0.28f, 0.56f, 0.90f, 1.0f}, 32.0f);
    roundedCard(ui, "solid.alpha", 326.0f, 78.0f, 220.0f, 150.0f, {0.92f, 0.44f, 0.56f, 0.62f}, 18.0f);
    roundedCard(ui, "solid.pill", 578.0f, 78.0f, 260.0f, 150.0f, {0.34f, 0.78f, 0.62f, 0.92f}, 75.0f);

    gradientCard(ui, "gradient.horizontal", 74.0f, 270.0f, 300.0f, 154.0f, eui::GradientDirection::Horizontal);
    gradientCard(ui, "gradient.vertical", 414.0f, 270.0f, 300.0f, 154.0f, eui::GradientDirection::Vertical);

    ui.rect("transform.rotated")
        .x(740.0f)
        .y(292.0f)
        .size(144.0f, 104.0f)
        .gradient({0.92f, 0.64f, 0.28f, 1.0f},
                  {0.72f, 0.34f, 0.84f, 1.0f})
        .radius(24.0f)
        .border(2.0f, {1.0f, 1.0f, 1.0f, 0.28f})
        .shadow(28.0f, 0.0f, 14.0f, {0.0f, 0.0f, 0.0f, 0.32f})
        .rotate(0.34f)
        .scale(1.10f, 0.92f)
        .transformOrigin(0.5f, 0.5f)
        .build();

    polygonSamples(ui, 96.0f, 492.0f);
    blurSample(ui, 420.0f, 496.0f);

    ui.rect("overlap.base")
        .x(704.0f)
        .y(492.0f)
        .size(190.0f, 126.0f)
        .color({0.20f, 0.30f, 0.45f, 0.86f})
        .radius(28.0f)
        .border(1.0f, {0.90f, 0.94f, 1.0f, 0.20f})
        .build();

    ui.rect("overlap.top")
        .x(784.0f)
        .y(528.0f)
        .size(164.0f, 112.0f)
        .color({0.94f, 0.46f, 0.34f, 0.68f})
        .radius(22.0f)
        .border(1.0f, {1.0f, 1.0f, 1.0f, 0.24f})
        .shadow(18.0f, -4.0f, 10.0f, {0.0f, 0.0f, 0.0f, 0.24f})
        .build();
}

} // namespace

const DslAppConfig& dslAppConfig() {
    static const DslAppConfig config = DslAppConfig{}
        .title("EUI Primitive Parity")
        .pageId("primitive-parity")
        .clearColor(kBackground)
        .windowSize(960, 720)
        .fps(1.0);
    return config;
}

void compose(eui::Ui& ui, const eui::Screen& screen) {
    composePrimitiveFixture(ui, screen.width, screen.height);
}

} // namespace app
