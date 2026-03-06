#include <gtest/gtest.h>
#include "pcb_cells.h"
#include "stm32f3xx_hal_def_mock.h"

TEST(SampleTest, SampleAssertion) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}