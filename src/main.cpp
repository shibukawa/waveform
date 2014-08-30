#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sndfile.h>
#include <zlib.h>
#include <png.h>

struct Color {
    unsigned char R, G, B;
};

void help() {
    std::cout << "waveform: convert wave file into PNG image" << std::endl
              << "   Copyright (c) 2014 Yoshiki Shibukawa (DeNA Co.,Ltd)" << std::endl
              << std::endl
              << "usage> waveform [options] input.wav output.png" << std::endl
              << std::endl
              << "  [options]" << std::endl
              << "  -w, --width [num]     : output image width (default 256)" << std::endl
              << "  -h, --height [num]    : output image height (default 256)" << std::endl
              << "  -m, --margin [num]    : margin size (defult 16)" << std::endl
              << "  -fg, --fgcolor RRGGBB : foreground color (default 0000FFFF)" << std::endl
              << "  -bg, --bgcolor RRGGBB : background color (default FFFFFFFF)" << std::endl
              << std::endl
              << "  --help                : show this message" << std::endl
              << std::endl;
}

bool parse_size_t(int argc, const char** argv, int& i, const char* short_arg, const char* long_arg, size_t& variable) {
    if (i == argc - 1) {
        std::cout << short_arg << " or " << long_arg << " needs integer parameter." << std::endl;
        return false;
    }
    variable = std::atoi(argv[++i]);
    if (variable == 0) {
        std::cout << short_arg << " or " << long_arg << " needs integer parameter." << std::endl;
        return false;
    }
    return true;
}

bool parse_color(int argc, const char** argv, int& i, const char* short_arg, const char* long_arg, Color& variable) {
    if (i == argc - 1) {
        std::cout << short_arg << " or " << long_arg << " needs color parameter like 00ff00." << std::endl;
        return false;
    }
    char *end;
    unsigned int value = std::strtol(argv[++i], &end, 16);
    if (value == 0) {
        std::cout << short_arg << " or " << long_arg << " needs color parameter like 00ff00." << std::endl;
        return false;
    }
    variable.R = static_cast<unsigned char>((value & 0xff0000) >> 16);
    variable.G = static_cast<unsigned char>((value & 0xff00) >> 8);
    variable.B = static_cast<unsigned char>((value & 0xff));
    return true;
}

bool parse_arg(int argc, const char** argv, const char*& input, const char*& output, size_t& width, size_t & height, size_t& margin, Color& fgcolor, Color& bgcolor) {
    // default value
    width = 256;
    height = 256;
    margin = 16;
    fgcolor.R = 0;
    fgcolor.G = 0;
    fgcolor.B = 255;
    bgcolor.R = 255;
    bgcolor.G = 255;
    bgcolor.B = 255;

    for (int i = 1; i < argc; ++i) {
        std::string opt(argv[i]);
        if (opt == "--help") {
            return false;
        } else if (opt == "-w" or opt == "--width") {
            if (!parse_size_t(argc, argv, i, "-w", "--width", width)) {
                return false;
            }
        } else if (opt == "-h" or opt == "--height") {
            if (!parse_size_t(argc, argv, i, "-h", "--height", height)) {
                return false;
            }
        } else if (opt == "-m" or opt == "--margin") {
            if (!parse_size_t(argc, argv, i, "-m", "--margin", margin)) {
                return false;
            }
        } else if (opt == "-bg" or opt == "--bgcolor") {
            if (!parse_color(argc, argv, i, "-bg", "--bgcolor", bgcolor)) {
                return false;
            }
        } else if (opt == "-fg" or opt == "--fgcolor") {
            if (!parse_color(argc, argv, i, "-fg", "--fgcolor", fgcolor)) {
                return false;
            }
        } else {
            if (i == argc - 2) {
                input = argv[i];
                output = argv[i + 1];
                return true;
            }
            break;
        }
    }
    std::cout << "This program needs two file names: 'input.wav' and 'output.png', but no file names found" << std::endl;
    return false;
}

void draw_png(const char* output, size_t width, size_t height, unsigned char*& rawimage) {
    FILE *fp = std::fopen(output, "wb");
    if (!fp) {
        std::cout << "output file open error" << std::endl;
        return;
    }
    png_structp png = 0;
    png_infop info = 0;
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png)
    {
        std::cout << "internal error: 1" << std::endl;
        return;
    }
    info = png_create_info_struct(png);
    if (!info)
    {
        std::cout << "internal error: 2" << std::endl;
        return;
    }
    png_init_io(png, fp);
    png_set_compression_strategy(png, Z_DEFAULT_STRATEGY);
    png_set_compression_window_bits(png, 15);
    png_set_compression_level(png, Z_BEST_COMPRESSION);
    png_set_compression_mem_level(png, MAX_MEM_LEVEL);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png, info);
    unsigned char* row = new unsigned char[width * 3];
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            // antialias
            row[x * 3] = ((int)rawimage[(y * 2 * width * 2 + x * 2) * 3]
                        + (int)rawimage[(y * 2 * width * 2 + (x * 2 + 1)) * 3]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + x * 2) * 3]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + (x * 2 + 1)) * 3]) / 4;
            row[x * 3 + 1] = ((int)rawimage[(y * 2 * width * 2 + x * 2) * 3 + 1]
                        + (int)rawimage[(y * 2 * width * 2 + (x * 2 + 1)) * 3 + 1]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + x * 2) * 3 + 1]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + (x * 2 + 1)) * 3 + 1]) / 4;
            row[x * 3 + 2] = ((int)rawimage[(y * 2 * width * 2 + x * 2) * 3 + 2]
                        + (int)rawimage[(y * 2 * width * 2 + (x * 2 + 1)) * 3 + 2]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + x * 2) * 3 + 2]
                        + (int)rawimage[((y * 2 + 1) * width * 2 + (x * 2 + 1)) * 3 + 2]) / 4;
        }
        png_write_row(png, row);
        // png_write_row(png, rawimage + width * 2 * y * 3);
    }
    png_write_end(png, info);
    delete row;
}

int main(int argc, const char **argv) {

    FILE *fp;
    SNDFILE *sfd;
    SF_INFO *sfinfo = new SF_INFO;

    // params
    const char* input;
    const char* output;
    size_t width;
    size_t height;
    size_t border;
    Color fgcolor, bgcolor;

    if (!parse_arg(argc, argv, input, output, width, height, border, fgcolor, bgcolor)) {
        help();
        return 0;
    }

    sfinfo->frames = 0;
    sfinfo->samplerate = 0;
    sfinfo->channels = 0;
    sfinfo->format = 0;
    sfinfo->sections = 0;
    sfinfo->seekable = 0;

    if ((sfd = sf_open(argv[1], SFM_READ, sfinfo)) == 0) {
        std::cout << "Error opening soundfile" << std::cout;
        return 1;
    }

    // create first image in x2 size.
    unsigned char* rawimage = new unsigned char[width * height * 4 * 3];
    // clear image with background color
    for (size_t y = 0; y < height * 2; y++) {
        for (size_t x = 0; x < width * 2; x++) {
            size_t offset = (y * width * 2 + x) * 3;
            rawimage[offset] = bgcolor.R;
            rawimage[offset + 1] = bgcolor.G;
            rawimage[offset + 2] = bgcolor.B;
        }
    }
    double canvas_height = (height - border * 2) * 2;
    double canvas_width = (width - border * 2) * 2;
    int channels = sfinfo->channels;
    int frames = sfinfo->frames;
    // create original image 
    sf_count_t count = 1;
    short *val = new short[(int)count * sfinfo->channels];
    size_t range_start = 0;
    for (size_t x = 0; x < canvas_width; x++) {
        // search min value and max value in the column in canvas
        size_t range_end = std::floor((double)frames / canvas_width * (x + 1));
        short max_value = -32768;
        short min_value = 32767;
        for (size_t f = range_start; f < range_end; f++) {
            sf_count_t readnum = sf_readf_short(sfd, val, count);
            if (readnum != count) {
                break;
            }
            for (size_t i = 0; i < channels; i++) {
                min_value = (min_value > val[i]) ? val[i] : min_value;
                max_value = (max_value < val[i]) ? val[i] : max_value;
            }
        }

        range_start = range_end + 1;

        // draw line
        size_t bar_top = std::floor((double)(32767 + min_value) / 65536.0 * canvas_height) + border * 2;
        size_t bar_bottom = std::ceil((double)(32767 + max_value) / 65536.0 * canvas_height) + border * 2;
        for (size_t y = bar_top; y < bar_bottom; y++) {
            size_t offset = (y * width * 2 + (x + border * 2)) * 3;
            rawimage[offset] = fgcolor.R;
            rawimage[offset + 1] = fgcolor.G;
            rawimage[offset + 2] = fgcolor.B;
        }
    }
    sf_close(sfd);
    delete val;
    
    // anti alias and write to png
    draw_png(output, width, height, rawimage);
    delete rawimage;
    return 0;
}

