include(FetchContent)

FetchContent_Declare(
    googleTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googleTest)

cmake_minimum_required(VERSION 3.10)

project(BMS_Pack_Controller_UnitTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(unit_tests
                unit/test_main.cpp
)

target_include_directories(unit_tests PRIVATE
        ../PCBCells_driver/PCB_Cells/Inc
        ../PCBCells_driver/ADC/Inc
        ../PCBCells_driver/CAN/Inc
        mocks/Inc
        ../EKO_Drivers/ADC/Inc
        ../EKO_Drivers/CAN/Inc
        ../Core/Inc

)


target_sources(unit_tests PRIVATE
        unit/test_main.cpp
        mocks/Src/stm32f3xx_hal_gpio_mock.c
        mocks/Src/stm32f3xx_hal_adc_mock.c
        mocks/Src/stm32f3xx_hal_can_mock.c
        mocks/Src/stm32f3xx_hal_dma_mock.c
        mocks/Src/stm32f3xx_hal_rcc_mock.c
)


target_link_libraries(unit_tests gtest_main)