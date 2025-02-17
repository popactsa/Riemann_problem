cmake_minimum_required(VERSION 2.15)
project(main)
file(GLOB SRC
	"source/*.cpp"
	)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES 
	source/Lagrange_1D.cpp
	source/Parameters.cpp
	#	source/elasticity_Lagrange_1D.cpp
	#	source/elasticity_Lagrange_1D_Parameters.cpp
	source/io_auxiliary.cpp
	source/main.cpp
	)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -std=c++20 -Wno-narrowing -Wno-return-type")
include_directories("headers/")
add_executable(${PROJECT_NAME} ${SRC})
