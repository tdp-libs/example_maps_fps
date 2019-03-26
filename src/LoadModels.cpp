#include "example_maps_fps/LoadModels.h"

#include "tp_maps/Map.h"
#include "tp_maps/textures/BasicTexture.h"
#include "tp_maps/layers/ImageLayer.h"
#include "tp_maps/layers/Geometry3DLayer.h"
#include "tp_maps/layers/PointsLayer.h"
#include "tp_maps/textures/DefaultSpritesTexture.h"

#include "tp_image_utils/LoadImages.h"

#include "tp_ply/ReadPLY.h"

#include "tp_math_utils/Geometry3D.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/DebugUtils.h"
#include "tp_utils/JSONUtils.h"
#include "tp_utils/FileUtils.h"

#include "glm/gtc/matrix_transform.hpp"

namespace example_maps_fps
{

namespace
{
//##################################################################################################
void loadImageLayers(tp_maps::Map* map, const std::string& path)
{
  auto images = tp_utils::listFiles(path, {"*.png"});
  for(const auto& image : images)
  {
    if(image.size()<5)
      continue;

    glm::vec3 topRight;
    glm::vec3 bottomRight;
    glm::vec3 bottomLeft;
    glm::vec3 topLeft;

    try
    {
      const auto& json = image.substr(0, image.size()-3) + "json";
      tpDebug() << json;
      auto j = tp_utils::readJSONFile(json);
      topRight    = tp_math_utils::getJSONVec3(j, "topRight",    {0.0f, 0.0f, 0.0f});
      bottomRight = tp_math_utils::getJSONVec3(j, "bottomRight", {0.0f, 0.0f, 0.0f});
      bottomLeft  = tp_math_utils::getJSONVec3(j, "bottomLeft",  {0.0f, 0.0f, 0.0f});
      topLeft     = tp_math_utils::getJSONVec3(j, "topLeft",     {0.0f, 0.0f, 0.0f});
    }
    catch(...)
    {
      continue;
    }

    auto colorMap = tp_image_utils::loadImage(image);

    tpDebug() << colorMap.width() << " " << colorMap.height();

    tp_maps::TextureData textureData;
    textureData.w = colorMap.width();
    textureData.h = colorMap.height();
    textureData.data = reinterpret_cast<TPPixel*>(colorMap.data());
    auto texture = new tp_maps::BasicTexture(map, textureData);
    texture->setMagFilterOption(GL_NEAREST);
    texture->setMinFilterOption(GL_NEAREST);

    auto layer = new tp_maps::ImageLayer(texture);
    layer->setImageCoords(topRight, bottomRight, bottomLeft, topLeft);
    map->addLayer(layer);
  }
}

//##################################################################################################
void loadGeometry3DLayers(tp_maps::Map* map, const std::string& path)
{
  auto images = tp_utils::listFiles(path, {"*.png"});
  for(const auto& image : images)
  {
    if(image.size()<5)
      continue;

    const auto& ply = image.substr(0, image.size()-3) + "ply";

    std::string error;
    std::vector<tp_maps::Geometry3D> geometry;
    geometry.emplace_back();
    tp_ply::readPLYFile(ply,
                        error,
                        GL_TRIANGLE_FAN,
                        GL_TRIANGLE_STRIP,
                        GL_TRIANGLES,
                        false,
                        geometry.front().geometry);

    geometry.front().material.alpha = 0.5f;

    auto colorMap = tp_image_utils::loadImage(image);

    tpDebug() << colorMap.width() << " " << colorMap.height();

    tp_maps::TextureData textureData;
    textureData.w = colorMap.width();
    textureData.h = colorMap.height();
    textureData.data = reinterpret_cast<TPPixel*>(colorMap.data());
    auto texture = new tp_maps::BasicTexture(map, textureData);
    texture->setMagFilterOption(GL_NEAREST);
    texture->setMinFilterOption(GL_NEAREST);

    auto layer = new tp_maps::Geometry3DLayer(texture);
    layer->setGeometry(geometry);
    layer->setShaderType(tp_maps::Geometry3DLayer::ShaderType::Image);
    map->addLayer(layer);
  }
}
}

//##################################################################################################
void loadModels(tp_maps::Map* map)
{  
  std::string imageLayersPath = "/home/tom/Desktop/wph_model/";
  std::string plyPath         = "/home/tom/Desktop/rolling_300_2/_3_1";


  constexpr bool useImageLayers = false;
  if(useImageLayers)
    loadImageLayers(map, imageLayersPath);
  else
    loadGeometry3DLayers(map, plyPath);

  {
    auto j = tp_utils::readJSONFile("/home/tom/Desktop/sfm/point_cloud/point_cloud.txt");
    std::vector<glm::vec3> pointCloud = tp_math_utils::vec3VectorFromJSON(j);

    std::vector<tp_maps::PointSpriteShader::PointSprite> points;
    points.resize(pointCloud.size());
    for(size_t p=0; p<pointCloud.size(); p++)
    {
      const auto& src = pointCloud.at(p);
      auto& dst = points.at(p);
      dst.position = src;
      dst.color = {1, 1, 1, 1};
      dst.radius = 1.0f;
    }

    auto spriteTexture = new tp_maps::SpriteTexture();
    spriteTexture->setTexture(new tp_maps::DefaultSpritesTexture(map));
    auto pointCloudLayer = new tp_maps::PointsLayer(spriteTexture);
    pointCloudLayer->setPoints(points);
    map->addLayer(pointCloudLayer);
  }
//  {
//    auto j = tp_utils::readJSONFile("/home/tom/Desktop/sfm/point_cloud/refined_point_cloud_old.txt");
//    std::vector<glm::vec3> pointCloud = tp_math_utils::vec3VectorFromJSON(j);

//    std::vector<tp_maps::PointSpriteShader::PointSprite> points;
//    points.resize(pointCloud.size());
//    for(size_t p=0; p<pointCloud.size(); p++)
//    {
//      const auto& src = pointCloud.at(p);
//      auto& dst = points.at(p);
//      dst.position = src;
//      dst.color = {1, 0, 0, 1};
//      dst.radius = 2.0f;
//    }

//    auto spriteTexture = new tp_maps::SpriteTexture();
//    spriteTexture->setTexture(new tp_maps::DefaultSpritesTexture(map));
//    auto pointCloudLayer = new tp_maps::PointsLayer(spriteTexture);
//    pointCloudLayer->setPoints(points);
//    map->addLayer(pointCloudLayer);
//  }

//  {
//    auto j = tp_utils::readJSONFile("/home/tom/Desktop/sfm/point_cloud/refined_point_cloud.txt");
//    std::vector<glm::vec3> pointCloud = tp_math_utils::vec3VectorFromJSON(j);

//    std::vector<tp_maps::PointSpriteShader::PointSprite> points;
//    points.resize(pointCloud.size());
//    for(size_t p=0; p<pointCloud.size(); p++)
//    {
//      const auto& src = pointCloud.at(p);
//      auto& dst = points.at(p);
//      dst.position = src;
//      dst.color = {0, 1, 0, 1};
//      dst.radius = 2.0f;
//    }

//    auto spriteTexture = new tp_maps::SpriteTexture();
//    spriteTexture->setTexture(new tp_maps::DefaultSpritesTexture(map));
//    auto pointCloudLayer = new tp_maps::PointsLayer(spriteTexture);
//    pointCloudLayer->setPoints(points);
//    map->addLayer(pointCloudLayer);
//  }
}

}
