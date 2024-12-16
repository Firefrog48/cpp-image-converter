#include <img_lib.h>
#include <bmp_image.h>
#include <jpeg_image.h>
#include <ppm_image.h>


#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

namespace ImageFormat {
    enum class Format {
        JPEG,
        PPM,
        BMP,
        UNKNOWN
    };



    class ImageFormatInterface {
    public:
        virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
        virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
    };

    class JPEG : public ImageFormatInterface {
    public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SaveJPEG(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadJPEG(file);
        }
    };

    class PPM : public ImageFormatInterface {
    public:
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SavePPM(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadPPM(file);
        }
    };

    class BMP : public ImageFormatInterface {
        bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
            return img_lib::SaveBMP(file, image);
        }
        img_lib::Image LoadImage(const img_lib::Path& file) const override {
            return img_lib::LoadBMP(file);
        }
    };

    Format GetFormatByExtension(const img_lib::Path& input_file) {
        const string ext = input_file.extension().string();
        if (ext == ".jpg"sv || ext == ".jpeg"sv) {
            return Format::JPEG;
        }
        else if (ext == ".ppm"sv) {
            return Format::PPM;
        }
        else if (ext == ".bmp"sv) {
            return Format::BMP;
        }

        return Format::UNKNOWN;
    }

    static const ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {
        Format format = GetFormatByExtension(path);
        if (format == Format::JPEG) {
            static const ImageFormat::JPEG jpg;
            return &jpg;
        }
        else if (format == Format::PPM) {
            static const ImageFormat::PPM ppm;
            return &ppm;
        }
        else if (format == Format::BMP) {
            static const ImageFormat::BMP bmp;
            return &bmp;
        }
        else {
            return nullptr;
        }
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    auto input_interface = ImageFormat::GetFormatInterface(in_path);
    auto output_interface = ImageFormat::GetFormatInterface(out_path);


    if (input_interface == nullptr) {
        cerr << "Unknown format of the input file"sv << endl;
        return 2;
    }


    if (output_interface == nullptr) {
        cerr << "Unknown format of the output file"sv << endl;
        return 3;
    }

    img_lib::Image image = input_interface->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }


    if (!output_interface->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }


    cout << "Successfully converted"sv << endl;
}
