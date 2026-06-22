#pragma once

#include "core/render/render_backend.h"
#include "core/window/window_types.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace core::render::opengl {

class OpenGLRenderBackend final : public RenderBackend {
public:
    explicit OpenGLRenderBackend(core::window::Handle window, RenderBackend* shareContext = nullptr);
    ~OpenGLRenderBackend() override;

    OpenGLRenderBackend(const OpenGLRenderBackend&) = delete;
    OpenGLRenderBackend& operator=(const OpenGLRenderBackend&) = delete;

    bool initialize() override;
    bool valid() const override;

    void makeCurrent() override;
    void beginFrame(const RenderSurface& surface) override;
    void present() override;
    bool ensureRenderCache(int width, int height) override;
    bool renderCacheWasRecreated() const override;
    void releaseRenderCache() override;
    void beginRenderCacheFrame(int width,
                               int height,
                               const std::vector<core::Rect>& repaintRects = {}) override;
    void endRenderCacheFrame() override;
    void blitRenderCache(int width,
                         int height,
                         RenderCacheBlitMode mode = RenderCacheBlitMode::Full,
                         const std::vector<core::Rect>& dirtyRects = {}) override;
    void clear(const core::Color& color) override;
    void setScissor(bool enabled, const core::Rect& rect, int framebufferHeight) override;
    void prepareBackdropBlur(const core::Rect& bounds, float blur, int windowWidth, int windowHeight) override;
    void drawRoundedRect(const RoundedRectDrawCommand& command, int windowWidth, int windowHeight) override;
    void drawText(const TextDrawCommand& command, int windowWidth, int windowHeight) override;
    TextureHandle createTexture(const unsigned char* pixels, int width, int height) override;
    bool updateTexture(TextureHandle handle, const unsigned char* pixels, int width, int height) override;
    void destroyTexture(TextureHandle handle) override;
    void drawTexture(TextureHandle handle,
                     const float* vertices,
                     std::size_t vertexFloatCount,
                     const core::Color& tint,
                     const core::Rect& rect,
                     float radius,
                     int windowWidth,
                     int windowHeight) override;

private:
    void releasePrimitiveResources();
    void releaseTextResources();
    bool ensureImageResources();
    unsigned int compileImageShader(unsigned int type, const char* source) const;
    void releaseImageResources();
    void resetStateCache();
    void useProgram(unsigned int program);
    void bindVertexArray(unsigned int vao);
    void bindArrayBuffer(unsigned int buffer);
    void activeTextureUnit(unsigned int unit);
    void bindTexture2D(unsigned int texture);
    void setBlendEnabled(bool enabled);
    void setStandardAlphaBlend();
    std::vector<core::Rect> resolveRenderCacheBlitRects(int width,
                                                        int height,
                                                        RenderCacheBlitMode mode,
                                                        const std::vector<core::Rect>& dirtyRects);
    void recordRenderCacheBlitHistory(std::uint64_t generation, bool fullSync, const std::vector<core::Rect>& rects);
    void invalidateRenderCacheSync();

    core::window::Handle window_ = nullptr;
    RenderBackend* shareContext_ = nullptr;
    void* context_ = nullptr;
    bool initialized_ = false;
    bool damagePresentSupported_ = false;
    unsigned int cacheFramebuffer_ = 0;
    unsigned int cacheTexture_ = 0;
    int cacheWidth_ = 0;
    int cacheHeight_ = 0;
    bool cacheRecreated_ = false;
    int framebufferWidth_ = 0;
    int framebufferHeight_ = 0;
    core::Rect cacheRenderArea_{};
    struct RenderCacheHistoryEntry {
        std::uint64_t generation = 0;
        bool full = false;
        std::vector<core::Rect> rects;
    };
    std::uint64_t renderCacheGeneration_ = 0;
    std::vector<std::uint64_t> backbufferCacheGenerations_{0, 0};
    std::vector<RenderCacheHistoryEntry> renderCacheHistory_;
    std::size_t currentBackbuffer_ = 0;
    unsigned int imageVao_ = 0;
    unsigned int imageVbo_ = 0;
    unsigned int imageShaderProgram_ = 0;
    int imageWindowSizeLocation_ = -1;
    int imageTextureLocation_ = -1;
    int imageTintLocation_ = -1;
    int imageRectLocation_ = -1;
    int imageRadiusLocation_ = -1;
    bool stateCacheValid_ = false;
    bool blendEnabled_ = false;
    bool alphaBlendSet_ = false;
    bool scissorEnabledState_ = false;
    int scissorX_ = 0;
    int scissorY_ = 0;
    int scissorWidth_ = 0;
    int scissorHeight_ = 0;
    unsigned int currentProgram_ = 0;
    unsigned int currentVao_ = 0;
    unsigned int currentArrayBuffer_ = 0;
    unsigned int currentTextureUnit_ = 0;
    unsigned int currentTexture2D_[8] = {};
};

} // namespace core::render::opengl
