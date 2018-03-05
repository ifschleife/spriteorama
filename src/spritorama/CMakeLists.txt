set(APP_TARGET_NAME ${LAUNCHER_NAME})

set(SOURCES
    main.cpp
    main_window.cpp
    main_window.hpp
    opengl_canvas.cpp
    opengl_canvas.hpp
    viewport.cpp
    viewport.hpp
)

find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/src/thirdparty)

add_executable(${APP_TARGET_NAME} ${SOURCES})

target_compile_features(${APP_TARGET_NAME} PRIVATE cxx_std_17)

target_compile_options(${APP_TARGET_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${DEFAULT_COMPILE_OPTIONS_DEBUG}>")
target_compile_options(${APP_TARGET_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${DEFAULT_COMPILE_OPTIONS_RELEASE}>")

set_target_properties(${APP_TARGET_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX})

target_link_libraries(${APP_TARGET_NAME}
    Qt5::OpenGL
    Qt5::Widgets)

install(TARGETS ${APP_TARGET_NAME} DESTINATION bin)
