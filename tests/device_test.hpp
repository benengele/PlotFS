#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "device.hpp"
#include "file_test.hpp"

class MockDeviceHandle : public DeviceHandle {
public:
    MockDeviceHandle(uint64_t begin, uint64_t end, uint8_t id) : DeviceHandle(-1, begin, end, std::vector<uint8_t>{id}) {}

    MOCK_ALL_METHODS()
};