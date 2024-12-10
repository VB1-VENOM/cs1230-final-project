#pragma once

#include "utils/rgba.h"
#include <iostream>
#include <memory>
#include <vector>

struct Image {
    std::vector<RGBA> data;
    int width;
    int height;
};

std::unique_ptr<Image> loadImageFromFile(const std::string& file);
