cmake_minimum_required(VERSION 3.14.0)
project(hello LANGUAGES C)

# Lowercase project name for binaries and packaging
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

if(NOT AMIGA)
	message(SEND_ERROR "This project only compiles for Amiga")
endif()

set(ACE_DEBUG ON)
set(ACE_DEBUG_UAE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAMIGA -Wall -Wextra -fomit-frame-pointer")
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS src/*.h)

include_directories(${PROJECT_SOURCE_DIR}/src)

# Debugging flag exposed via CMake specifically for your game code
if(GAME_DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGAME_DEBUG")
endif()

# ACE
add_subdirectory(deps/ace ace)
include_directories(deps/ace/include)
# If you built standalone and installed ACE, use following instead:
# find_package(ace REQUIRED)
# include_directories(${ace_INCLUDE_DIRS})

# Force lower-case binary name for Linux etc.
set(TARGET_NAME ${PROJECT_NAME_LOWER})

if(ELF2HUNK)
  # Add elf2hunk step for Bartman compiler
  set(GAME_LINKED ${TARGET_NAME}.elf) # Intermediate executable
  set(GAME_EXE ${TARGET_NAME}.exe) # Use this to launch the game
  add_executable(${GAME_LINKED} ${SOURCES} ${HEADERS})
  add_custom_command(
    TARGET ${GAME_LINKED} POST_BUILD
    COMMAND ${ELF2HUNK} ${GAME_LINKED} ${GAME_EXE}
  )
else()
  # Just produce the executable with Bebbo compiler
  SET(GAME_LINKED ${TARGET_NAME})
  SET(GAME_EXE ${TARGET_NAME})
  add_executable(${GAME_LINKED} ${SOURCES} ${HEADERS})
endif()

target_link_libraries(${GAME_LINKED} ace)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/res) 
set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data) 
file(MAKE_DIRECTORY ${DATA_DIR}) 

set(PLT_PATH ${DATA_DIR}/vk.plt)
convertPalette(hello.elf ${RES_DIR}/vk.pal ${PLT_PATH})

convertBitmaps(
	TARGET hello.elf PALETTE ${PLT_PATH}
	INTERLEAVED SOURCES
		${RES_DIR}/staryWicher.png
    ${RES_DIR}/hud_square_big.png
    ${RES_DIR}/rand_encounter_enemies.png
	DESTINATIONS
		${DATA_DIR}/staryWicher.bm 
    ${DATA_DIR}/hud_square_big.bm
    ${DATA_DIR}/rand_encounter_enemies.bm
)