#pragma once

namespace core {
struct Color;
struct Rect;
} // namespace core

namespace core::render {

class RenderBackend {
public:
    virtual ~RenderBackend() = default;

    virtual void makeCurrent() = 0;
    virtual void beginFrame(int framebufferWidth, int framebufferHeight, float dpiScale) = 0;
    virtual void present() = 0;
    virtual bool ensureRenderCache(int width, int height) = 0;
    virtual bool renderCacheWasRecreated() const = 0;
    virtual void releaseRenderCache() = 0;
    virtual void beginRenderCacheFrame(int width, int height) = 0;
    virtual void endRenderCacheFrame() = 0;
    virtual void blitRenderCache(int width, int height) = 0;
    virtual void clear(const core::Color& color) = 0;
    virtual void setScissor(bool enabled, const core::Rect& rect, int framebufferHeight) = 0;
};

inline RenderBackend*& activeRenderBackendSlot() {
    static thread_local RenderBackend* backend = nullptr;
    return backend;
}

inline RenderBackend* activeRenderBackend() {
    return activeRenderBackendSlot();
}

class ScopedRenderBackend {
public:
    explicit ScopedRenderBackend(RenderBackend& backend)
        : previous_(activeRenderBackendSlot()) {
        activeRenderBackendSlot() = &backend;
    }

    ~ScopedRenderBackend() {
        activeRenderBackendSlot() = previous_;
    }

    ScopedRenderBackend(const ScopedRenderBackend&) = delete;
    ScopedRenderBackend& operator=(const ScopedRenderBackend&) = delete;

private:
    RenderBackend* previous_ = nullptr;
};

} // namespace core::render
