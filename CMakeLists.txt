
find_package(OpenCV REQUIRED)

add_subdirectory(EcoTracker)

add_executable(dolly_zoom "dolly_zoom.cpp")
target_link_libraries(dolly_zoom ${OpenCV_LIBS} ecotracker)
