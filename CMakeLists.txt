#https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-ios-tvos-or-watchos
#rm ./* && cmake .. -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS && open 3d4.xcodeproj/
#
cmake_minimum_required(VERSION 3.17)



PROJECT(chain-gui)
list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake/macros")
include(AutoCollect)
set(CMAKE_CXX_STANDARD 17)

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
link_directories(${PROJECT_BINARY_DIR}/)
# lib blend2d
set(BLEND2D_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/blend2d"
        CACHE PATH "Location of 'blend2d'")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/blend2d/src/)
# Enable Blend2D static build.
set(BLEND2D_STATIC TRUE)
include("${BLEND2D_DIR}/CMakeLists.txt")
link_directories(${PROJECT_BINARY_DIR}/thirdparty/blend2d/build)
set(LIBS ${LIBS}  libblend2d.a Blend2D::Blend2D)


set(USE_GCC ON)
set(SDL 1)
set(GLFW 0)
set(IOS 0)
set(ASSIMP 0)
set(IMGUI 0)
set(GLEW 0)
set(GLAD 0)


IF (IOS)
    set(SDL 1)
    set(GLFW 0)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIOS")
ENDIF ()

IF (IOS)
    set(GLAD 0)
ELSEIF(EMSCRIPTEN)
    set(GLAD 0)
ELSEIF (GLAD)
ENDIF ()

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 1)

IF(SDL)
    add_definitions(-DSDL_BACKEND)
ENDIF()

# cd build
# cmake .. -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_CXX_FLAGS="-static"
PickIncludeDirectories(
        ${CMAKE_CURRENT_SOURCE_DIR}/code
        INCLUDEDIRS
        # Exclude
        ${CMAKE_CURRENT_SOURCE_DIR}/build
        ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
        ${CMAKE_CURRENT_SOURCE_DIR}/code/util
        ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
)

set(INCLUDEDIRS ${INCLUDEDIRS})
PickSourceFiles(
        ${CMAKE_CURRENT_SOURCE_DIR}/code
        CODEFILES
        # Exclude
        ${CMAKE_CURRENT_SOURCE_DIR}/build
        ${CMAKE_CURRENT_SOURCE_DIR}/code/GameLogic
        ${CMAKE_CURRENT_SOURCE_DIR}/code/util
        ${CMAKE_CURRENT_SOURCE_DIR}/code/Graphics
)


MakeFilter(
        ${CMAKE_CURRENT_SOURCE_DIR}/code)


#add_library(STB_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/stb/stb_image.cpp")

#include_directories(${CMAKE_PROJECT_NAME}
#        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/stb/include/)
#set(LIBS ${LIBS} STB_IMAGE)



if(GLAD)
    set(CODEFILES ${CODEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glad/glad.c)
    set(INCLUDEDIRS ${INCLUDEDIRS} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/glad/include/)
endif()

set(RES2 )
if (IOS)
    set(RES2 ${RES2} ${CMAKE_CURRENT_SOURCE_DIR}/data.0 code/ui/BaseControl.cpp code/ui/BaseControl.h code/utf8.h code/TextLayout.cpp code/TextLayout.h)
endif()


add_executable(${CMAKE_PROJECT_NAME}
        ${CODEFILES} ${RES2})

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${INCLUDEDIRS})

#HeadOnly Library
IF(RAPIDJSON)
    target_include_directories(${CMAKE_PROJECT_NAME}
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/rapidjson/)
ENDIF()

target_include_directories(${CMAKE_PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/general/entt-3.4.0/src/)

IF(PROTOBUF)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ELSE ()

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/protobuf/include/)

    ENDIF ()
ENDIF()

if (NOT WEB)
    IF (WINDOWS)
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
        set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
        IF(gcc)
            set(LIBS ${LIBS} m user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8 dxerr8)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -lmingw32 -Wl,--no-undefined -mwindows  -mconsole")

        ELSEIF(msvc)

            #            include_directories(${CMAKE_PROJECT_NAME}
            #                    PUBLIC ${PROJECT_BINARY_DIR}/thirdparty/SDL/include)
            #
            #            include_directories(${CMAKE_PROJECT_NAME}
            #                    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
            #            set(LIBS ${LIBS} user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32 dinput8 dxerr8)
        ENDIF()


        #        set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -mconsole")
        #        target_link_libraries(${target_name} )
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
    ELSEIF (LINUX)

        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL")
        set(LIBS ${LIBS} SDL2::SDL2main SDL2::SDL2-static )
        include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)

        #        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath=/usr/lib/x86_64-linux-gnu/ -ludev")
        #        set(LIBS ${LIBS} /usr/lib/x86_64-linux-gnu/libudev.so ${SDL2_LIBRARIES})
        #        #        set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -mconsole")
        #        #        target_link_libraries(${target_name} )
        #        include_directories(${CMAKE_PROJECT_NAME}
        #                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL/include)
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


IF(FMT)
    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ELSE ()
        target_include_directories(${CMAKE_PROJECT_NAME}
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/win64/fmt/include/)

    ENDIF ()
ENDIF()

IF (CMAKE_SYSTEM_NAME MATCHES "Windows") #target_link_libraries(MyExec PUBLIC "-static")//-fno-exceptions  -fdata-sections -ffunction-sections -Wl,-gc-sections -Wl,-strip-all
    IF (USE_GCC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O -static -fdata-sections -ffunction-sections -Wl,-gc-sections -Wl,-strip-all ")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${LIBS} )
    ELSE ()

        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        target_link_libraries(${CMAKE_PROJECT_NAME}
                ${LIBS})
    ENDIF ()
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_link_libraries(${CMAKE_PROJECT_NAME} pthread
            )
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    target_link_libraries(${CMAKE_PROJECT_NAME}
            pthread
            ${LIBS}
            )
    MESSAGE("MACOS")
ELSEIF (IOS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DGLES_SILENCE_DEPRECATION)
    target_link_libraries(${CMAKE_PROJECT_NAME}
            pthread
            ${LIBS}
            )
    #    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios")
    #    add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE main.cpp)
    #        -Wl,-framework,AudioToolbox
    #        -Wl,-framework,AVFoundation
    #        -Wl,-framework,CoreAudio
    #        -Wl,-framework,CoreBluetooth
    #        -Wl,-framework,CoreGraphics
    #        -Wl,-framework,CoreMotion
    #        -Wl,-framework,Foundation
    #        -Wl,-framework,GameController
    #        -Wl,-framework,Metal
    #        -Wl,-framework,OpenGLES
    #        -Wl,-framework,QuartzCore
    #        -Wl,-framework,UIKit
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

    #set(bundle_resources ${JUCER_ICON_FILE})
    #if(IOS)
    #list(APPEND RES
    #${JUCER_XCASSETS} ${JUCER_LAUNCH_STORYBOARD_FILE}
    #)
    #endif()

    #set_source_files_properties(${RES} PROPERTIES
    #MACOSX_PACKAGE_LOCATION "Resources"
    #)
    #source_group("Resources" FILES ${RES})

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/MacOSXBundleInfo.plist.in
            MACOSX_BUNDLE ON
            MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_FRAMEWORK_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_IDENTIFIER com.myname.${CMAKE_PROJECT_NAME}
            MACOSX_BUNDLE_SHORT_VERSION_STRING "123"
            MACOSX_BUNDLE_LONG_VERSION_STRING "456"
            MACOSX_BUNDLE_BUNDLE_VERSION "123"
            RESOURCE "${RES2}"
            #MACOSX_BUNDLE_ICON_FILE glfw.icns
            )
    MESSAGE("IOS")
ENDIF ()


if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_ES3")
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS
            "-s ASYNCIFY=1 -s USE_SDL=2  -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 --preload-file data.0 -O --closure 1 -s FULL_ES3=1")
    # -s ERROR_ON_UNDEFINED_SYMBOLS=0  -s USE_GLFW=3 -s USE_SDL=2 -s FULL_ES3=1 -s FULL_ES2=1 -s USE_WEBGL2=1  --preload-file data.0 --shell-file shell_minimal.html -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
endif()
