#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(vcalo VERSION 1.0.0 LANGUAGES C CXX )
add_compile_options(-w)

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
find_package(yaml-cpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Acts REQUIRED COMPONENTS Core)

message(STATUS "Acts include: " ${Acts_include})
# get_target_property(ACTS_INCLUDE_DIR Acts::Core INTERFACE_INCLUDE_DIRECTORIES)
# message(STATUS "ACTS Core Include Directory: ${ACTS_INCLUDE_DIR}")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${YAML_CPP_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR}
		    ${ROOT_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIR})

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(vcalo vcalo.cc ${sources} ${headers})
add_executable(vana vana.cc ${sources} ${headers})
target_link_libraries(vcalo 
    ${ROOT_LIBRARIES}
    ROOT::Geom
    yaml-cpp
    ${Geant4_LIBRARIES}
    ActsCore
)
target_link_libraries(vana
    ${ROOT_LIBRARIES}
    ROOT::Geom
    yaml-cpp
    ${Geant4_LIBRARIES}
    ActsCore
)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS vcalo DESTINATION bin)

