#include "example_maps_fps/LoadModels.h"

#include "tp_maps/Map.h"
#include "tp_maps/layers/ImageLayer.h"
#include "tp_maps/layers/Geometry3DLayer.h"
#include "tp_maps/layers/PointsLayer.h"
#include "tp_maps/layers/LinesLayer.h"
#include "tp_maps/textures/DefaultSpritesTexture.h"

#include "tp_image_utils/LoadImages.h"

#include "tp_ply/ReadPLY.h"

#include "tp_obj/ReadOBJ.h"

#include "tp_math_utils/Geometry3D.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/DebugUtils.h"
#include "tp_utils/FileUtils.h"
#include "tp_utils/Resources.h"

#include "glm/gtc/matrix_transform.hpp"

#define DRAW_NORMALS 0

namespace example_maps_fps
{

//##################################################################################################
void loadModels(tp_maps::Map* map)
{
  //The following functions are just examples of how to load models and data from files and resource
  //files. Modify the functions to suit your needs.

  //Loads a model of a bunny from a resource file.
  //loadPLYFromResource(map, "/example_maps_fps/bunny.ply");

  //Load a point cloud from file.
  //loadPointCloudFromFile(map, "path/to/point_cloud.json", {1, 0, 0, 1}, 2.0f);

  //Load multiple image layers from a directory.
  //loadImageLayers(map, "path/to/directory/containing/image/layers/");

  //Load multiple geometry layers from a directory.
  //loadGeometry3DLayers(map, "path/to/directory/containing/ply/files/");

  //loadOBJFromFile(map, "/home/tom/Downloads/old_barrel/old_barrel_final.obj", 1.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/CashRegister/Register.obj", 1.0f);

  //loadOBJFromFile(map, "/home/tom/Downloads/Penguin/PenguinBaseMesh.obj", 1.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/HangingLight/HangingLight.obj", 50.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/Futuristic_Transport_Shuttle_Rigged/Transport Shuttle_obj.obj", 1.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/Futuristic_Car_Game-Ready/obj/Futuristic_Car_2.1.obj", 1.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/Fireplace/Obj/fireplace.obj", 0.1f);
  //loadOBJFromFile(map, "/home/tom/Downloads/Cobblestones3/Files/untitled.obj", 1.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/MONARCH/monarch.obj", 50.0f);
  //loadOBJFromFile(map, "/home/tom/Downloads/crocodile/CROCODIL.obj", 1.0f);

  //loadOBJFromFile(map, "/Users/tom/scene/anim_pack_19_F1.obj", 1.0f);
  loadOBJFromFile(map, "/Users/tom/scene/anim_pack_19_F2.obj", 1.0f);
}

//##################################################################################################
void loadPLYFromResource(tp_maps::Map* map, const std::string& resourceName)
{
  //Get the model data from the resource file. These resources are built into the application
  //binary, see:
  //example_maps_fps/src/example_maps_fps.qrc
  //And the following line:
  //TP_RC += src/example_maps_fps.qrc
  //In:
  //example_maps_fps/vars.pri
  auto resource = tp_utils::resource(resourceName);
  tp_utils::ResourceStream resourceStream(resource);

  //Read the contents of the PLY file into a tp_maps::Geometry3D object.
  std::string error;
  std::vector<tp_maps::Geometry3D> geometry;
  auto& object = geometry.emplace_back();
  tp_ply::readPLYStream(resourceStream,
                        error,
                        GL_TRIANGLE_FAN,
                        GL_TRIANGLE_STRIP,
                        GL_TRIANGLES,
                        true,
                        object.geometry);

  object.geometry.calculateFaceNormals();
  //object.geometry.calculateVertexNormals();
  object.material.alpha = 1.0f;

  //Display the object in the map.
  auto layer = new tp_maps::Geometry3DLayer();
  layer->setGeometry(geometry);
  layer->setShaderType(tp_maps::Geometry3DLayer::ShaderType::Material);
  map->addLayer(layer);

  //Position rotate and scale the object.
  glm::mat4 m(1.0f);
  m = glm::translate(m, {1.0f, 1.0f, -0.30f});
  m = glm::scale(m, {10.0f, 10.0f, 10.0f});
  m = glm::rotate(m, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
  layer->setObjectMatrix(m);

#if DRAW_NORMALS
  auto linesLayer = new tp_maps::LinesLayer();
  linesLayer->setLinesFromGeometryNormals(geometry, 0.0004f);
  linesLayer->setObjectMatrix(m);
  map->addLayer(linesLayer);
#endif
}

//##################################################################################################
void loadPointCloudFromFile(tp_maps::Map* map, const std::string& filePath, const glm::vec4& color, float radius)
{
  //Read in and parse the point cloud JSON data. The format is simple, an array of points, each
  //point is an array of 3 floats representing x,y,z. For example:
  // [[1.0,0.0,0.0],[-1.0,1.0,0.0],[-6.0,1.0,3.0]]
  auto j = tp_utils::readJSONFile(filePath);
  std::vector<glm::vec3> pointCloud = tp_math_utils::vec3VectorFromJSON(j);

  //Style the points and prepare them for rendering.
  std::vector<tp_maps::PointSpriteShader::PointSprite> points;
  points.resize(pointCloud.size());
  for(size_t p=0; p<pointCloud.size(); p++)
  {
    const auto& src = pointCloud.at(p);
    auto& dst = points.at(p);
    dst.position = src;
    dst.color = color;
    dst.radius = radius;
  }

  //Select a texture to draw the points with and add them to the map.
  auto spriteTexture = new tp_maps::SpriteTexture();
  spriteTexture->setTexture(new tp_maps::DefaultSpritesTexture(map));
  auto pointCloudLayer = new tp_maps::PointsLayer(spriteTexture);
  pointCloudLayer->setPoints(points);
  map->addLayer(pointCloudLayer);
}

//##################################################################################################
void loadImageLayers(tp_maps::Map* map, const std::string& path)
{
  //This function searches a directory for all .png files and corresponding .json files. The json
  //file needs to define four 3D coordinates, one for each corner of the image.

  //Iterate over all .png's in the directory.
  auto images = tp_utils::listFiles(path, {"*.png"});
  for(const auto& image : images)
  {
    if(image.size()<5)
      continue;

    //Read the 3D coords.
    glm::vec3 topRight;
    glm::vec3 bottomRight;
    glm::vec3 bottomLeft;
    glm::vec3 topLeft;

    try
    {
      const auto& json = image.substr(0, image.size()-3) + "json";
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

    //Load the texture.
    auto colorMap = tp_image_utils::loadImage(image);
    tpWarning() << colorMap.width() << " " << colorMap.height();

    //Prepare the texture for rendering in OpenGL.
    tp_maps::TextureData textureData;
    textureData.w = colorMap.width();
    textureData.h = colorMap.height();
    textureData.data = colorMap.data();
    auto texture = new tp_maps::BasicTexture(map, textureData);
    texture->setMagFilterOption(GL_NEAREST);
    texture->setMinFilterOption(GL_NEAREST);

    //Add the image layer to the map.
    auto layer = new tp_maps::ImageLayer(texture);
    layer->setImageCoords(topRight, bottomRight, bottomLeft, topLeft);
    map->addLayer(layer);
  }
}

//##################################################################################################
void loadGeometry3DLayers(tp_maps::Map* map, const std::string& path)
{
  //This function searches a directory for all .png files and corresponding .ply files. For each
  //pair it creates a geometry layer and adds it to the map.

  //Iterate over all .png's in the directory.
  auto images = tp_utils::listFiles(path, {"*.png"});
  for(const auto& image : images)
  {
    if(image.size()<5)
      continue;

    //Load the model geometry and texture coords from the ply file.
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
    geometry.front().material.ambientTexture = "Image";

    //Load a texture to texture the model with.
    auto colorMap = tp_image_utils::loadImage(image);

#if 0
    colorMap = colorMap.flipped();
#endif

    tpWarning() << colorMap.width() << " " << colorMap.height();

    tp_maps::TextureData textureData;
    textureData.w = colorMap.width();
    textureData.h = colorMap.height();
    textureData.data = reinterpret_cast<TPPixel*>(colorMap.data());
    auto texture = new tp_maps::BasicTexture(map, textureData);
    texture->setMagFilterOption(GL_NEAREST);
    texture->setMinFilterOption(GL_NEAREST);

    //Add the model to the map.
    auto layer = new tp_maps::Geometry3DLayer();
    layer->setTextures({{"Image", texture}});
    layer->setGeometry(geometry);
    layer->setShaderType(tp_maps::Geometry3DLayer::ShaderType::Image);
    map->addLayer(layer);
  }
}


//##################################################################################################
void loadOBJFromFile(tp_maps::Map* map, const std::string& path, float scale)
{
  //Read the contents of the OBJ file into a tp_maps::Geometry3D object.
  std::string error;
  std::vector<tp_maps::Geometry3D> geometry;

  tp_obj::readOBJFile(path,
                      error,
                      GL_TRIANGLE_FAN,
                      GL_TRIANGLE_STRIP,
                      GL_TRIANGLES,
                      true,
                      geometry);

  std::unordered_map<tp_utils::StringID, std::string> texturePaths;
  for(auto& g : geometry)
  {
    // Hack to move the Kiwi
    if(tpContains(g.geometry.comments, "AM130_015_005_Mesh.013") || tpContains(g.geometry.comments, "AM130_015_003_Mesh.014"))
    {
      for(auto& vert : g.geometry.verts)
      {
        vert.vert.x +=  0.2f;
        vert.vert.z += -1.0f;
      }
    }

    //g.geometry.calculateVertexNormals();
    g.geometry.calculateTangentsAndBitangents();

    auto addTexture = [&](const tp_utils::StringID& name)
    {
      if(!name.isValid())
        return;

      if(tpContainsKey(texturePaths, name))
        return;

      texturePaths[name] = tp_obj::getAssociatedFilePath(path, name.keyString());
    };

    addTexture(g.material.ambientTexture);
    addTexture(g.material.diffuseTexture);
    addTexture(g.material.specularTexture);
    addTexture(g.material.alphaTexture);
    addTexture(g.material.bumpTexture);
  }

  std::unordered_map<tp_utils::StringID, tp_maps::Texture*> textures;
  for(const auto& i : texturePaths)
  {
    auto colorMap = tp_image_utils::loadImage(i.second);

    //colorMap = colorMap.flipped();
    tpWarning() << "Loading texture: " << i.second << " w:" << colorMap.width() << " h:" << colorMap.height();

    tp_maps::TextureData textureData;
    textureData.w = colorMap.width();
    textureData.h = colorMap.height();
    textureData.data = reinterpret_cast<TPPixel*>(colorMap.data());
    auto texture = new tp_maps::BasicTexture(map, textureData);
    texture->setMagFilterOption(GL_LINEAR);
    texture->setMinFilterOption(GL_LINEAR);
    texture->setTextureWrapS(GL_REPEAT);
    texture->setTextureWrapT(GL_REPEAT);

    textures[i.first] = texture;
  }

  //Display the object in the map.
  auto layer = new tp_maps::Geometry3DLayer();
  layer->setGeometry(geometry);
  layer->setTextures(textures);
  layer->setShaderType(tp_maps::Geometry3DLayer::ShaderType::Material);
  map->addLayer(layer);

  glm::mat4 m(1.0f);
  m = glm::translate(m, {1.0f, 1.0f, -0.30f});
  m = glm::scale(m, {scale, scale, scale});
  m = glm::rotate(m, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
  layer->setObjectMatrix(m);

#if DRAW_NORMALS
  auto linesLayer = new tp_maps::LinesLayer();
  linesLayer->setLinesFromGeometryNormals(geometry, 0.01f);
  linesLayer->setObjectMatrix(m);
  map->addLayer(linesLayer);
#endif

  //Position rotate and scale the object.
//  {
//    float angle=90.0;
//    double prevTimestampMS=-1.0;
//    layer->animateCallbacks.addCallback([=](double timestampMS) mutable
//    {
//      if(prevTimestampMS<0.0)
//        prevTimestampMS = timestampMS;

//      glm::mat4 m(1.0f);
//      m = glm::translate(m, {1.0f, 1.0f, -0.30f});
//      m = glm::scale(m, {scale, scale, scale});
//      m = glm::rotate(m, glm::radians(float(angle)), {1.0f, 0.0f, 0.0f});
//      layer->setObjectMatrix(m);
//      float degPerSecond=5;
//      angle += (degPerSecond/1000)*float(timestampMS-prevTimestampMS);
//      if(angle>=360.0f)
//        angle=0.0f;

//      prevTimestampMS = timestampMS;
//    });
//  }


}

}
