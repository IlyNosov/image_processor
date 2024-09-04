#ifndef CPP_HSE_FILTER_H
#define CPP_HSE_FILTER_H

#include <memory>
#include "../Image/Image.h"
#include "../Parsing/parse.h"

const double RED_COEF = 0.299;
const double GREEN_COEF = 0.587;
const double BLUE_COEF = 0.114;

namespace filters {
class Filter {
public:
    Filter() = default;
    virtual Image Apply(const Image& img) const = 0;
    virtual ~Filter() = default;

protected:
    template <typename T>
    std::vector<T> GetPixColor(const std::vector<std::vector<T>>& matrix, const Image& image, size_t x, size_t y) const;
};
class NegativeFilter : public Filter {
public:
    Image Apply(const Image& img) const override;
};
class CropFilter : public Filter {
public:
    CropFilter(size_t width, size_t height);
    Image Apply(const Image& img) const override;

private:
    size_t width_;
    size_t height_;
};
class EdgeDetectionFilter : public Filter {
public:
    explicit EdgeDetectionFilter(double threshold);
    Image Apply(const Image& img) const override;

private:
    double threshold_;
};
class SharpeningFilter : public Filter {
public:
    Image Apply(const Image& img) const override;
};
class GrayScaleFilter : public Filter {
public:
    Image Apply(const Image& img) const override;

private:
    double r_coef_ = RED_COEF;
    double g_coef_ = GREEN_COEF;
    double b_coef_ = BLUE_COEF;
};
class GaussianBlurFilter : public Filter {
public:
    explicit GaussianBlurFilter(float sigma);
    Image Apply(const Image& img) const override;

private:
    float sigma_;
};
std::unique_ptr<Filter> CreateFilter(const parser::Token& token);
}  // namespace filters
#endif  // CPP_HSE_FILTER_H
