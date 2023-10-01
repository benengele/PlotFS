#pragma once

#include "file.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <random>

class DeviceHandle : public FileHandle {
private:
    uint64_t begin_ = 0;
    uint64_t end_ = 0;
    std::vector<uint8_t> id_;

public:
    uint64_t begin() const { return begin_; }
    uint64_t end() const { return end_; }
    const std::vector<uint8_t>& id() const { return id_; }

    DeviceHandle(int fd, uint64_t begin, uint64_t end, const std::vector<uint8_t>& id)
        : FileHandle(fd)
        , begin_(begin)
        , end_(end)
        , id_(id.begin(), id.end()) {};
        
};