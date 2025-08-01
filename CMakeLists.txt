cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

CREATE_LIB(datetime INTERFACE)

target_compile_features(datetime INTERFACE cxx_std_17)

target_include_directories(datetime INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/datetime>
)

INSTALL_LIB(datetime True datetime)