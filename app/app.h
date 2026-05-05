#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace app {

const char* windowTitle();
bool showFrameCountInTitle();
double frameRateLimit();
int initialWindowWidth();
int initialWindowHeight();
bool trayEnabled();
const char* trayTitle();
const char* trayIconPath();
bool initialize(GLFWwindow* window);
bool update(GLFWwindow* window, float deltaSeconds, int windowWidth, int windowHeight, float dpiScale, float pointerScale);
bool isAnimating();
void render(int windowWidth, int windowHeight, float dpiScale);
void releaseGraphicsResources();
void shutdown();

} // namespace app
