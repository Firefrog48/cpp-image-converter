#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

//using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader{
    char B = 'B';
    char M = 'M';
    uint32_t header_and_data_size = 0;
    uint32_t reserved = 0;
    uint32_t stride = 54;
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader{
    uint32_t info_size = 40;
    uint32_t width = 0;
    uint32_t height = 0;
    uint16_t number_of_planes = 1;
    uint16_t bit_in_pixel = 24;
    uint32_t compress_type = 0;
    uint32_t data_size = 0;
    uint32_t horizontal_resolution = 11811;
    uint32_t vertical_resolution = 11811;
    uint32_t number_of_colors = 0;
    uint32_t number_of_significant_colors = 0x1000000;
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image) {
    if (!image) {
        return false;
    }

    std::ofstream out(file, std::ios::binary);

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header; 
    
    info_header.width = image.GetWidth();
    info_header.height = image.GetHeight();

    int padding = GetBMPStride(image.GetWidth());

    file_header.header_and_data_size = image.GetHeight() * padding + file_header.stride;
    info_header.data_size = image.GetHeight() * padding;

    out.write((char*)&file_header, sizeof(file_header));
    out.write((char*)&info_header, sizeof(info_header));

    std::vector<char> buff(padding);

    for (int y = info_header.height - 1; y >= 0; --y) {
        const Color* line = image.GetLine(y);

        for (int x = 0; x < info_header.width; ++x) {
            buff[x * 3 + 0] = static_cast<char>(line[x].b);
            buff[x * 3 + 1] = static_cast<char>(line[x].g);
            buff[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        out.write(buff.data(), padding);
    }
    
    return out.good();
}


// напишите эту функцию
Image LoadBMP(const Path& file) {
    
    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;
     
    std::ifstream in(file, std::ios::binary);

    if (!in.is_open()) {
        return {};
    }

    in.read((char*)&file_header, sizeof(file_header));
    if (file_header.B != 'B' || file_header.M != 'M' || file_header.reserved != 0 || file_header.stride != 54) {
        return {};
    }

    in.read((char*)&info_header, sizeof(info_header));

    
    int padding = GetBMPStride(info_header.width);

    Image result(info_header.width, info_header.height, Color::Black());
    std::vector<char> buff(padding);

    for (int y = info_header.height - 1; y >= 0; --y) {
        Color* line = result.GetLine(y);
        in.read(buff.data(), padding);

        for (int x = 0; x < info_header.width; ++x) {
            line[x].r = static_cast<std::byte>(buff[x * 3 + 2]);
            line[x].g = static_cast<std::byte>(buff[x * 3 + 1]);
            line[x].b = static_cast<std::byte>(buff[x * 3 + 0]);
        }
    }

    return result;
}

}  // namespace img_lib
