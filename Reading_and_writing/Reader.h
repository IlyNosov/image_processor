#ifndef CPP_HSE_READER_H
#define CPP_HSE_READER_H

#include <string>

#include "../Image/Image.h"

namespace reading_and_writing {
class Reader {
private:
    std::string path_;
    size_t BytesToNum(unsigned char* bytes);

public:
    explicit Reader(const std::string& path);
    Image Read();
};
const size_t GetPaddedSize(size_t width);
}  // namespace reading_and_writing

#endif  // CPP_HSE_READER_H
