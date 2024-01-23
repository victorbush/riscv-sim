cmake_minimum_required(VERSION 3.14)

add_executable (riscv-sim
	"main.cpp"
	"memory.h"
	"rv32.cpp" "rv32.h"
	"rv32-hart.cpp" "rv32-hart.h"
	"simple-system.cpp" "simple-system.h"
	"../third-party/cpp-httplib/httplib.h"
)

target_include_directories(riscv-sim PRIVATE "../third-party/cpp-httplib" "../third-party")

set_property(TARGET riscv-sim PROPERTY CXX_STANDARD 23)
