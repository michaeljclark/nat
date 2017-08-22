project(nat)
cmake_minimum_required(VERSION 3.1.0)

set(
	nat_SOURCES
	src/nat.cc
	src/int.cc
 )

include_directories(
	src
)

add_library(nat ${nat_SOURCES})
target_compile_features(nat PUBLIC cxx_generalized_initializers)

add_executable(nat-tests tests/nat-tests.cc)
target_link_libraries(nat-tests nat)
target_compile_features(nat-tests PUBLIC cxx_generalized_initializers)

add_executable(int-tests tests/int-tests.cc)
target_link_libraries(int-tests nat)
target_compile_features(int-tests PUBLIC cxx_generalized_initializers)

add_executable(nat-demo demos/nat-demo.cc)
target_link_libraries(nat-demo nat)
target_compile_features(nat-demo PUBLIC cxx_generalized_initializers)
