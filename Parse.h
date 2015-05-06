/*
 *  Parse.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

//int getScene(char filename[200]);
//int getNumberOfObjects();
//int splitString(char *, char *, char *);

#include "Scene.h"
#include "Image.h"

//include Cameras
#include "PerspectiveCamera.h"
#include "DepthOfFieldCamera.h"
#include "PaintingCamera.h"

//include textures
#include "Texture2D.h"
#include "Bump.h"
#include "Normal.h"

//include shaders
#include "FlatShader.h"
#include "Environment.h"
#include "Phong.h"
#include "Lambert.h"
#include "Gooch.h"
#include "Plasma.h"
#include "Cloud.h"
#include "Mirror.h"
#include "Glass.h"

//include objects 
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Ellipsoid.h"
#include "Cone.h"
#include "VCloud.h"

//include lights 
#include "PointLight.h"
#include "AreaLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Projection.h"

int findShader(char *name);
Shader* getShader(char *name);
Scene* getScene(char filename[200]);

void moveObject(char *name, Vector3f pos, float rotAngle, Scene* scene);
Vector3f rotateVec(Vector3f vec, float rotAngle);
