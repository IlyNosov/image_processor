#include <algorithm>
#include <cmath>
#include "filter.h"

const int MAGIC_FIVE = 5;
const int MAX_COLOR = 255;

namespace filters {
template <typename T>
std::vector<T> Filter::GetPixColor(const std::vector<std::vector<T>>& matrix, const Image& image, size_t row,
                                   size_t column) const {
    T blue = 0;
    T green = 0;
    T red = 0;
    size_t dx = matrix.size() / 2;
    size_t dy = matrix[0].size() / 2;
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[0].size(); ++j) {
            red += image
                       .GetColor(std::min(column + j >= dy ? column + j - dy : 0, image.GetWidth() - 1),
                                 std::min(row + i >= dx ? row + i - dx : 0, image.GetHeight() - 1))
                       .red *
                   matrix[i][j];

            blue += image
                        .GetColor(
                            std::min(column + j >= dy ? column + j - dy : static_cast<size_t>(0), image.GetWidth() - 1),
                            std::min(row + i >= dx ? row + i - dx : static_cast<size_t>(0), image.GetHeight() - 1))
                        .blue *
                    matrix[i][j];

            green +=
                image
                    .GetColor(
                        std::min(column + j >= dy ? column + j - dy : static_cast<size_t>(0), image.GetWidth() - 1),
                        std::min(row + i >= dx ? row + i - dx : static_cast<size_t>(0), image.GetHeight() - 1))
                    .green *
                matrix[i][j];
        }
    }
    return {blue, green, red};
}

uint8_t GetCorrectColors(int color, const uint8_t max_color = 255) {
    if (color < 0) {
        return 0;
    } else if (color > max_color) {
        return max_color;
    }
    return color;
}

Image NegativeFilter::Apply(const Image& img) const {
    std::vector<std::vector<Color>> new_data;
    for (size_t i = 0; i < img.GetHeight(); ++i) {
        std::vector<Color> row(img.GetWidth());
        for (size_t j = 0; j < img.GetWidth(); ++j) {
            row[j].blue = static_cast<uint8_t>(MAX_COLOR - img.GetColor(j, i).blue);
            row[j].green = static_cast<uint8_t>(MAX_COLOR - img.GetColor(j, i).green);
            row[j].red = static_cast<uint8_t>(MAX_COLOR - img.GetColor(j, i).red);
        }
        new_data.push_back(row);
    }
    return Image{new_data};
}

CropFilter::CropFilter(size_t width, size_t height) : width_(width), height_(height) {
}

Image CropFilter::Apply(const Image& img) const {
    size_t new_height = std::min(height_, img.GetHeight());
    size_t new_width = std::min(width_, img.GetWidth());
    Image cropped_img(new_width, new_height);
    for (size_t i = img.GetHeight() - new_height; i < img.GetHeight(); ++i) {
        for (size_t j = 0; j < new_width; ++j) {
            cropped_img.SetColor(j, i + new_height - img.GetHeight(), img.GetColor(j, i));
        }
    }
    return cropped_img;
}

filters::EdgeDetectionFilter::EdgeDetectionFilter(double threshold) : threshold_(threshold) {
}

Image EdgeDetectionFilter::Apply(const Image& img) const {
    std::vector<std::vector<Color>> new_data;
    Image gs_img = GrayScaleFilter().Apply(img);
    std::vector<std::vector<int>> matrix = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
    for (size_t i = 0; i < img.GetHeight(); ++i) {
        std::vector<Color> row(img.GetWidth());
        for (size_t j = 0; j < img.GetWidth(); ++j) {
            std::vector<int> colors = GetPixColor(matrix, gs_img, i, j);
            uint8_t color = GetCorrectColors(colors[0]);
            if (color >= threshold_ * MAX_COLOR) {
                row[j].blue = MAX_COLOR;
                row[j].green = MAX_COLOR;
                row[j].red = MAX_COLOR;
            } else {
                row[j].blue = 0;
                row[j].green = 0;
                row[j].red = 0;
            }
        }
        new_data.push_back(row);
    }
    return Image{new_data};
}

Image SharpeningFilter::Apply(const Image& img) const {
    std::vector<std::vector<Color>> new_data;
    std::vector<std::vector<int>> matrix = {{0, -1, 0}, {-1, MAGIC_FIVE, -1}, {0, -1, 0}};
    for (size_t i = 0; i < img.GetHeight(); ++i) {
        std::vector<Color> row(img.GetWidth());
        for (size_t j = 0; j < img.GetWidth(); ++j) {
            std::vector<int> colors = GetPixColor(matrix, img, i, j);
            row[j].blue = GetCorrectColors(colors[0]);
            row[j].green = GetCorrectColors(colors[1]);
            row[j].red = GetCorrectColors(colors[2]);
        }
        new_data.push_back(row);
    }
    return Image{new_data};
}

Image GrayScaleFilter::Apply(const Image& img) const {
    std::vector<std::vector<Color>> new_data;
    for (size_t i = 0; i < img.GetHeight(); ++i) {
        std::vector<Color> row(img.GetWidth());
        for (size_t j = 0; j < img.GetWidth(); ++j) {
            double color = img.GetColor(j, i).blue * b_coef_ + img.GetColor(j, i).green * g_coef_ +
                           img.GetColor(j, i).red * r_coef_;
            row[j].blue = static_cast<uint8_t>(color);
            row[j].green = static_cast<uint8_t>(color);
            row[j].red = static_cast<uint8_t>(color);
        }
        new_data.push_back(row);
    }
    return Image{new_data};
}

filters::GaussianBlurFilter::GaussianBlurFilter(float sigma) : sigma_(sigma) {
}

Image filters::GaussianBlurFilter::Apply(const Image& img) const {
    std::vector<std::vector<Color>> new_data_x(img.GetWidth(), std::vector<Color>(img.GetHeight()));
    std::vector<std::vector<Color>> new_data_y(img.GetHeight(), std::vector<Color>(img.GetWidth()));
    std::vector<float> kernel;
    float sum = 0.0f;
    const int radius = static_cast<int>(sigma_ * 3);
    for (int i = -radius; i <= radius; ++i) {
        kernel.push_back(std::pow(static_cast<float>(M_E), -static_cast<float>(i * i) / (2 * sigma_ * sigma_)) /
                         std::sqrt(static_cast<float>(M_PI) * sigma_ * sigma_ * 2));
        sum += kernel.back();
    }
    for (int i = 0; i <= 2 * radius; ++i) {
        kernel[i] /= sum;
    }
    for (int x = 0; x < img.GetHeight(); ++x) {
        for (int y = 0; y < img.GetWidth(); ++y) {
            float red = 0.0f;
            float green = 0.0f;
            float blue = 0.0f;
            for (int i = -radius; i <= radius; ++i) {
                int dy = std::clamp(i + y, 0, static_cast<int>(img.GetWidth() - 1));
                red += kernel[i + radius] * static_cast<float>(img.GetColor(x, dy).red);
                green += kernel[i + radius] * static_cast<float>(img.GetColor(x, dy).green);
                blue += kernel[i + radius] * static_cast<float>(img.GetColor(x, dy).blue);
            }
            new_data_x[x][y].red = GetCorrectColors(static_cast<int>(red));
            new_data_x[x][y].green = GetCorrectColors(static_cast<int>(green));
            new_data_x[x][y].blue = GetCorrectColors(static_cast<int>(blue));
        }
    }
    for (int y = 0; y < img.GetHeight(); ++y) {
        for (int x = 0; x < img.GetWidth(); ++x) {
            float red = 0.0f;
            float green = 0.0f;
            float blue = 0.0f;
            for (int i = -radius; i <= radius; ++i) {
                int dx = std::clamp(i + x, 0, static_cast<int>(img.GetHeight() - 1));
                red += kernel[i + radius] * static_cast<float>(new_data_x[dx][y].red);
                green += kernel[i + radius] * static_cast<float>(new_data_x[dx][y].green);
                blue += kernel[i + radius] * static_cast<float>(new_data_x[dx][y].blue);
            }
            new_data_y[y][x].red = GetCorrectColors(static_cast<int>(red));
            new_data_y[y][x].green = GetCorrectColors(static_cast<int>(green));
            new_data_y[y][x].blue = GetCorrectColors(static_cast<int>(blue));
        }
    }
    return Image{new_data_y};
}

std::unique_ptr<filters::Filter> CreateFilter(const parser::Token& token) {
    const ::std::string& name = token.name;
    if (name == "-sharp") {
        if (!token.params.empty()) {
            throw std::invalid_argument("Too many parameters for sharp filter");
        }
        auto ptr = std::make_unique<SharpeningFilter>();
        return ptr;
    } else if (name == "-gs") {
        if (!token.params.empty()) {
            throw std::invalid_argument("Too many parameters for gs filter");
        }
        auto ptr = std::make_unique<GrayScaleFilter>();
        return ptr;
    } else if (name == "-neg") {
        if (!token.params.empty()) {
            throw std::invalid_argument("Too many parameters for neg filter");
        }
        auto ptr = std::make_unique<NegativeFilter>();
        return ptr;
    } else if (name == "-crop") {
        if (token.params.size() != 2) {
            throw std::invalid_argument("Too many or too few parameters for crop filter");
        }
        try {
            size_t width = std::stoul(token.params[0]);
            size_t height = std::stoul(token.params[1]);
            auto ptr = std::make_unique<CropFilter>(width, height);
            return ptr;
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument("Invalid argument");
        }

    } else if (name == "-edge") {
        if (token.params.size() != 1) {
            throw std::invalid_argument("Not correct parameters for edge filter");
        }
        double threshold = std::stod(token.params[0]);
        auto ptr = std::make_unique<EdgeDetectionFilter>(threshold);
        return ptr;
    } else if (name == "-blur") {
        if (token.params.size() != 1) {
            throw std::invalid_argument("Not correct parameters for gauss filter");
        }
        float sigma = std::stof(token.params[0]);
        auto ptr = std::make_unique<GaussianBlurFilter>(sigma);
        return ptr;
    } else {
        throw std::invalid_argument("Not correct filter name");
    }
}
}  // namespace filters
