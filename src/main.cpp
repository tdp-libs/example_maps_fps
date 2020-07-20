#include "example_maps_fps/Globals.h"
#include "example_maps_fps/LoadModels.h"

#include "tp_maps_sdl/Map.h"

#include "tp_maps/controllers/FPSController.h"
#include "tp_maps/layers/GridLayer.h"
#include "tp_maps/layers/FBOLayer.h"

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
  map.setBackgroundColor({0.2f, 0.2f, 0.2f});

  map.setRenderPasses({
                        tp_maps::RenderPass::LightFBOs,
                        tp_maps::RenderPass::ReflectionFBO,
                        tp_maps::RenderPass::Background,
                        tp_maps::RenderPass::Normal,
                        tp_maps::RenderPass::Transparency,
                        tp_maps::RenderPass::Reflection,
                        tp_maps::RenderPass::Text,
                        tp_maps::RenderPass::GUI
                      });

  auto controller = new tp_maps::FPSController(&map, fullScreen);
  controller->setCameraOrigin({4.0f, 4.0f, 2.0f});
  controller->setRotationAngle(270.0f);
  TP_UNUSED(controller);

  //map.addLayer(new tp_maps::GridLayer());
  map.addLayer(new tp_maps::FBOLayer(tp_maps::FBOLayerSource::LightDepth, 0));

  example_maps_fps::loadModels(&map);

  map.exec();
  return 0;
}
