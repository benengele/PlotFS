#include <gmock/gmock.h>
#include "plotfactory.hpp"

class MockPlotFactory : public testing::StrictMock<PlotFactory> {
public:
    MOCK_CONST_METHOD1(openPlot, std::shared_ptr<PlotFile>(const std::string& path));
};
