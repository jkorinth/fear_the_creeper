cmake_minimum_required(VERSION 3.16)
project(sim C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS_DEBBUG " -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBBUG " -g3 -DDEBUG")

find_library(ELF_LIBRARY elf)
if(NOT ELF_LIBRARY)
	message(FATAL_ERROR "libelf not found!")
endif()

file(GLOB SIMAVR_SRCS
	"simavr/simavr/sim/sim_*.c"
	"simavr/simavr/sim/avr_*.c"
	"simavr/simavr/cores/*.c"
	"simavr/simavr/cores/avr/*.c"
)

add_library(simavr_static STATIC ${SIMAVR_SRCS})
target_include_directories(simavr_static PUBLIC
	simavr
	simavr/simavr
	simavr/simavr/cores
	simavr/simavr/sim
)

target_compile_options(simavr_static INTERFACE
	"-fPIC"
	"-Wall"
	"-O2"
	"-g"
)
target_compile_definitions(simavr_static PRIVATE
	-DHAVE_LIBELF=1
	-DELF_SYMBOLS=1
	-DCONFIG_SIMAVR_TRACE=1
)
target_link_libraries(simavr_static INTERFACE elf)

add_executable(sim src/main.cpp src/leds.cpp)
target_include_directories(sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_compile_options(sim PUBLIC
	"-Wall"
	"-O2"
	"-g"
)

target_link_libraries(sim PUBLIC simavr_static)
