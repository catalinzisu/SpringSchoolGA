#--------------------------------------------------------------
# 
# Example of CMake configuration file to build an external 
# project depending on Chrono and on optional Chrono modules.
# 
# This minimal sample project can be used as a template for a
# user project.  Modify sections 1, 2, and 3 below as appropriate.
# 
#--------------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

#--------------------------------------------------------------
# === 1 === 
# Modify the project name if you want: 
#--------------------------------------------------------------

project(ChronoProject_FinalGA)

# Show CHRONO_LIB_PATH only on macOS (so it appears in cmake-gui on macOS even
# if Chrono isn't found and the configure aborts). This avoids showing it on Windows/Linux.


# CHRONO_LIB_PATH will be shown only on macOS; it is declared above inside the if(APPLE) block.

find_package(Chrono REQUIRED COMPONENTS Irrlicht Postprocess CONFIG)

#--------------------------------------------------------------
# Return now if Chrono or a required component was not found.
#--------------------------------------------------------------

if (NOT Chrono_FOUND)
  message( FATAL_ERROR "Could not find Chrono or one of its required modules")
  SET(Chrono_DIR "${ChronoProject_FinalGA_SOURCE_DIR}/../chrono_build/cmake")
  LIST(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}/../Chrono/lib")
endif()

#--------------------------------------------------------------
# Enable creation of "application bundles" on MacOSX.
#--------------------------------------------------------------

# This is necessary for any Irrlicht-based project (like the example here).
# For OpenGL-based or non-graphics projects, this is optional and the block
# below can be removed (or else explcitly set CMAKE_MACOSX_BUNDLE to 'OFF').
#
# If creating application bundles, the build output will be named 'myexe.app'.
# Use the convenience script 'run_app.sh' available under 'contrib/appbundle-macosx/'
# to run:
#     start_demo.sh myexe.app

#--------------------------------------------------------------
# Add path to Chrono headers and to headers of all dependencies
# of the requested modules.
#--------------------------------------------------------------

# include directories are added to the target after the executable is created

#--------------------------------------------------------------
# Tweaks to disable some warnings with MSVC
#--------------------------------------------------------------
if(MSVC)
    add_definitions("-D_CRT_SECURE_NO_DEPRECATE")  # avoids deprecation warnings
    add_definitions("-D_SCL_SECURE_NO_DEPRECATE")  # avoids deprecation warnings
    add_definitions( "-DNOMINMAX" )                # do not use MSVC's min/max macros
    set(EXTRA_COMPILE_FLAGS "/wd4275")             # disable warnings triggered by Irrlicht
else()
    set(EXTRA_COMPILE_FLAGS "")
endif()

#--------------------------------------------------------------
# === 3 ===
# Add the executable from your project and specify all C++ 
# files in your project. 
#--------------------------------------------------------------

set(INCROOT ${PROJECT_SOURCE_DIR}/inc)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE INCS 
	"${INCROOT}/*.h"
)
file(GLOB_RECURSE SRCS 
	"${SRCROOT}/*.cpp"
)

ADD_EXECUTABLE(ChronoProject_FinalGA
${INCS} 
${SRCS}
)   

foreach(FILE ${SRCS}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${INCS}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

#--------------------------------------------------------------
# Set properties for your executable target
# 
# Note that here we define a macro CHRONO_DATA_DIR which will
# contain the path to the Chrono data directory, either in its
# source tree (if using a build version of Chrono), or in its
# install tree (if using an installed version of Chrono).
#--------------------------------------------------------------

set_target_properties(ChronoProject_FinalGA PROPERTIES 
    COMPILE_FLAGS "${CHRONO_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS}"
    LINK_FLAGS "${CHRONO_LINKER_FLAGS}"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)
    
# Define CHRONO_DATA_DIR for the target (same as the sample architecture)
target_compile_definitions(ChronoProject_FinalGA PRIVATE CHRONO_DATA_DIR="${CHRONO_DATA_DIR}")

#--------------------------------------------------------------
# Link to Chrono libraries and dependency libraries
#--------------------------------------------------------------

target_link_libraries(ChronoProject_FinalGA ${CHRONO_LIBRARIES})

# Add include directories for the target (project headers + Chrono includes)
target_include_directories(ChronoProject_FinalGA PRIVATE ${INCROOT} ${CHRONO_INCLUDE_DIRS})

#--------------------------------------------------------------
# === 4 (OPTIONAL) ===
# 
# Optionally, add a custom command for copying all Chrono and
# dependency DLLs to the appropriate binary output folder.
# This function has effect only on Windows.
#
# Note that you must first set EXECUTABLE_OUTPUT_PATH
# (this can simply be ${CMAKE_BINARY_DIR}, like in this example)
#--------------------------------------------------------------

if(MSVC)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
    add_DLL_copy_command("${CHRONO_DLLS}")

    set (DEBUG_PATH ${ChronoProject_FinalGA_SOURCE_DIR}/build/Debug)
    set (CHRONO_DEBUG_PATH ${Chrono_DIR}/../bin/Debug)

    set (RELEASE_PATH ${ChronoProject_FinalGA_SOURCE_DIR}/build/Release)
    set (CHRONO_RELEASE_PATH ${Chrono_DIR}/../bin/Release)

    file(COPY
       ${CHRONO_RELEASE_PATH}/ChronoEngine.dll
       DESTINATION ${RELEASE_PATH})

    file(COPY
        ${CHRONO_RELEASE_PATH}/ChronoEngine_irrlicht.dll
        DESTINATION ${RELEASE_PATH})

    file(COPY
        ${CHRONO_RELEASE_PATH}/ChronoEngine_postprocess.dll
        DESTINATION ${RELEASE_PATH})

    file(COPY
        ${CHRONO_RELEASE_PATH}/Irrlicht.dll
        DESTINATION ${RELEASE_PATH})
endif()

include_directories(ChronoProject_FinalGA ${ChronoProject_FinalGA_SOURCE_DIR}/inc)

# --------------------
# macOS-specific settings for the target (bundle, rpath, copy dylibs)
# Added after the target is created and linked.
# --------------------
if(APPLE)
    # Prefer creating an app bundle for macOS and make the executable look in its own folder at runtime
    set(CMAKE_MACOSX_BUNDLE ON)
    set_target_properties(ChronoProject_FinalGA PROPERTIES BUILD_RPATH "@executable_path")

    # Expose CHRONO_LIB_PATH in cmake-gui on macOS so the user can pick where Chrono dylibs live.
    # The variable is a PATH cache entry so it appears in graphical frontends like cmake-gui / ccmake.
    set(CHRONO_LIB_PATH "" CACHE PATH "Path to Chrono dylibs directory (contains libChronoEngine.dylib). Set via cmake-gui on macOS.")

    # Simple flow: require the user to set CHRONO_LIB_PATH. If it's unset, skip post-build copy.
    if(NOT CHRONO_LIB_PATH)
        message(STATUS "CHRONO_LIB_PATH not set. Set CHRONO_LIB_PATH in cmake-gui to the directory containing Chrono dylibs (e.g. /Users/you/chrono-install/lib). Post-build copy will be skipped.")
    else()
        message(STATUS "Using Chrono dylib directory: ${CHRONO_LIB_PATH}")

        # Create a minimal Info.plist for the macOS bundle (harmless if a user already provides one)
        set(_plist "${CMAKE_BINARY_DIR}/Info.plist")
        file(WRITE ${_plist}
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
            "<plist version=\"1.0\">\n"
            "<dict>\n"
            "    <key>CFBundleName</key>\n"
            "    <string>${PROJECT_NAME}</string>\n"
            "    <key>CFBundleExecutable</key>\n"
            "    <string>${PROJECT_NAME}</string>\n"
            "    <key>CFBundleIdentifier</key>\n"
            "    <string>com.yourcompany.${PROJECT_NAME}</string>\n"
            "    <key>CFBundleShortVersionString</key>\n"
            "    <string>1.0</string>\n"
            "    <key>CFBundleVersion</key>\n"
            "    <string>1</string>\n"
            "    <key>NSPrincipalClass</key>\n"
            "    <string>NSApplication</string>\n"
            "    <key>NSHighResolutionCapable</key>\n"
            "    <false/>\n"
            "</dict>\n"
            "</plist>\n"
        )

        set_target_properties(ChronoProject_FinalGA PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${_plist}
        )
        # Explicit per-file copies from the user-provided CHRONO_LIB_PATH. Keep simple and explicit.
        if(EXISTS "${CHRONO_LIB_PATH}/libChronoEngine.dylib")
            add_custom_command(TARGET ChronoProject_FinalGA POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CHRONO_LIB_PATH}/libChronoEngine.dylib" $<TARGET_FILE_DIR:ChronoProject_FinalGA>
            )
            message(STATUS "Will copy libChronoEngine.dylib from ${CHRONO_LIB_PATH}")
        else()
            message(WARNING "libChronoEngine.dylib not found at ${CHRONO_LIB_PATH}; it will not be copied.")
        endif()

        if(EXISTS "${CHRONO_LIB_PATH}/libChronoEngine_postprocess.dylib")
            add_custom_command(TARGET ChronoProject_FinalGA POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CHRONO_LIB_PATH}/libChronoEngine_postprocess.dylib" $<TARGET_FILE_DIR:ChronoProject_FinalGA>
            )
            message(STATUS "Will copy libChronoEngine_postprocess.dylib from ${CHRONO_LIB_PATH}")
        else()
            message(WARNING "libChronoEngine_postprocess.dylib not found at ${CHRONO_LIB_PATH}; it will not be copied.")
        endif()

        if(EXISTS "${CHRONO_LIB_PATH}/libChronoEngine_irrlicht.dylib")
            add_custom_command(TARGET ChronoProject_FinalGA POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CHRONO_LIB_PATH}/libChronoEngine_irrlicht.dylib" $<TARGET_FILE_DIR:ChronoProject_FinalGA>
            )
            message(STATUS "Will copy libChronoEngine_irrlicht.dylib from ${CHRONO_LIB_PATH}")
        else()
            message(WARNING "libChronoEngine_irrlicht.dylib not found at ${CHRONO_LIB_PATH}; it will not be copied.")
        endif()

        if(EXISTS "${CHRONO_LIB_PATH}/libChronoModels_robot.dylib")
            add_custom_command(TARGET ChronoProject_FinalGA POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CHRONO_LIB_PATH}/libChronoModels_robot.dylib" $<TARGET_FILE_DIR:ChronoProject_FinalGA>
            )
            message(STATUS "Will copy libChronoModels_robot.dylib from ${CHRONO_LIB_PATH}")
        else()
            message(WARNING "libChronoModels_robot.dylib not found at ${CHRONO_LIB_PATH}; it will not be copied.")
        endif()

        # Optional: copy Homebrew-provided Irrlicht if present at the common location
        if(EXISTS "/opt/homebrew/lib/libIrrlicht.dylib")
            add_custom_command(TARGET ChronoProject_FinalGA POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "/opt/homebrew/lib/libIrrlicht.dylib" $<TARGET_FILE_DIR:ChronoProject_FinalGA>
            )
            message(STATUS "Will copy /opt/homebrew/lib/libIrrlicht.dylib into the bundle")
        else()
            message(WARNING "/opt/homebrew/lib/libIrrlicht.dylib not found; Irrlicht will not be copied automatically.")
        endif()
    endif()
endif()
