#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "ppmio.h"

void applyGrayscale(std::vector<std::vector<RGB>>& image);
void applyInversion(std::vector<std::vector<RGB>>& image);
void applyContrastAdjustment(std::vector<std::vector<RGB>>& image);
void applyBlur(std::vector<std::vector<RGB>>& image);
void applyMirroring(std::vector<std::vector<RGB>>& image);
void applyCompression(std::vector<std::vector<RGB>>& image);

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "ppmio.h"

void applyGrayscale(std::vector<std::vector<RGB>>& image);
void applyInversion(std::vector<std::vector<RGB>>& image);
void applyContrastAdjustment(std::vector<std::vector<RGB>>& image);
void applyBlur(std::vector<std::vector<RGB>>& image);
void applyMirroring(std::vector<std::vector<RGB>>& image);
void applyCompression(std::vector<std::vector<RGB>>& image);

int main(int argc, char* argv[]) {
    std::set<std::string> valid_arguments = {"-g", "-i", "-x", "-b", "-m", "-c"};
    std::string inputFile, outputFile;
    std::vector<std::string> options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            if (arg.length() != 2) {
                std::cerr << "Ignoring invalid argument: " << arg << std::endl;
                continue;
            }
            if (valid_arguments.find(arg) == valid_arguments.end()) {
                std::cerr << "Ignoring unknown flag: " << arg << std::endl;
                continue;
            }
            options.push_back(arg);
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        } else {
            std::cerr << "Too many non-flag arguments provided." << std::endl;
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "No input file specified." << std::endl;
        return 1;
    }

    if (outputFile.empty()) {
        size_t dot_pos = inputFile.find_last_of('.');
        if (dot_pos != std::string::npos) {
            outputFile = inputFile.substr(0, dot_pos) + "_output" + inputFile.substr(dot_pos);
        } else {
            outputFile = inputFile + "_output";
        }
    }

    try {
        std::vector<std::vector<RGB>> image = readPPM(inputFile);

        for (const auto& option : options) {
            if (option == "-g") applyGrayscale(image);
            else if (option == "-i") applyInversion(image);
            else if (option == "-x") applyContrastAdjustment(image);
            else if (option == "-b") applyBlur(image);
            else if (option == "-m") applyMirroring(image);
            else if (option == "-c") {
                if (image.size() > 1 && image[0].size() > 1) {
                    applyCompression(image);
                } else {
                    std::cerr << "Image too small to compress further." << std::endl;
                }
            }
        }

        writePPM(outputFile, image);
        std::cout << "Image processing complete. Output saved to " << outputFile << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


// converts image to grayscale
void applyGrayscale(std::vector<std::vector<RGB>>& image) {
    for (auto& row : image) {
        for (auto& pixel : row) {
            unsigned char gray = (pixel.r + pixel.g + pixel.b) / 3;
            pixel.r = pixel.g = pixel.b = gray;
        }
    }
}

// inverts the colors of the image
void applyInversion(std::vector<std::vector<RGB>>& image) {
    for (auto& row : image) {
        for (auto& pixel : row) {
            pixel.r = 255 - pixel.r;
            pixel.g = 255 - pixel.g;
            pixel.b = 255 - pixel.b;
        }
    }
}

// adjusts contrast of the image
void applyContrastAdjustment(std::vector<std::vector<RGB>>& image) {
    const float contrastFactor = 1.2f;
    for (auto& row : image) {
        for (auto& pixel : row) {
            auto adjust = [contrastFactor](unsigned char& color) {
                int newColor = (color - 128) * contrastFactor + 128;
                color = std::max(0, std::min(255, newColor));
            };
            adjust(pixel.r);
            adjust(pixel.g);
            adjust(pixel.b);
        }
    }
}

// blurs the image
void applyBlur(std::vector<std::vector<RGB>>& image) {
    std::vector<std::vector<RGB>> blurred = image;
    for (size_t y = 1; y < image.size() - 1; ++y) {  
        for (size_t x = 1; x < image[y].size() - 1; ++x) {  
            int sumR = 0, sumG = 0, sumB = 0;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sumR += image[y + dy][x + dx].r;
                    sumG += image[y + dy][x + dx].g;
                    sumB += image[y + dy][x + dx].b;
                }
            }
            blurred[y][x].r = sumR / 9;
            blurred[y][x].g = sumG / 9;
            blurred[y][x].b = sumB / 9;
        }
    }
    image = blurred;
}

// mirrors the image
void applyMirroring(std::vector<std::vector<RGB>>& image) {
    for (auto& row : image) {
        std::reverse(row.begin(), row.end());
    }
}

// compresses the image
void applyCompression(std::vector<std::vector<RGB>>& image) {
    std::vector<std::vector<RGB>> compressed;
    for (size_t y = 0; y < image.size(); y += 2) {
        std::vector<RGB> row;
        for (size_t x = 0; x < image[y].size(); x += 2) {
            row.push_back(image[y][x]);
        }
        compressed.push_back(row);
    }
    image = compressed;
}