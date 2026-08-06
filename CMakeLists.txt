#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(g4cmpPhonon)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})

# ── Find G4CMP ───────────────────────────────────────────────────────
# G4CMP uses old-style cmake variables (no imported target).
# Pass: cmake -DG4CMP_DIR=$HOME/soft/packages/G4CMP-V10-04-00_install/cmake ..
find_package(G4CMP REQUIRED)
include(${G4CMP_USE_FILE})

#----------------------------------------------------------------------------
# Setup Geant4/G4CMP include directories and compile definitions
# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Geant4_INCLUDE_DIRS})
include_directories(${G4CMP_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(g4cmpPhonon g4cmpPhonon.cc ${sources} ${headers})
target_link_libraries(g4cmpPhonon ${G4CMP_LIBRARIES} ${Geant4_LIBRARIES})


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(g4cmpPhonon DEPENDS g4cmpPhonon)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4cmpPhonon DESTINATION bin)
