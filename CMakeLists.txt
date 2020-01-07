#=========================================================================
#
# Copyright 2019 Kitware, Inc.
# Author: Matthieu Zins
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

cmake_minimum_required(VERSION 3.10)

project(DetectionsAssociationTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


# Eigen
find_package(Eigen3 REQUIRED)

# Qt dependancy
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# OpenCV
find_package(OpenCV REQUIRED)

# Yaml-cpp
find_package(yaml-cpp REQUIRED)

set(SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)


# The main application
add_executable(DetectionsAssociationTool
    assets/resources.qrc
    ${SRC_DIRECTORY}/utils.cxx
    ${SRC_DIRECTORY}/utils.h
    ${SRC_DIRECTORY}/object.cxx
    ${SRC_DIRECTORY}/object.h
    ${SRC_DIRECTORY}/image.cxx
    ${SRC_DIRECTORY}/image.h
    ${SRC_DIRECTORY}/customLabel.cxx
    ${SRC_DIRECTORY}/customLabel.h
    ${SRC_DIRECTORY}/detection.cxx
    ${SRC_DIRECTORY}/detection.h
    ${SRC_DIRECTORY}/main.cxx
    ${SRC_DIRECTORY}/MainWindow.cxx
    ${SRC_DIRECTORY}/MainWindow.h
    ${SRC_DIRECTORY}/MainWindow.ui
)

target_include_directories(DetectionsAssociationTool SYSTEM PRIVATE
    ${SRC_DIRECTORY}/
    ${YAML_CPP_INCLUDE_DIR}
    )
target_link_libraries(DetectionsAssociationTool PRIVATE
  Qt5::Widgets
  ${OpenCV_LIBS}
  Eigen3::Eigen
  stdc++fs          # for std::filesystem
  ${YAML_CPP_LIBRARIES}
  )

