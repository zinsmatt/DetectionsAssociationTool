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

# VTK dependancy
#find_package(VTK 8.1.1 REQUIRED)
#include(${VTK_USE_FILE})

# YAML-cpp
find_package(yaml-cpp REQUIRED)

set(SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)


# The main application
add_executable(BoundingBoxMe
    assets/resources.qrc
#    ${SRC_DIRECTORY}/boundingBox.h
#    ${SRC_DIRECTORY}/boundingBox.cxx
#    ${SRC_DIRECTORY}/boundingBoxInformation_ui.ui
#    ${SRC_DIRECTORY}/boundingBoxInformation_ui.cxx
#    ${SRC_DIRECTORY}/boundingBoxInformation_ui.h
#    ${SRC_DIRECTORY}/boundingBoxManager.h
#    ${SRC_DIRECTORY}/boundingBoxManager.cxx
#    ${SRC_DIRECTORY}/cameraFrame.cxx
#    ${SRC_DIRECTORY}/cameraFrame.h
#    ${SRC_DIRECTORY}/cameraFramesManager.cxx
#    ${SRC_DIRECTORY}/cameraFramesManager.h
#    ${SRC_DIRECTORY}/classesManager.cxx
#    ${SRC_DIRECTORY}/classesManager.h
#    ${SRC_DIRECTORY}/datasetHelper.cxx
#    ${SRC_DIRECTORY}/datasetHelper.h
#    ${SRC_DIRECTORY}/displayManager_ui.cxx
#    ${SRC_DIRECTORY}/displayManager_ui.h
#    ${SRC_DIRECTORY}/displayManager_ui.ui
#    ${SRC_DIRECTORY}/mainViewInteractorStyle.cxx
#    ${SRC_DIRECTORY}/mainViewInteractorStyle.h
#    ${SRC_DIRECTORY}/lidarFrame.cxx
#    ${SRC_DIRECTORY}/lidarFrame.h
#    ${SRC_DIRECTORY}/lidarFrameManager.cxx
#    ${SRC_DIRECTORY}/lidarFrameManager.h
#    ${SRC_DIRECTORY}/logger.cxx
#    ${SRC_DIRECTORY}/logger.h
    ${SRC_DIRECTORY}/main.cxx
    ${SRC_DIRECTORY}/MainWindow.cxx
    ${SRC_DIRECTORY}/MainWindow.h
    ${SRC_DIRECTORY}/MainWindow.ui
#    ${SRC_DIRECTORY}/sideViewsInteractorStyle.cxx
#    ${SRC_DIRECTORY}/sideViewsInteractorStyle.h
#    ${SRC_DIRECTORY}/switchViewportCallback.cxx
#    ${SRC_DIRECTORY}/switchViewportCallback.h
#    ${SRC_DIRECTORY}/timeStepsManager.h
#    ${SRC_DIRECTORY}/timeStepsManager.cxx
#    ${SRC_DIRECTORY}/timeStepsManager_ui.cxx
#    ${SRC_DIRECTORY}/timeStepsManager_ui.h
#    ${SRC_DIRECTORY}/timeStepsManager_ui.ui
#    ${SRC_DIRECTORY}/vtkBoundingBoxManipulatorWidget.cxx
#    ${SRC_DIRECTORY}/vtkBoundingBoxManipulatorWidget.h
#    ${SRC_DIRECTORY}/vtkEigenUtils.cxx
#    ${SRC_DIRECTORY}/vtkEigenUtils.h
#    ${SRC_DIRECTORY}/vtkBoundingBoxSource.h
#    ${SRC_DIRECTORY}/vtkBoundingBoxSource.cxx
)

target_include_directories(BoundingBoxMe SYSTEM PRIVATE
    #${VTK_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIR}
    ${SRC_DIRECTORY}/
    )
target_link_libraries(BoundingBoxMe PRIVATE
  Qt5::Widgets
  #${VTK_LIBRARIES}
  Eigen3::Eigen
  ${YAML_CPP_LIBRARIES}
  stdc++fs          # for std::filesystem
  )

