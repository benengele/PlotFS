#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "device.hpp"
#include "devicefactory.hpp"

class MockDeviceFactory : public DeviceFactory {

public:
    MOCK_CONST_METHOD1(format, std::shared_ptr<DeviceHandle>(const std::string& path));
    MOCK_CONST_METHOD2(format, std::shared_ptr<DeviceHandle>(const std::string& path, const std::vector<uint8_t>& dev_id));
    MOCK_CONST_METHOD3(open, std::shared_ptr<DeviceHandle>(const std::string& path, bool require_signature, int mode));
};