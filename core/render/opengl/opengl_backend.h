#pragma once

#include "core/render/render_backend.h"

#include <functional>

namespace core::render::opengl {

class OpenGLRenderBackend final : public RenderBackend {
public:
    using Callback = std::function<void()>;

    OpenGLRenderBackend(Callback makeCurrent, Callback present);

    void makeCurrent() override;
    void beginFrame(int framebufferWidth, int framebufferHeight, float dpiScale) override;
    void present() override;
    bool ensureRenderCache(int width, int height) override;
    bool renderCacheWasRecreated() const override;
    void releaseRenderCache() override;
    void beginRenderCacheFrame(int width, int height) override;
    void endRenderCacheFrame() override;
    void blitRenderCache(int width, int height) override;
    void clear(const core::Color& color) override;
    void setScissor(bool enabled, const core::Rect& rect, int framebufferHeight) override;

private:
    Callback makeCurrent_;
    Callback present_;
    unsigned int cacheFramebuffer_ = 0;
    unsigned int cacheTexture_ = 0;
    int cacheWidth_ = 0;
    int cacheHeight_ = 0;
    bool cacheRecreated_ = false;
};

} // namespace core::render::opengl
