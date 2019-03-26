#include "example_maps_fps/Globals.h"
#include "example_maps_fps/LoadModels.h"

#include "tp_maps_sdl/Map.h"

#include "tp_maps/controllers/FPSController.h"
#include "tp_maps/layers/GridLayer.h"

#include "tp_image_utils_freeimage/Globals.h"

#include "tp_utils_filesystem/Globals.h"

#include "tp_utils/DebugUtils.h"

//##################################################################################################
int main()
{
  tp_utils_filesystem::init();
  tp_image_utils_freeimage::init();

  bool fullScreen = true;

  tp_maps_sdl::Map map(true, fullScreen, "TP Maps FPS Example");
  map.setBackgroundColor({0.0f, 0.0f, 0.0f});

  auto controller = new tp_maps::FPSController(&map, fullScreen);
  TP_UNUSED(controller);

  map.addLayer(new tp_maps::GridLayer());

  example_maps_fps::loadModels(&map);

  map.exec();
  return 0;
}
