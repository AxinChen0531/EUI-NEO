#pragma once

#include "eui/app.h"
#include "core/platform/async.h"
#include "core/platform/performance_stats.h"
#include "core/platform/platform.h"
#include "core/render/render_backend.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

namespace app {

struct AppRunner {
    bool paintRequested = true;
    bool trayAvailable = false;
    bool hiddenToTray = false;
    int renderedFrames = 0;
    double lastTitleUpdate = 0.0;
    double nextFrameTime = 0.0;
    double frameInterval = 1.0 / 60.0;
    double lastFrameTime = 0.0;
    double lastFrameRateLimit = 0.0;
    double lastRefreshRateUpdate = 0.0;
    double accumulatedRenderMs = 0.0;
    double accumulatedDirtyRectCount = 0.0;
    double accumulatedDirtyAreaPercent = 0.0;
    double accumulatedBlitAreaPercent = 0.0;
    double accumulatedRectDraws = 0.0;
    double accumulatedPolygonDraws = 0.0;
    double accumulatedTextPrepares = 0.0;
    double accumulatedTextDraws = 0.0;
    double accumulatedImageDraws = 0.0;
    double accumulatedClearCalls = 0.0;
    double accumulatedRenderDirectPasses = 0.0;
    double accumulatedCacheBlits = 0.0;
    int measuredRenderFrames = 0;
    int measuredRenderStatsFrames = 0;
    int fullPaintFrames = 0;
    int renderCacheFrames = 0;
    int renderCacheRecreatedFrames = 0;
    bool renderedSinceLastClock = false;
    core::platform::ProcessUsageSampler usageSampler;

    void resetTiming(double now) {
        lastTitleUpdate = now;
        nextFrameTime = now;
        lastFrameTime = now;
        usageSampler.reset();
    }

    float consumeFrameDelta(double now) {
        const float deltaSeconds = static_cast<float>(now - lastFrameTime);
        lastFrameTime = now;
        return deltaSeconds;
    }

    bool initializeTray() {
        if (!trayEnabled()) {
            trayAvailable = false;
            return false;
        }
        trayAvailable = core::platform::initializeTray({
            trayTitle(),
            trayIconPath()
        });
        return trayAvailable;
    }

    void pollTray(bool wait = false) {
        core::platform::pollTray(wait);
    }

    bool consumeTrayExitRequested() {
        return core::platform::consumeTrayExitRequested();
    }

    bool consumeTrayShowRequested() {
        return core::platform::consumeTrayShowRequested();
    }

    bool consumeUpdateRequest() {
        const bool asyncReady = core::async::dispatchReady();
        const bool updateRequested = core::platform::consumeUiUpdate();
        return updateRequested || asyncReady;
    }

    bool consumeFrameRequest() {
        return core::platform::consumeFrameRequest();
    }

    bool anyAnimating(bool childAnimating) const {
        return isAnimating() || childAnimating;
    }

    void updateFrameInterval(double refreshRate, double now, bool force = false) {
        const double limit = frameRateLimit();
        if (!force && limit == lastFrameRateLimit && now - lastRefreshRateUpdate < 0.5) {
            return;
        }

        refreshRate = std::clamp(refreshRate, 30.0, 500.0);
        if (limit > 0.0) {
            refreshRate = std::min(refreshRate, limit);
        }
        frameInterval = 1.0 / std::max(1.0, refreshRate);
        lastFrameRateLimit = limit;
        lastRefreshRateUpdate = now;
    }

    void markRendered() {
        paintRequested = false;
        ++renderedFrames;
        renderedSinceLastClock = true;
    }

    void recordRenderDuration(double milliseconds) {
        if (milliseconds < 0.0 || milliseconds > 10000.0) {
            return;
        }
        accumulatedRenderMs += milliseconds;
        ++measuredRenderFrames;
    }

    void recordRenderStats(const core::render::RenderFrameStats& stats) {
        if (!stats.rendered) {
            return;
        }

        const double framePixels = static_cast<double>(std::max(1, stats.framebufferWidth)) *
                                   static_cast<double>(std::max(1, stats.framebufferHeight));
        accumulatedDirtyRectCount += static_cast<double>(stats.dirtyRectCount);
        accumulatedDirtyAreaPercent += stats.fullPaint
            ? 100.0
            : (static_cast<double>(stats.dirtyPixels) * 100.0 / framePixels);
        accumulatedBlitAreaPercent += static_cast<double>(stats.blitPixels) * 100.0 / framePixels;
        accumulatedRectDraws += static_cast<double>(stats.rectDraws);
        accumulatedPolygonDraws += static_cast<double>(stats.polygonDraws);
        accumulatedTextPrepares += static_cast<double>(stats.textPrepares);
        accumulatedTextDraws += static_cast<double>(stats.textDraws);
        accumulatedImageDraws += static_cast<double>(stats.imageDraws);
        accumulatedClearCalls += static_cast<double>(stats.clearCalls);
        accumulatedRenderDirectPasses += static_cast<double>(stats.renderDirectPasses);
        accumulatedCacheBlits += static_cast<double>(stats.cacheBlits);
        ++measuredRenderStatsFrames;
        if (stats.fullPaint) {
            ++fullPaintFrames;
        }
        if (stats.usedRenderCache) {
            ++renderCacheFrames;
        }
        if (stats.renderCacheRecreated) {
            ++renderCacheRecreatedFrames;
        }
    }

    template <typename SetTitleFn>
    void updateFrameTitle(double now, SetTitleFn&& setTitle) {
        if (!showDebugStatsInTitle()) {
            return;
        }
        const double elapsed = now - lastTitleUpdate;
        if (elapsed < 1.0) {
            return;
        }

        const core::platform::ProcessUsageSample usage = usageSampler.sample(elapsed);
        const double averageRenderMs = measuredRenderFrames > 0
            ? accumulatedRenderMs / static_cast<double>(measuredRenderFrames)
            : std::numeric_limits<double>::quiet_NaN();

        char cpuText[32];
        if (usage.hasCpuPercent) {
            std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", usage.cpuPercent);
        } else {
            std::snprintf(cpuText, sizeof(cpuText), "n/a");
        }

        const double statsFrames = static_cast<double>(std::max(1, measuredRenderStatsFrames));
        const double averageDirtyRects = accumulatedDirtyRectCount / statsFrames;
        const double averageDirtyAreaPercent = accumulatedDirtyAreaPercent / statsFrames;
        const double averageBlitAreaPercent = accumulatedBlitAreaPercent / statsFrames;
        const double averageRectDraws = accumulatedRectDraws / statsFrames;
        const double averagePolygonDraws = accumulatedPolygonDraws / statsFrames;
        const double averageTextPrepares = accumulatedTextPrepares / statsFrames;
        const double averageTextDraws = accumulatedTextDraws / statsFrames;
        const double averageImageDraws = accumulatedImageDraws / statsFrames;
        const double averageClearCalls = accumulatedClearCalls / statsFrames;
        const double averageRenderDirectPasses = accumulatedRenderDirectPasses / statsFrames;
        const double averageCacheBlits = accumulatedCacheBlits / statsFrames;
        const double fullPaintPercent = static_cast<double>(fullPaintFrames) * 100.0 / statsFrames;
        const double cachePercent = static_cast<double>(renderCacheFrames) * 100.0 / statsFrames;
        const double cacheRecreatedPercent = static_cast<double>(renderCacheRecreatedFrames) * 100.0 / statsFrames;

        char renderStatsText[256];
        if (measuredRenderStatsFrames > 0) {
            std::snprintf(renderStatsText,
                          sizeof(renderStatsText),
                          " | Dirty %.1f/%.0f%% | Draw R%.0f P%.0f TP%.0f T%.0f I%.0f | Pass %.1f C%.1f B%.1f/%.0f%% | Full %.0f%% Cache %.0f%% Re %.0f%%",
                          averageDirtyRects,
                          averageDirtyAreaPercent,
                          averageRectDraws,
                          averagePolygonDraws,
                          averageTextPrepares,
                          averageTextDraws,
                          averageImageDraws,
                          averageRenderDirectPasses,
                          averageClearCalls,
                          averageCacheBlits,
                          averageBlitAreaPercent,
                          fullPaintPercent,
                          cachePercent,
                          cacheRecreatedPercent);
        } else {
            renderStatsText[0] = '\0';
        }

        char title[512];
        if (!usage.hasGpuPercent && std::isnan(averageRenderMs)) {
            std::snprintf(title,
                          sizeof(title),
                          "%s - %.0f FPS | CPU %s | GPU n/a%s",
                          windowTitle(),
                          renderedFrames / elapsed,
                          cpuText,
                          renderStatsText);
        } else if (!usage.hasGpuPercent) {
            std::snprintf(title,
                          sizeof(title),
                          "%s - %.0f FPS | CPU %s | GPU %.2f ms%s",
                          windowTitle(),
                          renderedFrames / elapsed,
                          cpuText,
                          averageRenderMs,
                          renderStatsText);
        } else if (std::isnan(averageRenderMs)) {
            std::snprintf(title,
                          sizeof(title),
                          "%s - %.0f FPS | CPU %s | GPU %.0f%%%s",
                          windowTitle(),
                          renderedFrames / elapsed,
                          cpuText,
                          usage.gpuPercent,
                          renderStatsText);
        } else {
            std::snprintf(title,
                          sizeof(title),
                          "%s - %.0f FPS | CPU %s | GPU %.0f%% | Render %.2f ms%s",
                          windowTitle(),
                          renderedFrames / elapsed,
                          cpuText,
                          usage.gpuPercent,
                          averageRenderMs,
                          renderStatsText);
        }
        setTitle(title);
        renderedFrames = 0;
        accumulatedRenderMs = 0.0;
        accumulatedDirtyRectCount = 0.0;
        accumulatedDirtyAreaPercent = 0.0;
        accumulatedBlitAreaPercent = 0.0;
        accumulatedRectDraws = 0.0;
        accumulatedPolygonDraws = 0.0;
        accumulatedTextPrepares = 0.0;
        accumulatedTextDraws = 0.0;
        accumulatedImageDraws = 0.0;
        accumulatedClearCalls = 0.0;
        accumulatedRenderDirectPasses = 0.0;
        accumulatedCacheBlits = 0.0;
        measuredRenderFrames = 0;
        measuredRenderStatsFrames = 0;
        fullPaintFrames = 0;
        renderCacheFrames = 0;
        renderCacheRecreatedFrames = 0;
        lastTitleUpdate = now;
    }

    void advanceFrameClock(double now, bool animating) {
        if (animating || renderedSinceLastClock) {
            nextFrameTime += frameInterval;
            if (nextFrameTime <= now || nextFrameTime > now + frameInterval * 2.0) {
                nextFrameTime = now + frameInterval;
            }
        } else {
            nextFrameTime = now;
        }
        renderedSinceLastClock = false;
    }
};

} // namespace app
