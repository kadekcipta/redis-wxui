add_subdirectory(res)
include_directories(/usr/local/include)
link_directories(/usr/local/lib /usr/lib)
set(HIREDIS_STATIC_LIB libhiredis.a)

file(GLOB SRC_LIST *.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})
find_package(wxWidgets COMPONENTS core base aui REQUIRED)
if (wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${HIREDIS_STATIC_LIB})
endif(wxWidgets_FOUND)

