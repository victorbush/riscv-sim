cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(riscv-sim-tests
	"rv32-tests.cpp"
	"rv32-hart-tests.cpp"
	"../riscv-sim/rv32.cpp"
	"../riscv-sim/rv32-hart.cpp"
	"../riscv-sim/simple-system.cpp"
	"simple-system-tests.cpp"
	"test-utils.h"
)

target_include_directories(riscv-sim-tests PRIVATE "../riscv-sim")

target_link_libraries(
  riscv-sim-tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(riscv-sim-tests)

set_property(TARGET riscv-sim-tests PROPERTY CXX_STANDARD 23)
