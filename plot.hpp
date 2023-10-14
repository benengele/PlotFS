#pragma once

#include <iomanip>
#include <sstream>
#include <vector>
#include "file.hpp"

class PlotFile : public FileHandle {
private:
    uint8_t k_;
    std::vector<uint8_t> id_;

public:
    PlotFile(int fd, uint8_t k, const std::vector<uint8_t> id)
        : FileHandle(fd)
        , k_(k)
        , id_(id) {};

    const uint8_t k() const { return k_; }
    const std::vector<uint8_t>& id() const { return id_; }
};