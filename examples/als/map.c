#ifndef _K_BASIC_MAP_
#define _K_BASIC_MAP_

#include <raylib.h>
#include <stdlib.h>

#include "kutils.c"

typedef struct {
  Model model;
  Model water;

  Shader shader;

  Vector3 meshScale;
  Image image;
  Texture heightmap;
} Map;

Map CreateMap() {
  Map map;

  Vector2 noiseSize = {100, 100};
  Vector2 noiseOffset = {50, 50};

  map.image = LoadImage("resources/textures/baseMap.png");
  map.heightmap = LoadTextureFromImage(map.image); 

  map.meshScale = (Vector3){ 200, 10, 200 };
  Mesh mesh = GenMeshHeightmap(map.image, map.meshScale); // Generate heightmap mesh (RAM and VRAM)
  map.model = LoadModelFromMesh(mesh);                  // Load model from generated mesh

  Texture2D texture = LoadTexture("/home/kirandeep/Documents/My Programme/Learn-RayLib/Assets/Textures/Prototype/PNG/Purple/texture_05.png");
  map.shader = LoadShader("resources/shaders/map.vs.glsl", "resources/shaders/map.fs.glsl");

  for (int i = 0; i < map.model.materialCount; i++) {
    map.model.materials[i].shader = map.shader;
    SetMaterialTexture(&map.model.materials[0], MATERIAL_MAP_DIFFUSE, texture);
  }

  Mesh waterMesh = GenMeshPlane(map.meshScale.x, map.meshScale.z, 1, 1);
  map.water = LoadModelFromMesh(waterMesh);

  Texture2D waterTexture = LoadTexture("/home/kirandeep/Documents/My Programme/Learn-RayLib/Assets/Textures/Prototype/PNG/Light/texture_05.png");
  for (int i = 0; i < map.water.materialCount; i++) {
    map.water.materials[i].shader = map.shader;
    SetMaterialTexture(&map.water.materials[0], MATERIAL_MAP_DIFFUSE, waterTexture);
  }

  return map;
}

float MapGetAt(Map map, float x, float z) {
  float _x = x * ((float)map.image.width)  / ((float)map.meshScale.x);
  float _y = z * ((float)map.image.height) / ((float)map.meshScale.z);

  _x = clampFloat(_x, 0, map.image.width);
  _y = clampFloat(_y, 0, map.image.height);

  float _rem_y = _y - (int)_y;
  float _rem_x = _x - (int)_x;

  float value = bilinear_interpolation(_rem_x, _rem_y, 
    GetImageColor(map.image, _x    , _y    ).r * ((float) map.meshScale.y) / 255.0f,
    GetImageColor(map.image, _x    , _y + 1).r * ((float) map.meshScale.y) / 255.0f,
    GetImageColor(map.image, _x + 1, _y    ).r * ((float) map.meshScale.y) / 255.0f,
    GetImageColor(map.image, _x + 1, _y + 1).r * ((float) map.meshScale.y) / 255.0f);

 return value;
  // return GetImageColor(map->image, _x    , _y    ).r * ((float) map->meshScale.y) / 255.0f;
}

void DrawMap(Map map) {
  DrawModel(map.model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
  DrawModel(map.water, (Vector3){100.0f, 0.1f, 100.0f}, 1.0f, BLUE);
}

void UnloadMap(Map map) {
  UnloadModel(map.model);
  UnloadTexture(map.heightmap);
  UnloadImage(map.image);
}

#endif
