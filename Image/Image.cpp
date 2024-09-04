#include "Image.h"

Image::Image(size_t width, size_t height) {
    CheckHeightAndWidth(width, height);
    width_ = width;
    height_ = height;
    pixels_.resize(height);
    for (size_t i = 0; i < height; ++i) {
        pixels_[i].resize(width);
    }
}

Image::Image(const std::vector<std::vector<Color>>& pixels) {
    if (pixels.empty()) {
        throw std::invalid_argument("Empty pixels");
    }
    width_ = pixels[0].size();
    height_ = pixels.size();
    for (size_t i = 0; i < height_; ++i) {
        if (pixels[i].size() != width_) {
            throw std::invalid_argument("Different row sizes");
        }
    }
    pixels_ = pixels;
}

Image::Image(const Image& other) {
    width_ = other.width_;
    height_ = other.height_;
    pixels_ = other.pixels_;
}

size_t Image::GetHeight() const {
    return height_;
}

size_t Image::GetWidth() const {
    return width_;
}

const std::vector<std::vector<Color>>& Image::GetData() {
    return pixels_;
}

const Color& Image::GetColor(size_t x, size_t y) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Out of range");
    }
    return pixels_[y][x];
}

Color& Image::GetColor(size_t x, size_t y) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Out of range");
    }
    return pixels_[y][x];
}

void Image::SetColor(size_t x, size_t y, const Color& color) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Out of range");
    }
    pixels_[y][x] = color;
}

void Image::CheckHeightAndWidth(size_t width, size_t height) const {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("Zero size");
    }
}
