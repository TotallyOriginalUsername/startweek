# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)


# Gather source files using wildcards for src directory
file(GLOB SRC_HARDWARE_SOURCES 			"../hardware/src/*.c")
file(GLOB SRC_HARDWARECONTROL_SOURCES 	"../hardwareControl/src/*.c")
file(GLOB SRC_MAIN_SOURCES 				"src/*.c")
file(GLOB SRC_STATES_SOURCES 			"../states/src/*.c")
file(GLOB SRC_STATEMACHINE 				"../main/src/statemachine.c")
file(GLOB SRC_THREADS 					"../main/src/threads.c")

# Add your source files
target_sources(app PRIVATE 
    ${SRC_HARDWARE_SOURCES}
	${SRC_HARDWARECONTROL_SOURCES}
	${SRC_MAIN_SOURCES}
	${SRC_STATES_SOURCES}
	${SRC_STATEMACHINE}
	${SRC_THREADS}
)

# Add include directories
target_include_directories(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../hardware/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../hardwareControl/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../main/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../states/inc
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)