#include "imagereader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

// this function is copied from the projects 3/4 stencil, but we ported it from Qt to stb_image
/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * @param file: file path to an image
 * @return Pointer to the image if successfully loads image, nullptr otherwise.
 */
std::unique_ptr<Image> loadImageFromFile(const std::string& file) {
    int width, height;

    // 4 tells stb to force load the image with an alpha channel
    unsigned char* imgData = stbi_load(file.c_str(), &width, &height, nullptr, 4);

    if (!imgData) {
        std::cerr << "Failed to load image: " << file << std::endl;
        return nullptr;
    }

    std::unique_ptr<Image> myImage = std::make_unique<Image>();
    myImage->width = width;
    myImage->height = height;
    myImage->data.resize(width * height);

    for (int i = 0; i < width * height; ++i) {
        myImage->data[i] = {
                imgData[4 * i + 0],
                imgData[4 * i + 1],
                imgData[4 * i + 2],
                imgData[4 * i + 3]
        };
    }

    // we copied the image data out, so we can free the stb image data
    stbi_image_free(imgData);
    return myImage;
}
