#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(vcalo VERSION 1.0.0 LANGUAGES C CXX )
add_compile_options(-w)
# add_definitions(-std=c++17)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED COMPONENTS TMVA ROOTVecOps ROOTDataFrame Geom Eve)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(yaml-cpp REQUIRED)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${YAML_CPP_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/Simulation
                    ${PROJECT_SOURCE_DIR}/include/Analysis
                    ${Geant4_INCLUDE_DIR}
		    ${ROOT_INCLUDE_DIR})
link_directories(/cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/lib)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/Simulation/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/Simulation/*.hh)

file(GLOB anasrc ${PROJECT_SOURCE_DIR}/src/Analysis/*.cc)
file(GLOB anainc ${PROJECT_SOURCE_DIR}/include/Analysis/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(vcalo vcalo.cc ${sources} ${headers})
target_link_libraries(vcalo ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} yaml-cpp )

add_executable(vana vana.cc ${anasrc} ${anainc})
target_link_libraries(vana ${ROOT_LIBRARIES} yaml-cpp onnxruntime)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
# install(TARGETS vcalo DESTINATION bin)

