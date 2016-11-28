cmake_minimum_required(VERSION 2.8)
project(pdc_evol_model)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -fopenmp")

set(SOURCE_FILES main.cpp src/sources/World.cpp src/headers/World.h src/sources/GridCell.cpp src/headers/GridCell.h src/sources/Organism.cpp src/headers/Organism.h src/sources/DNA.cpp src/headers/DNA.h src/sources/BP.cpp src/headers/BP.h src/sources/BP_Protein_Block.cpp src/headers/BP_Protein_Block.h src/sources/BP_Protein.cpp src/headers/BP_Protein.h src/sources/BP_Pump_Block.cpp src/headers/BP_Pump_Block.h src/sources/BP_Pump.cpp src/headers/BP_Pump.h src/sources/BP_Move_Block.cpp src/headers/BP_Move_Block.h src/sources/BP_Move.cpp src/headers/BP_Move.h src/sources/RNA.cpp src/headers/RNA.h src/sources/Protein.cpp src/headers/Protein.h src/sources/Move.cpp src/headers/Move.h src/sources/Pump.cpp src/headers/Pump.h src/sources/Common.cpp src/headers/Common.h)
add_executable(pdc_evol_model ${SOURCE_FILES})

#find_package(SFML 2 REQUIRED graphics window system)
#if(SFML_FOUND)
#    include_directories(${SFML_INCLUDE_DIR})
#    target_link_libraries(pdc_evol_model ${SFML_LIBRARIES})
#endif(SFML_FOUND)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
    target_link_libraries(pdc_evol_model ${ZLIB_LIBRARIES})
endif(ZLIB_FOUND)
