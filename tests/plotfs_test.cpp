#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "plotfs.hpp"
#include "device.hpp"
#include "devicefactory_test.cpp"
#include "plotfactory_test.hpp"
#include "plot_test.hpp"
#include "device_test.hpp"
#include <sys/stat.h>

using namespace testing;

class PlotFSTest : public Test
{
protected:
    const std::string path = "plotfs.bin";

    
    const size_t SIZE_1GiB = 1024 * 1024 * 1024;
    const size_t SIZE_400_MiB = 4 * 1024 * 1024;
    const size_t SIZE_101_4GiB = 101 * SIZE_1GiB + SIZE_400_MiB;
    const size_t SIZE_500GiB = 500 * SIZE_1GiB;

    std::shared_ptr<MockDeviceFactory> mockDeviceFactory;
    std::shared_ptr<MockPlotFactory> mockPlotFactory;
    std::shared_ptr<PlotFS> plotfs;

    void SetUp() override
    {
        ASSERT_TRUE(PlotFS::init(path, true)) << "Failed to initialize.";
        mockDeviceFactory.reset(new MockDeviceFactory);
        mockPlotFactory.reset(new MockPlotFactory);
        plotfs.reset(new PlotFS(path, *mockDeviceFactory, *mockPlotFactory));
    }

    std::shared_ptr<MockDeviceHandle> mockDevice(uint8_t id, size_t size)
    {
        return std::make_shared<MockDeviceHandle>(0, size, id);
    }

    std::shared_ptr<MockDeviceHandle> mockDevice(uint8_t id)
    {
        return mockDevice(id, 128);
    }

    bool addDevice(std::string path, int id, bool force)
    {
        EXPECT_CALL(*mockDeviceFactory, open(path, false, O_RDONLY)).WillOnce(Return(std::shared_ptr<DeviceHandle>()));

        EXPECT_CALL(*mockDeviceFactory, format(path)).Times(1).WillOnce(testing::Return(mockDevice(id)));
        return plotfs->addDevice(path, false);
    }

    std::shared_ptr<MockDeviceHandle> initDevice(int id, size_t size, bool active)
    {
        std::string path = std::to_string(id);
        EXPECT_CALL(*mockDeviceFactory, open(path, false, O_RDONLY)).WillOnce(Return(std::shared_ptr<DeviceHandle>()));
        EXPECT_CALL(*mockDeviceFactory, format(path)).Times(1).WillOnce(testing::Return(mockDevice(id,size)));

        plotfs->addDevice(path, false);

        auto device = mockDevice(id, size);

        if(active) {
            EXPECT_CALL(*mockDeviceFactory, open(path, true, O_RDWR)).WillOnce(Return(device));
        }

        return device;
    }

    std::shared_ptr<MockDeviceHandle> initDevice(int id, bool active)
    {
        return initDevice(id, 128, active);
    }

    std::shared_ptr<MockDeviceHandle> initDevice(int id)
    {
        return initDevice(id, true);
    }

    std::shared_ptr<MockPlot> mockPlot(uint8_t id, off_t size)
    {
        auto plot = std::make_shared<MockPlot>(id);
        EXPECT_CALL(*mockPlotFactory, openPlot(std::to_string(id) + ".plot")).WillOnce(Return(plot));
        struct stat st;
        st.st_size = size;
        ON_CALL(*plot, stat()).WillByDefault(testing::Return(st));
        return plot;
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
    EXPECT_CALL(*mockDeviceFactory, open("device1", false, O_RDONLY)).Times(2).WillRepeatedly(Return(mockDevice(1)));    
    EXPECT_FALSE(plotfs->addDevice("device1", false)) << "Device with existing PolotFS should not be added.";

    EXPECT_CALL(*mockDeviceFactory, format("device1")).WillOnce(Return(mockDevice(1)));
    EXPECT_TRUE(plotfs->addDevice("device1", true)) << "Device with existing PlotFS should be added even with force.";
}

TEST_F(PlotFSTest, AddDevice_FailedFormat)
{
    EXPECT_CALL(*mockDeviceFactory, open("device1", false, O_RDONLY)).WillOnce(Return(std::shared_ptr<DeviceHandle>()));
    EXPECT_CALL(*mockDeviceFactory, format("device1")).WillOnce(Return(std::shared_ptr<DeviceHandle>()));
    EXPECT_FALSE(plotfs->addDevice("device1", false)) << "Return false if formatDevice fails.";
}

void expectWriteHeader(std::shared_ptr<MockDeviceHandle> device) {
    EXPECT_CALL(*device, write(A<const uint8_t*>(), 64)).WillOnce(Return(64));
}

void expectWritePlot(std::shared_ptr<MockDeviceHandle> device, std::shared_ptr<PlotFile> plot, size_t size, int times) {
    EXPECT_CALL(*device, write(std::dynamic_pointer_cast<FileHandle>(plot), _, size)) 
        .Times(times)
        .WillRepeatedly([size](std::shared_ptr<FileHandle> file, off_t* offset, size_t length) {
            *offset += size;
            return size;
         });
}

void expectWritePlot(std::shared_ptr<MockDeviceHandle> device, std::shared_ptr<PlotFile> plot, size_t size) {
    expectWritePlot(device, plot, size, 1);
}

TEST_F(PlotFSTest, AddPlot)
{
    auto plot = mockPlot(1, 16);
    auto device = initDevice(1);

    EXPECT_CALL(*device, seek(0)).WillOnce(Return(true));
    expectWriteHeader(device);
    expectWritePlot(device, plot, 16);
    
    EXPECT_TRUE(plotfs->addPlot("1.plot"));
}


TEST_F(PlotFSTest, AddPlot_Two_Devices)
{
    auto plot = mockPlot(1, 65);
    auto device1 = initDevice(1);
    auto device2 = initDevice(2); 

    EXPECT_CALL(*device1, seek(0)).WillOnce(Return(true));
    expectWriteHeader(device1);
    expectWritePlot(device1, plot, 64);

    EXPECT_CALL(*device2, seek(0)).WillOnce(Return(true));
    expectWriteHeader(device2);
    expectWritePlot(device2, plot, 1);

    EXPECT_TRUE(plotfs->addPlot("1.plot"));
}

TEST_F(PlotFSTest, AddPlot_Too_Big)
{
    auto plot = mockPlot(1, 65);
    auto device1 = initDevice(1, false);

    EXPECT_FALSE(plotfs->addPlot("1.plot"));
}

TEST_F(PlotFSTest, AddPlot_FULLSIZE)
{
    auto plot = mockPlot(1, SIZE_101_4GiB);
    auto device = initDevice(1, SIZE_500GiB, true);

    EXPECT_CALL(*device, seek(0)).WillOnce(Return(true));
    expectWriteHeader(device);
    expectWritePlot(device, plot, SIZE_1GiB, 101);
    expectWritePlot(device, plot, SIZE_400_MiB);
    
    EXPECT_TRUE(plotfs->addPlot("1.plot"));
}