cmake_minimum_required(VERSION 3.5)
project(bbs CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BCRYPT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bcrypt/bcrypt.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bcrypt/blowfish.cpp
        controllers/NotificationController.h
        controllers/NotificationController.cc
        controllers/BrowseHistoryController.h
        controllers/BrowseHistoryController.cc
        controllers/FollowController.h
        controllers/FollowController.cc
        controllers/LikeController.h
        controllers/LikeController.cc
        filters/TokenHelper.h
        filters/TokenHelper.cc
)

add_executable(${PROJECT_NAME} main.cc
        third_party/bcrypt/bcrypt.h
        utils/BcryptUtils.h
        utils/BcryptUtils.cc
        utils/JwtUtils.h
        utils/JwtUtils.cc
        utils/ResponseUtil.cc
        utils/ResponseUtil.h
        ${BCRYPT_SOURCES})

target_compile_definitions(bbs PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines

set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/mysql-connector-c++-9.3.0-winx64/include")
set(MYSQL_LIBRARY_DIR "C:/Program Files/MySQL/mysql-connector-c++-9.3.0-winx64/lib64")

target_include_directories(${PROJECT_NAME} PRIVATE ${MYSQL_INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${MYSQL_LIBRARY_DIR})

find_package(Drogon CONFIG REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Drogon::Drogon
        jwt-cpp::jwt-cpp
        OpenSSL::SSL
)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bcrypt)
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)
