#ifndef example_maps_fps_LoadModels_h
#define example_maps_fps_LoadModels_h

#include "example_maps_fps/Globals.h"

#include "glm/glm.hpp"

namespace tp_maps
{
class Map;
class ImageLayer;
}

namespace example_maps_fps
{

//##################################################################################################
void loadModels(tp_maps::Map* map);

//##################################################################################################
void loadPLYFromResource(tp_maps::Map* map, const std::string& resourceName);

//##################################################################################################
void loadPointCloudFromFile(tp_maps::Map* map, const std::string& filePath, const glm::vec4& color, float radius);

//##################################################################################################
void loadImageLayers(tp_maps::Map* map, const std::string& path);

//##################################################################################################
void loadGeometry3DLayers(tp_maps::Map* map, const std::string& path);

}

#endif
