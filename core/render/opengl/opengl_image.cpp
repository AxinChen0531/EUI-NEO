#include "core/render/image.h"
#include "core/render/image_source.h"
#include "core/window/window_backend.h"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace core {
namespace {

struct TextureRecord {
    GLuint texture = 0;
    int width = 0;
    int height = 0;
    int references = 0;
};

std::unordered_map<std::string, TextureRecord> gTextureCache;

GLuint createTexture(const unsigned char* pixels, int width, int height) {
    if (pixels == nullptr || width <= 0 || height <= 0) {
        return 0;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

void updateTexturePixels(GLuint texture, const unsigned char* pixels, int width, int height) {
    if (texture == 0 || pixels == nullptr || width <= 0 || height <= 0) {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace

struct ImagePrimitive::Impl {
    struct SharedResources;

    bool initialize();
    void destroy();

    void setSource(const std::string& source);
    void setFlipVertically(bool value);
    void setBounds(float x, float y, float width, float height);
    void setTint(const Color& tint);
    void setCornerRadius(float radius);
    void setOpacity(float opacity);
    void setTransform(const Transform& transform);
    void setTransformMatrix(const TransformMatrix& matrix);
    void setFit(ImageFit fit);
    void setCoverViewport(bool enabled, const Vec2& canvasSize, const Vec2& viewportOffset);

    bool updateTexture();
    bool hasPendingLoad() const;
    bool isAnimating() const;
    void render(int windowWidth, int windowHeight);

    static bool isSourceReady(const std::string& source);
    static bool consumeRemoteImageReady();
    static void releaseCachedTextures();

    static SharedResources& sharedResources();
    static bool retainSharedResources();
    static void releaseSharedResources();
    static unsigned int compileShader(unsigned int type, const char* source);
    static unsigned int acquireTexture(const std::string& source, bool flipVertically, bool* pending, int* width, int* height, std::string* cacheKey);
    static void releaseCachedTexture(const std::string& cacheKey);

    bool updateGifTexture(const std::string& resolvedPath);
    void releaseOwnedTexture();
    void releaseCachedTextureReference();
    Vec3 transformPoint(float x, float y) const;
    void rebuildVertices(float* vertices) const;

    std::string source_;
    std::string loadedSource_;
    bool flipVertically_ = false;
    bool loadedFlipVertically_ = false;
    bool pendingLoad_ = false;
    Rect bounds_;
    Color tint_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float radius_ = 0.0f;
    float opacity_ = 1.0f;
    Transform transform_;
    TransformMatrix transformMatrix_;
    bool hasTransformMatrix_ = false;
    ImageFit fit_ = ImageFit::Cover;
    bool hasCoverViewport_ = false;
    Vec2 coverViewportSize_;
    Vec2 coverViewportOffset_;
    GLuint texture_ = 0;
    bool ownsTexture_ = false;
    std::string loadedTextureCacheKey_;
    int textureWidth_ = 0;
    int textureHeight_ = 0;

    std::string loadedGifPath_;
    bool loadedGifFlipVertically_ = false;
    std::shared_ptr<const render::image::GifFrameData> gifFrames_;
    std::vector<int> gifDelays_;
    int gifFrameCount_ = 0;
    int gifFrameIndex_ = 0;
    double gifNextFrameTime_ = 0.0;

    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint shaderProgram_ = 0;
    GLint windowSizeLocation_ = -1;
    GLint textureLocation_ = -1;
    GLint tintLocation_ = -1;
    GLint rectLocation_ = -1;
    GLint radiusLocation_ = -1;
};

struct ImagePrimitive::Impl::SharedResources {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shaderProgram = 0;
    GLint windowSizeLocation = -1;
    GLint textureLocation = -1;
    GLint tintLocation = -1;
    GLint rectLocation = -1;
    GLint radiusLocation = -1;
    int references = 0;
};

bool ImagePrimitive::Impl::initialize() {
    if (!retainSharedResources()) {
        return false;
    }

    SharedResources& resources = sharedResources();
    vao_ = resources.vao;
    vbo_ = resources.vbo;
    shaderProgram_ = resources.shaderProgram;
    windowSizeLocation_ = resources.windowSizeLocation;
    textureLocation_ = resources.textureLocation;
    tintLocation_ = resources.tintLocation;
    rectLocation_ = resources.rectLocation;
    radiusLocation_ = resources.radiusLocation;
    return true;
}

void ImagePrimitive::Impl::destroy() {
    releaseOwnedTexture();
    if (shaderProgram_ != 0) {
        releaseSharedResources();
    }
    vao_ = 0;
    vbo_ = 0;
    shaderProgram_ = 0;
    windowSizeLocation_ = -1;
    textureLocation_ = -1;
    tintLocation_ = -1;
    rectLocation_ = -1;
    radiusLocation_ = -1;
}

void ImagePrimitive::Impl::setSource(const std::string& source) {
    source_ = source;
}

void ImagePrimitive::Impl::setFlipVertically(bool value) {
    flipVertically_ = value;
}

void ImagePrimitive::Impl::setBounds(float x, float y, float width, float height) {
    bounds_ = {x, y, width, height};
}

void ImagePrimitive::Impl::setTint(const Color& tint) {
    tint_ = tint;
}

void ImagePrimitive::Impl::setCornerRadius(float radius) {
    radius_ = std::max(0.0f, radius);
}

void ImagePrimitive::Impl::setOpacity(float opacity) {
    opacity_ = std::clamp(opacity, 0.0f, 1.0f);
}

void ImagePrimitive::Impl::setTransform(const Transform& transform) {
    transform_ = transform;
    hasTransformMatrix_ = false;
}

void ImagePrimitive::Impl::setTransformMatrix(const TransformMatrix& matrix) {
    transformMatrix_ = matrix;
    hasTransformMatrix_ = true;
}

void ImagePrimitive::Impl::setFit(ImageFit fit) {
    fit_ = fit;
}

void ImagePrimitive::Impl::setCoverViewport(bool enabled, const Vec2& canvasSize, const Vec2& viewportOffset) {
    hasCoverViewport_ = enabled;
    coverViewportSize_ = canvasSize;
    coverViewportOffset_ = viewportOffset;
}

bool ImagePrimitive::Impl::updateTexture() {
    if (texture_ != 0 &&
        loadedGifPath_.empty() &&
        loadedSource_ == source_ &&
        loadedFlipVertically_ == flipVertically_) {
        pendingLoad_ = false;
        return false;
    }

    bool pending = false;
    const std::string resolvedPath = render::image::resolveImagePath(source_, &pending);
    pendingLoad_ = pending;
    if (!resolvedPath.empty() && render::image::isGifPath(resolvedPath)) {
        return updateGifTexture(resolvedPath);
    }

    if (!loadedGifPath_.empty()) {
        releaseOwnedTexture();
        loadedGifPath_.clear();
        gifFrames_.reset();
        gifDelays_.clear();
        gifFrameCount_ = 0;
        gifFrameIndex_ = 0;
        gifNextFrameTime_ = 0.0;
    }

    int nextWidth = 0;
    int nextHeight = 0;
    std::string nextCacheKey;
    const GLuint nextTexture = acquireTexture(source_, flipVertically_, &pending, &nextWidth, &nextHeight, &nextCacheKey);
    pendingLoad_ = pending;

    if (nextTexture == 0) {
        if (source_.empty()) {
            releaseOwnedTexture();
            texture_ = 0;
            textureWidth_ = 0;
            textureHeight_ = 0;
            loadedSource_.clear();
        }
        return false;
    }

    const bool changed = texture_ != nextTexture || loadedSource_ != source_ || loadedFlipVertically_ != flipVertically_;
    releaseOwnedTexture();
    texture_ = nextTexture;
    ownsTexture_ = false;
    loadedTextureCacheKey_ = std::move(nextCacheKey);
    textureWidth_ = nextWidth;
    textureHeight_ = nextHeight;
    loadedSource_ = source_;
    loadedFlipVertically_ = flipVertically_;
    pendingLoad_ = false;
    return changed;
}

bool ImagePrimitive::Impl::hasPendingLoad() const {
    return pendingLoad_;
}

bool ImagePrimitive::Impl::isAnimating() const {
    return gifFrameCount_ > 1;
}

void ImagePrimitive::Impl::render(int windowWidth, int windowHeight) {
    if (texture_ == 0 || shaderProgram_ == 0 || vao_ == 0 || vbo_ == 0 || bounds_.width <= 0.0f || bounds_.height <= 0.0f) {
        return;
    }

    float vertices[42] = {};
    rebuildVertices(vertices);

    const GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram_);
    glUniform2f(windowSizeLocation_, static_cast<float>(std::max(1, windowWidth)), static_cast<float>(std::max(1, windowHeight)));
    glUniform4f(tintLocation_, tint_.r, tint_.g, tint_.b, tint_.a * opacity_);
    glUniform4f(rectLocation_, bounds_.x, bounds_.y, bounds_.width, bounds_.height);
    glUniform1f(radiusLocation_, radius_);
    glUniform1i(textureLocation_, 0);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (!blendEnabled) {
        glDisable(GL_BLEND);
    }
}

bool ImagePrimitive::Impl::isSourceReady(const std::string& source) {
    return render::image::isSourceReady(source);
}

bool ImagePrimitive::Impl::consumeRemoteImageReady() {
    return render::image::consumeRemoteImageReady();
}

void ImagePrimitive::Impl::releaseCachedTextures() {
    for (auto& item : gTextureCache) {
        if (item.second.texture != 0) {
            glDeleteTextures(1, &item.second.texture);
        }
    }
    gTextureCache.clear();
}

ImagePrimitive::Impl::SharedResources& ImagePrimitive::Impl::sharedResources() {
    static std::unordered_map<window::ContextKey, SharedResources> resourcesByContext;
    return resourcesByContext[window::currentContextKey()];
}

bool ImagePrimitive::Impl::retainSharedResources() {
    SharedResources& resources = sharedResources();
    ++resources.references;
    if (resources.shaderProgram != 0) {
        return true;
    }

    const char* vertexSource =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aScreenPos;\n"
        "layout(location = 1) in vec2 aLocalPos;\n"
        "layout(location = 2) in vec2 aUV;\n"
        "uniform vec2 uWindowSize;\n"
        "out vec2 vLocalPos;\n"
        "out vec2 vUV;\n"
        "void main() {\n"
        "    vLocalPos = aLocalPos;\n"
        "    vUV = aUV;\n"
        "    vec2 ndc = vec2((aScreenPos.x / uWindowSize.x) * 2.0 - 1.0,\n"
        "                    1.0 - (aScreenPos.y / uWindowSize.y) * 2.0);\n"
        "    gl_Position = vec4(ndc * aScreenPos.z, 0.0, aScreenPos.z);\n"
        "}\n";

    const char* fragmentSource =
        "#version 330 core\n"
        "in vec2 vLocalPos;\n"
        "in vec2 vUV;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "uniform vec4 uTint;\n"
        "uniform vec4 uRect;\n"
        "uniform float uRadius;\n"
        "float roundedBoxDistance(vec2 point, vec2 halfSize, float radius) {\n"
        "    vec2 cornerVector = abs(point) - halfSize + vec2(radius);\n"
        "    return length(max(cornerVector, 0.0)) + min(max(cornerVector.x, cornerVector.y), 0.0) - radius;\n"
        "}\n"
        "void main() {\n"
        "    vec2 center = uRect.xy + uRect.zw * 0.5;\n"
        "    float distanceToEdge = roundedBoxDistance(vLocalPos - center, uRect.zw * 0.5, uRadius);\n"
        "    float edgeWidth = max(fwidth(distanceToEdge), 0.75);\n"
        "    float shapeAlpha = 1.0 - smoothstep(-edgeWidth, edgeWidth, distanceToEdge);\n"
        "    if (shapeAlpha <= 0.0) discard;\n"
        "    vec4 sampled = texture(uTexture, vUV);\n"
        "    FragColor = vec4(sampled.rgb * uTint.rgb, sampled.a * uTint.a * shapeAlpha);\n"
        "}\n";

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        if (vertexShader != 0) {
            glDeleteShader(vertexShader);
        }
        if (fragmentShader != 0) {
            glDeleteShader(fragmentShader);
        }
        resources.references = std::max(0, resources.references - 1);
        return false;
    }

    resources.shaderProgram = glCreateProgram();
    glAttachShader(resources.shaderProgram, vertexShader);
    glAttachShader(resources.shaderProgram, fragmentShader);
    glLinkProgram(resources.shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linked = 0;
    glGetProgramiv(resources.shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteProgram(resources.shaderProgram);
        resources.shaderProgram = 0;
        resources.references = std::max(0, resources.references - 1);
        return false;
    }

    resources.windowSizeLocation = glGetUniformLocation(resources.shaderProgram, "uWindowSize");
    resources.textureLocation = glGetUniformLocation(resources.shaderProgram, "uTexture");
    resources.tintLocation = glGetUniformLocation(resources.shaderProgram, "uTint");
    resources.rectLocation = glGetUniformLocation(resources.shaderProgram, "uRect");
    resources.radiusLocation = glGetUniformLocation(resources.shaderProgram, "uRadius");

    glGenVertexArrays(1, &resources.vao);
    glGenBuffers(1, &resources.vbo);
    glBindVertexArray(resources.vao);
    glBindBuffer(GL_ARRAY_BUFFER, resources.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 42, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, reinterpret_cast<void*>(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, reinterpret_cast<void*>(sizeof(float) * 5));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return resources.shaderProgram != 0 && resources.vao != 0 && resources.vbo != 0;
}

void ImagePrimitive::Impl::releaseSharedResources() {
    SharedResources& resources = sharedResources();
    resources.references = std::max(0, resources.references - 1);
    if (resources.references > 0) {
        return;
    }

    if (resources.vbo != 0) {
        glDeleteBuffers(1, &resources.vbo);
        resources.vbo = 0;
    }
    if (resources.vao != 0) {
        glDeleteVertexArrays(1, &resources.vao);
        resources.vao = 0;
    }
    if (resources.shaderProgram != 0) {
        glDeleteProgram(resources.shaderProgram);
        resources.shaderProgram = 0;
    }
    resources.windowSizeLocation = -1;
    resources.textureLocation = -1;
    resources.tintLocation = -1;
    resources.rectLocation = -1;
    resources.radiusLocation = -1;
}

unsigned int ImagePrimitive::Impl::compileShader(unsigned int type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ImagePrimitive::Impl::updateGifTexture(const std::string& resolvedPath) {
    if (resolvedPath.empty()) {
        return false;
    }

    if (loadedGifPath_ != resolvedPath || loadedGifFlipVertically_ != flipVertically_) {
        std::shared_ptr<const render::image::GifFrameData> frameData =
            render::image::loadGifFrames(resolvedPath, flipVertically_);
        if (!frameData) {
            return false;
        }

        releaseOwnedTexture();
        texture_ = createTexture(frameData->pixels.data(), frameData->width, frameData->height);
        if (texture_ == 0) {
            gifFrames_.reset();
            gifDelays_.clear();
            gifFrameCount_ = 0;
            return false;
        }

        ownsTexture_ = true;
        textureWidth_ = frameData->width;
        textureHeight_ = frameData->height;
        loadedSource_ = source_;
        loadedFlipVertically_ = flipVertically_;
        loadedGifPath_ = resolvedPath;
        loadedGifFlipVertically_ = flipVertically_;
        gifFrames_ = std::move(frameData);
        gifDelays_ = gifFrames_->delays;
        gifFrameCount_ = gifFrames_->frameCount;
        gifFrameIndex_ = 0;
        gifNextFrameTime_ = window::timeSeconds() + static_cast<double>(gifDelays_.front()) / 1000.0;
        pendingLoad_ = false;
        return true;
    }

    if (gifFrameCount_ <= 1 || texture_ == 0 || !gifFrames_ || gifFrames_->pixels.empty()) {
        return false;
    }

    const double now = window::timeSeconds();
    if (now < gifNextFrameTime_) {
        return false;
    }

    int guard = 0;
    do {
        gifFrameIndex_ = (gifFrameIndex_ + 1) % gifFrameCount_;
        const int delay = gifDelays_.empty() ? 100 : gifDelays_[static_cast<size_t>(gifFrameIndex_)];
        gifNextFrameTime_ += static_cast<double>(std::max(10, delay)) / 1000.0;
        ++guard;
    } while (now >= gifNextFrameTime_ && guard < gifFrameCount_);

    const size_t frameBytes = static_cast<size_t>(textureWidth_) * static_cast<size_t>(textureHeight_) * 4u;
    const unsigned char* frame = gifFrames_->pixels.data() + frameBytes * static_cast<size_t>(gifFrameIndex_);
    updateTexturePixels(texture_, frame, textureWidth_, textureHeight_);
    return true;
}

void ImagePrimitive::Impl::releaseOwnedTexture() {
    releaseCachedTextureReference();
    if (ownsTexture_ && texture_ != 0) {
        glDeleteTextures(1, &texture_);
        texture_ = 0;
    }
    ownsTexture_ = false;
}

void ImagePrimitive::Impl::releaseCachedTextureReference() {
    if (loadedTextureCacheKey_.empty()) {
        return;
    }
    releaseCachedTexture(loadedTextureCacheKey_);
    loadedTextureCacheKey_.clear();
    if (!ownsTexture_) {
        texture_ = 0;
    }
}

unsigned int ImagePrimitive::Impl::acquireTexture(const std::string& source,
                                            bool flipVertically,
                                            bool* pending,
                                            int* outWidth,
                                            int* outHeight,
                                            std::string* outCacheKey) {
    const std::string resolvedPath = render::image::resolveImagePath(source, pending);
    if (resolvedPath.empty()) {
        return 0;
    }

    const std::string cacheKey = render::image::imageCacheKey(resolvedPath, flipVertically);
    const auto cached = gTextureCache.find(cacheKey);
    if (cached != gTextureCache.end()) {
        ++cached->second.references;
        if (outWidth != nullptr) {
            *outWidth = cached->second.width;
        }
        if (outHeight != nullptr) {
            *outHeight = cached->second.height;
        }
        if (outCacheKey != nullptr) {
            *outCacheKey = cacheKey;
        }
        return cached->second.texture;
    }

    const std::shared_ptr<const render::image::StaticImageData> image =
        render::image::loadStaticImageFromPath(resolvedPath, flipVertically);
    if (!image || image->pixels.empty() || image->width <= 0 || image->height <= 0) {
        return 0;
    }

    const GLuint texture = createTexture(image->pixels.data(), image->width, image->height);
    if (texture != 0) {
        gTextureCache[cacheKey] = {texture, image->width, image->height, 1};
        if (outWidth != nullptr) {
            *outWidth = image->width;
        }
        if (outHeight != nullptr) {
            *outHeight = image->height;
        }
        if (outCacheKey != nullptr) {
            *outCacheKey = cacheKey;
        }
    }
    return texture;
}

void ImagePrimitive::Impl::releaseCachedTexture(const std::string& cacheKey) {
    const auto cached = gTextureCache.find(cacheKey);
    if (cached == gTextureCache.end()) {
        return;
    }

    cached->second.references = std::max(0, cached->second.references - 1);
    if (cached->second.references > 0) {
        return;
    }

    if (cached->second.texture != 0) {
        glDeleteTextures(1, &cached->second.texture);
    }
    gTextureCache.erase(cached);
}

Vec3 ImagePrimitive::Impl::transformPoint(float x, float y) const {
    if (hasTransformMatrix_) {
        return core::transformPointWithW(transformMatrix_, x, y);
    }

    const Vec2 origin = {
        bounds_.x + bounds_.width * transform_.origin.x,
        bounds_.y + bounds_.height * transform_.origin.y
    };

    const float scaledX = (x - origin.x) * transform_.scale.x;
    const float scaledY = (y - origin.y) * transform_.scale.y;
    const float cosine = std::cos(transform_.rotate);
    const float sine = std::sin(transform_.rotate);

    return {
        origin.x + scaledX * cosine - scaledY * sine + transform_.translate.x,
        origin.y + scaledX * sine + scaledY * cosine + transform_.translate.y,
        1.0f
    };
}

void ImagePrimitive::Impl::rebuildVertices(float* vertices) const {
    Rect drawRect = bounds_;
    if (fit_ == ImageFit::Contain && textureWidth_ > 0 && textureHeight_ > 0 && bounds_.width > 0.0f && bounds_.height > 0.0f) {
        const float rectAspect = bounds_.width / bounds_.height;
        const float imageAspect = static_cast<float>(textureWidth_) / static_cast<float>(textureHeight_);
        if (imageAspect > rectAspect) {
            drawRect.height = bounds_.width / imageAspect;
            drawRect.y = bounds_.y + (bounds_.height - drawRect.height) * 0.5f;
        } else if (imageAspect < rectAspect) {
            drawRect.width = bounds_.height * imageAspect;
            drawRect.x = bounds_.x + (bounds_.width - drawRect.width) * 0.5f;
        }
    }

    const Vec3 screen[4] = {
        transformPoint(drawRect.x, drawRect.y),
        transformPoint(drawRect.x + drawRect.width, drawRect.y),
        transformPoint(drawRect.x + drawRect.width, drawRect.y + drawRect.height),
        transformPoint(drawRect.x, drawRect.y + drawRect.height)
    };
    const Vec2 local[4] = {
        {drawRect.x, drawRect.y},
        {drawRect.x + drawRect.width, drawRect.y},
        {drawRect.x + drawRect.width, drawRect.y + drawRect.height},
        {drawRect.x, drawRect.y + drawRect.height}
    };
    float u0 = 0.0f;
    float v0 = 0.0f;
    float u1 = 1.0f;
    float v1 = 1.0f;
    if (fit_ != ImageFit::Stretch && textureWidth_ > 0 && textureHeight_ > 0 && bounds_.width > 0.0f && bounds_.height > 0.0f) {
        const bool useCoverViewport = fit_ == ImageFit::Cover &&
                                      hasCoverViewport_ &&
                                      coverViewportSize_.x > 0.0f &&
                                      coverViewportSize_.y > 0.0f;
        const float sampleWidth = useCoverViewport ? coverViewportSize_.x : bounds_.width;
        const float sampleHeight = useCoverViewport ? coverViewportSize_.y : bounds_.height;
        const float rectAspect = sampleWidth / sampleHeight;
        const float imageAspect = static_cast<float>(textureWidth_) / static_cast<float>(textureHeight_);
        if (fit_ == ImageFit::Cover) {
            if (imageAspect > rectAspect) {
                const float visible = std::clamp(rectAspect / imageAspect, 0.0f, 1.0f);
                u0 = (1.0f - visible) * 0.5f;
                u1 = 1.0f - u0;
            } else if (imageAspect < rectAspect) {
                const float visible = std::clamp(imageAspect / rectAspect, 0.0f, 1.0f);
                v0 = (1.0f - visible) * 0.5f;
                v1 = 1.0f - v0;
            }
            if (useCoverViewport) {
                const float left = std::clamp(coverViewportOffset_.x / sampleWidth, 0.0f, 1.0f);
                const float top = std::clamp(coverViewportOffset_.y / sampleHeight, 0.0f, 1.0f);
                const float right = std::clamp((coverViewportOffset_.x + bounds_.width) / sampleWidth, left, 1.0f);
                const float bottom = std::clamp((coverViewportOffset_.y + bounds_.height) / sampleHeight, top, 1.0f);
                const float fullU0 = u0;
                const float fullV0 = v0;
                const float fullU1 = u1;
                const float fullV1 = v1;
                u0 = fullU0 + (fullU1 - fullU0) * left;
                u1 = fullU0 + (fullU1 - fullU0) * right;
                v0 = fullV0 + (fullV1 - fullV0) * top;
                v1 = fullV0 + (fullV1 - fullV0) * bottom;
            }
        }
    }
    const Vec2 uv[4] = {
        {u0, v0},
        {u1, v0},
        {u1, v1},
        {u0, v1}
    };
    const int order[6] = {0, 1, 2, 0, 2, 3};

    for (int i = 0; i < 6; ++i) {
        const int index = order[i];
        const int offset = i * 7;
        vertices[offset + 0] = screen[index].x;
        vertices[offset + 1] = screen[index].y;
        vertices[offset + 2] = screen[index].z;
        vertices[offset + 3] = local[index].x;
        vertices[offset + 4] = local[index].y;
        vertices[offset + 5] = uv[index].x;
        vertices[offset + 6] = uv[index].y;
    }
}

ImagePrimitive::ImagePrimitive()
    : impl_(std::make_unique<Impl>()) {}

ImagePrimitive::~ImagePrimitive() = default;
ImagePrimitive::ImagePrimitive(ImagePrimitive&&) noexcept = default;
ImagePrimitive& ImagePrimitive::operator=(ImagePrimitive&&) noexcept = default;

bool ImagePrimitive::initialize() { return impl_->initialize(); }
void ImagePrimitive::destroy() { impl_->destroy(); }
void ImagePrimitive::setSource(const std::string& source) { impl_->setSource(source); }
void ImagePrimitive::setFlipVertically(bool value) { impl_->setFlipVertically(value); }
void ImagePrimitive::setBounds(float x, float y, float width, float height) { impl_->setBounds(x, y, width, height); }
void ImagePrimitive::setTint(const Color& tint) { impl_->setTint(tint); }
void ImagePrimitive::setCornerRadius(float radius) { impl_->setCornerRadius(radius); }
void ImagePrimitive::setOpacity(float opacity) { impl_->setOpacity(opacity); }
void ImagePrimitive::setTransform(const Transform& transform) { impl_->setTransform(transform); }
void ImagePrimitive::setTransformMatrix(const TransformMatrix& matrix) { impl_->setTransformMatrix(matrix); }
void ImagePrimitive::setFit(ImageFit fit) { impl_->setFit(fit); }
void ImagePrimitive::setCoverViewport(bool enabled, const Vec2& canvasSize, const Vec2& viewportOffset) {
    impl_->setCoverViewport(enabled, canvasSize, viewportOffset);
}
bool ImagePrimitive::updateTexture() { return impl_->updateTexture(); }
bool ImagePrimitive::hasPendingLoad() const { return impl_->hasPendingLoad(); }
bool ImagePrimitive::isAnimating() const { return impl_->isAnimating(); }
void ImagePrimitive::render(int windowWidth, int windowHeight) { impl_->render(windowWidth, windowHeight); }
bool ImagePrimitive::isSourceReady(const std::string& source) { return Impl::isSourceReady(source); }
bool ImagePrimitive::consumeRemoteImageReady() { return Impl::consumeRemoteImageReady(); }
void ImagePrimitive::releaseCachedTextures() { Impl::releaseCachedTextures(); }

} // namespace core
