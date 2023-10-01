#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "plotfs.hpp"
#include "device.hpp"
#include "devicefactory_test.cpp"
#include "device_test.cpp"

using ::testing::StrictMock;

class PlotFSTest : public ::testing::Test
{
protected:
    const std::string path = "plotfs.bin";

    std::shared_ptr<MockDeviceFactory> mockFactory;
    std::shared_ptr<PlotFS> plotfs;

    void SetUp() override
    {
        ASSERT_TRUE(PlotFS::init(path, true)) << "Failed to initialize.";
        mockFactory.reset(new MockDeviceFactory);
        plotfs.reset(new PlotFS(path, *mockFactory));
    }

    std::shared_ptr<DeviceHandle> device(uint8_t id)
    {
        return std::dynamic_pointer_cast<DeviceHandle>(std::make_shared<MockDeviceHandle>(0, 100, id));
    }

    bool addDevice(std::string path, int id, bool force)
    {
        EXPECT_CALL(*mockFactory, open(path, false, O_RDONLY))
            .Times(1)
            .WillOnce(testing::Return(std::shared_ptr<DeviceHandle>()));

        EXPECT_CALL(*mockFactory, format(path))
            .Times(1)
            .WillOnce(testing::Return(device(id)));
        return plotfs->addDevice(path, false);
    }
};

TEST_F(PlotFSTest, AddDevice)
{
    EXPECT_TRUE(addDevice("device1", 1, false)) << "New device should be added.";
    EXPECT_TRUE(addDevice("device2", 2, false)) << "Second new device should be added.";
    EXPECT_TRUE(addDevice("device3", 3, true)) << "New device should be added with force.";
}


TEST_F(PlotFSTest, AddDevice_ExistingPath)
{
    addDevice("device1", 1, false);
    
    EXPECT_FALSE(plotfs->addDevice("device1", false)) << "Device with existing path should not be added.";
    EXPECT_FALSE(plotfs->addDevice("device1", true)) << "Device with existing path should not be added even with force.";
}

TEST_F(PlotFSTest, AddDevice_ExistingPlotFS)
{
    EXPECT_CALL(*mockFactory, open("device1", false, O_RDONLY))
        .Times(2)
        .WillRepeatedly(testing::Return(device(1)));    
    EXPECT_FALSE(plotfs->addDevice("device1", false)) << "Device with existing PolotFS should not be added.";

    EXPECT_CALL(*mockFactory, format("device1"))
        .Times(1)
        .WillOnce(testing::Return(device(1)));
    EXPECT_TRUE(plotfs->addDevice("device1", true)) << "Device with existing PlotFS should be added even with force.";
}

TEST_F(PlotFSTest, AddDevice_FailedFormat)
{
    EXPECT_CALL(*mockFactory, open("device1", false, O_RDONLY))
        .Times(1)
        .WillOnce(testing::Return(std::shared_ptr<DeviceHandle>()));
    EXPECT_CALL(*mockFactory, format("device1"))
        .Times(1)
        .WillOnce(testing::Return(std::shared_ptr<DeviceHandle>()));
    EXPECT_FALSE(plotfs->addDevice("device1", false)) << "Return false if format fails.";
}