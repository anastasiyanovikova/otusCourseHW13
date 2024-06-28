cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "5" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(sort_elements VERSION ${PROJECT_VESRION})

add_executable(sort_elements main.cpp)
add_library(timsort timsort.cpp)
add_library(sortedElements sortedElements.cpp)

set_target_properties(sort_elements timsort sortedElements PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)


target_include_directories(timsort
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_include_directories(sortedElements
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(sort_elements
sortedElements
)


if (MSVC)
    target_compile_options(sort_elements PRIVATE
        /W4
    )
else ()
    target_compile_options(sort_elements PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(timsort PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(sortedElements PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS sort_elements RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "NN")
include(CPack)
