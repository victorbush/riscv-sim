# CMakeList.txt : CMake project for riscv-sim, include source and define
# project specific logic here.
#

cmake_minimum_required(VERSION 3.14)

# Add source to this project's executable.
add_executable (riscv-sim
	"memory.cpp" "memory.h"
	"riscv-sim.cpp" "riscv-sim.h"
	"rv32.cpp" "rv32.h"
	"rv32-hart.cpp" "rv32-hart.h"
	"simple-system.cpp" "simple-system.h"
	"../third-party/cpp-httplib/httplib.h"
)

target_include_directories(riscv-sim PRIVATE "../third-party/cpp-httplib")

set_property(TARGET riscv-sim PROPERTY CXX_STANDARD 23)

# TODO: Add tests and install targets if needed.
