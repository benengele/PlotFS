#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "device.hpp"
#include "devicefactory.hpp"

class MockDeviceFactory : public testing::StrictMock<DeviceFactory> {
public:
    MOCK_METHOD(std::shared_ptr<DeviceHandle>, format, (const std::string& path), (const override));
    MOCK_METHOD(std::shared_ptr<DeviceHandle>, format, (const std::string& path, const std::vector<uint8_t>& dev_id), (const override));
    MOCK_METHOD(std::shared_ptr<DeviceHandle>, open, (const std::string& path, bool require_signature, int mode), (const override));
};
