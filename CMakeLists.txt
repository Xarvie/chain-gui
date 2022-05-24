cmake_minimum_required(VERSION 3.10)

project(APP)
# cmake .. -DCMAKE_BUILD_TYPE=Release
# cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
# cmake .. -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS
set(SERVER_ONLY 0)
IF(EMSCRIPTEN)
    set(WEB 1)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "iOS")

    set(IOS 1)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Android")
    set(ANDROID 1)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(MACOS 1)
ELSEIF(WIN32)
    set(WINDOWS 1)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX 1)
ELSE()
    MESSAGE("OS NAME UNDEFINED ")
ENDIF()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(clang 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(gcc 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(icc 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(msvc 1)
endif()

list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake/macros")
include(cmake/macros/AutoCollect.cmake)
set(CMAKE_CXX_STANDARD 17)
IF(SERVER_ONLY)
    PickIncludeDirectories(
            ${CMAKE_CURRENT_SOURCE_DIR}/code
            INCLUDEDIRS
            # Exclude
            ${CMAKE_CURRENT_SOURCE_DIR}/code/client
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Device
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GraphicsBase
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics

    )
    PickSourceFiles(
            ${CMAKE_CURRENT_SOURCE_DIR}/code
            CODEFILES
            # Exclude
            ${CMAKE_CURRENT_SOURCE_DIR}/code/client
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Device
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GraphicsBase
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
    )

ELSE()
    PickIncludeDirectories(
            ${CMAKE_CURRENT_SOURCE_DIR}/code
            INCLUDEDIRS
            # Exclude
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
            ${CMAKE_CURRENT_SOURCE_DIR}/code/server/serverOnly
    )
    PickSourceFiles(
            ${CMAKE_CURRENT_SOURCE_DIR}/code
            CODEFILES
            # Exclude
            ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
            ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
            ${CMAKE_CURRENT_SOURCE_DIR}/code/server/serverOnly
    )

ENDIF()


IF(SERVER_ONLY)
ELSE()
    include_directories(${CMAKE_PROJECT_NAME}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb/include/)
ENDIF()

IF(OPENCV)
    find_package( OpenCV REQUIRED )
    include_directories("C:\\msys64\\mingw64\\include\\opencv4\\")
    set(LIBS ${LIBS} ${OpenCV_LIBS})
    #            set(LIBS ${LIBS} z)
ENDIF()

if(EMSCRIPTEN)
    set(INCLUDEDIRS ${INCLUDEDIRS} c:/msys64/usr/local/emsdk/upstream/emscripten/cache/wasm/include/SDL2)
endif()

IF(SERVER_ONLY)
ELSE()
    link_directories(${PROJECT_BINARY_DIR}/)
    # lib blend2d
    set(BLEND2D_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/blend2d"
            CACHE PATH "Location of 'blend2d'")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/blend2d/src/)
    # Enable Blend2D static build.
    set(BLEND2D_STATIC TRUE)
    include("${BLEND2D_DIR}/CMakeLists.txt")
    link_directories(${PROJECT_BINARY_DIR}/thirdparty/)
    set(LIBS ${LIBS}   Blend2D::Blend2D)
ENDIF()

if (NOT WEB)
    IF(SERVER_ONLY)
    ELSE()
    ADD_DEFINITIONS(-DSDL_BACKEND)
    ENDIF()
    IF (WINDOWS)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
        set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
        IF(gcc)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -lmingw32 -Wl,--no-undefined -mwindows -mconsole")

        ELSEIF(msvc)
        ENDIF()
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
       ELSEIF (LINUX)
        find_package(SDL2)
        if (SDL2_FOUND)
            include_directories(${SDL2_INCLUDE_DIRS})
            set(LIBS ${LIBS} ${SDL2_LIBRARIES})
        else()
            add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
            set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
            include_directories(${CMAKE_PROJECT_NAME}
                    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
        endif()
    ELSEIF(ANDROID)
        add_subdirectory(thirdparty/SDL)

        find_library(SDL2 SDL2)
        set(LIBS ${LIBS} SDL2)
    ELSEIF(MACOS)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
        #        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-search_paths_first  -Wno-unused-command-line-argument -lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal")
        set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
        #        target_link_libraries(${target_name} )
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
    ELSEIF(IOS)

        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-search_paths_first  -Wno-unused-command-line-argument -lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal")
        set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
        #        target_link_libraries(${target_name} )
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
    ELSE()

    ENDIF()

    #ELSE()
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lSDL2")
ENDIF()



IF(IOS)
    ADD_DEFINITIONS(-DIOS)
    ADD_DEFINITIONS(-DGLES_SILENCE_DEPRECATION)
    set(RES ${RES} ${CMAKE_CURRENT_SOURCE_DIR}/data.0)
ENDIF()

IF(ANDROID)
    add_library(${CMAKE_PROJECT_NAME} SHARED)
    set(LIBS ${LIBS} GLESv3  log)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CODEFILES})
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "main" )
ELSEIF(IOS)
    add_executable(${CMAKE_PROJECT_NAME} ${CODEFILES} ${RES})
ELSE()
    add_executable(${CMAKE_PROJECT_NAME} ${CODEFILES})
ENDIF()

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    ADD_DEFINITIONS("-DNOMINMAX")
    target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
ELSEIF(ANDROID)
ELSE()
    target_link_libraries(${CMAKE_PROJECT_NAME} pthread)
ENDIF ()

IF(IOS)

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            XCODE_ATTRIBUTE_OTHER_LDFLAGS "${XCODE_ATTRIBUTE_OTHER_LDFLAGS}
            -framework AudioToolbox
            -framework AudioToolbox
            -framework AVFoundation
            -framework CoreAudio
            -framework CoreBluetooth
            -framework CoreGraphics
            -framework CoreMotion
            -framework Foundation
            -framework CoreFoundation
            -framework GameController
            -framework Metal
            -framework OpenGLES
            -framework QuartzCore
            -framework UIKit
")

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/MacOSXBundleInfo.plist.in
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER com.caspasex13.${CMAKE_PROJECT_NAME}
            MACOSX_FRAMEWORK_IDENTIFIER com.caspasex13.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_IDENTIFIER com.caspasex13.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_SHORT_VERSION_STRING "123"
            MACOSX_BUNDLE_LONG_VERSION_STRING "456"
            MACOSX_BUNDLE_BUNDLE_VERSION "123"
            RESOURCE "${RES}"
            XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
            #MACOSX_BUNDLE_ICON_FILE glfw.icns
            )
ENDIF()
target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include/)

target_link_libraries(${CMAKE_PROJECT_NAME} ${CMAKE_DL_LIBS} ${LIBS})
target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${INCLUDEDIRS})

IF(msvc)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 ")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    #    target_compile_definitions(foo PUBLIC /wd4101)
ELSE()

ENDIF()
IF(msvc)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /Od")
ELSE()
    set(CMAKE_CXX_FLAGS_RELEASE "-static ${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -g")
ENDIF()

#add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E
#        copy ${CMAKE_CURRENT_SOURCE_DIR}/data.0 ${CMAKE_CURRENT_SOURCE_DIR}/../AS/app/src/main/assets/data.0)
#add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E
#        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Script ${PROJECT_BINARY_DIR}/script)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS
            "-s ASYNCIFY=1 -s USE_SDL=2 -s USE_WEBGL2=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --preload-file data.0 -O2 --closure 1 -s FULL_ES3=1 --shell-file shell_minimal.html")
    # -s ERROR_ON_UNDEFINED_SYMBOLS=0  -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s FULL_ES2=1 -s USE_WEBGL2=1  --preload-file data.0 --shell-file shell_minimal.html -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
endif()
