# Example tp_maps and SDL2 Application
This project demonstrates building a simple 3D view using tdp-libs and SDL2 using CMake or QMake. This project should build on Linux and OSX and deploy to desktop, iOS, and Android. Support for Windows has not yet been added.

## Prerequisites 
* tdp_tools [Installation instructions](https://github.com/tdp-libs/tdp_tools)
* A toolchain for your chosen platform. [Installation instructions](https://github.com/tdp-libs/tdp_build)

## Clone Instructions
These steps will clone the repo and all of its dependencies. You will need tdp_tools in your path for this to work.
```
mkdir maps_fps
cd maps_fps
git clone https://github.com/tdp-libs/example_maps_fps.git
cp example_maps_fps/maps_fps.pro .
cp example_maps_fps/CMakeLists.top CMakeLists.txt
tdpUpdate
```

## Include and Library Paths
Depending on your platform you may need to specify include and library paths. The previous steps should have created a `maps_fps/project.inc` file, this can be modified to add platform specific include and library paths. If these modifications may be of use to someone else then they can be added to the project.inc file found in `maps_fps/example_maps_fps/` directory.

## CMake Build Instructions
From the maps_fps directory created in the steps above perform the following steps.
```
mkdir ../build-maps_fps-desktop
cd ../build-maps_fps-desktop
cmake ../maps_fps
make -j12

```
To run:
```
example_maps_fps/example_maps_fps

```

## QMake Build Instructions
To build and deploy the application using QtCreator perform the following steps.
1. Open the top level maps_fps.pro file in QtCreator.
2. Select a kit.
3. If appropriate set the number of jobs in Projects->Build->Build Steps->Make add something like -j12.
4. Click the green arrow in the bottom left to build and run.

# Modification

This project is designed to be modified to suit your purposes, the following files configure the OpenGL view and load the models.
```
example_maps_fps/src/LoadModels.cpp
example_maps_fps/src/main.cpp
```