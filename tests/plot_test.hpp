#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "plot.hpp"
#include "file_test.hpp"

class MockPlot : public PlotFile  {
public:
    MockPlot(uint8_t id) : PlotFile(-1, 32, std::vector<uint8_t>{id}) {}
    
    MOCK_ALL_METHODS()
};