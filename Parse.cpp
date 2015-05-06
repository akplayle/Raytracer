/*
 *  Parse.cpp
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "Parse.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>



using namespace std;


Vector3f setZero()
{
	Vector3f q;
	q << 0.0, 0.0, 0.0;
	return q;
}

Object* AddObj(char* filename, Scene& s, bool useAcc = true, Object::PrimitiveFactory *factory = new Object::TriangleFactory(), Vector3f pos = setZero())
{
	Object* obj = new Object(useAcc);
	obj->position = pos;
	obj->ParseObj(filename, factory);
	obj->BuildAccelStructure();
	s.AddObject(obj);
	return obj;
}

Object* AddSphere(Sphere* sphere, Scene& s, bool useAcc = true)
{
	Object *obj = new Object(useAcc);
	obj->Add(sphere);
	obj->BuildAccelStructure();
	
	s.AddObject(obj);
	return obj;
}

Object* AddVCloud(VCloud* cloud, Scene& s, bool useAcc = true)
{
	Object *obj = new Object(useAcc);
	obj->Add(cloud);
	obj->BuildAccelStructure();
	
	s.AddObject(obj);
	return obj;
}

Object* AddPlane(Plane* plane, Scene& s, bool useAcc = true)
{
	Object *obj = new Object(useAcc);
	obj->Add(plane);
	obj->BuildAccelStructure();
	
	s.AddObject(obj);
	return obj;
}

Object* AddEllipsoid(Ellipsoid* ellipsoid, Scene& s, bool useAcc = true)
{
	Object *obj = new Object(useAcc);
	obj->Add(ellipsoid);
	obj->BuildAccelStructure();
	
	s.AddObject(obj);
	return obj;
}

Object* AddCone(Cone* cone, Scene& s, bool useAcc = true)
{
	Object *obj = new Object(useAcc);
	obj->Add(cone);
	obj->BuildAccelStructure();
	
	s.AddObject(obj);
	return obj;
}

vector<Shader*> shaders;

int findShader(char *name)
{
	int index = NULL;
	for (int i = 0; i < shaders.size(); i++) {
		if (!strcmp(shaders[i]->name, name)) {
			index = i;
		}
	}
	return index;
}

Shader* getShader(char *name)
{
	int index = findShader(name);
    return shaders[index];
}


Object* findObject(char * name, Scene* scene)
{
    int index = NULL;
	for (int i = 0; i < scene->objects.size(); i++) {
		if (!strcmp(scene->objects[i]->name, name)) {
			index = i;
		}
	}
	return scene->objects[index];
}


void moveObject(char *name, Vector3f posOffset, float rotAngle, Scene* scene)
{
    rotAngle = rotAngle * PI / 180.f;
    
    Object* object = findObject(name, scene);
    float x = posOffset[0];
    float y = posOffset[1];
    float z = posOffset[2];
    object->position << x, y , z;
    
    object->Update();
}

Vector3f rotateVec(Vector3f vec, float rotAngle)
{
    rotAngle = rotAngle * PI / 180.f;
    return Vector3f((cos(rotAngle) * vec[0] + sin(rotAngle) * vec[2]), vec[1], (-sin(rotAngle)*vec[0] + cos(rotAngle)*vec[2]));
}


Scene* getScene(char filename[200])
{
	
	Scene * s = new Scene();
	char string[100];
	char inputFilename[200];
	
	strcpy(inputFilename, filename);
	ifstream inputFile(inputFilename, ios::in);
	
	
	//--------------------------------------------
	// if not a valid file
	if (!inputFile)
	{
		cerr << "ERROR opening file: " << inputFilename << endl;
		return s;
	}
	//--------------------------------------------
	// process scene file
	else {
		cout << "Opened Scene Description File Successfully!\n";
		
		
		
		//--------------------------
		//find the line that contains the background
		inputFile >> string;
		
		while (strcmp(string, ":background")) {
			inputFile >> string;
		}
		cout << "Processing Background ... \n";
		while (inputFile >> string && strcmp(string, ":camera")) {
			// color
			if (!(strcmp(string, "color"))){
				char fileName[100];
				inputFile >> fileName;
				if (isdigit(fileName[0])) {
					s->background.r = atof(fileName);
					inputFile >> s->background.g;
					inputFile >> s->background.b;
				}
				else {
					Environment* e = new Environment(s);
					Texture* tex = new Texture2D(fileName);
					tex->name = "env";
					tex->setWrapMode(Texture::REPEAT);
					tex->setInterpolationMode(Texture::LINEAR);
					e->addTexture(tex);
					s->env = e;
				}
				
			}
		}
		
		//--------------------------
		//find the line that contains the camera
		while (strcmp(string, ":camera")) {
			inputFile >> string;
		}
		cout << "Processing Camera ... \n";
        s->camera = new PaintingCamera();
        
		while (inputFile >> string && strcmp(string, ":shaders")) {
			
			// position
			if(!(strcmp(string, "position")))
			{
				inputFile >> s->camera->position[0];
				inputFile >> s->camera->position[1];
				inputFile >> s->camera->position[2];
			}
			
			// view vector
			if(!(strcmp(string, "view")))
			{
				inputFile >> s->camera->viewV[0];
				inputFile >> s->camera->viewV[1];
				inputFile >> s->camera->viewV[2];
			}
			
			// up vector
			if(!(strcmp(string, "up")))
			{
				inputFile >> s->camera->upV[0];
				inputFile >> s->camera->upV[1];
				inputFile >> s->camera->upV[2];
			}
			
			// width
			if(!(strcmp(string, "width")))
			{
				inputFile >> s->camera->width;
			}
			
			//height
			if(!(strcmp(string, "height")))
			{
				inputFile >> s->camera->height;
			}
            
            
            //Input Image for Painting Camera
            if (!(strcmp(string, "image"))){
                char fileName[100];
                inputFile >> fileName;
                Texture* tex = new Normal(fileName);
                tex->name = "image";
                tex->setWrapMode(Texture::REPEAT);
                tex->setInterpolationMode(Texture::LINEAR);
                s->camera->inputImage = tex;
                
            }
			
		}
		cout << "Done Processing Camera\n";
		//--------------------------
		//find the line that contains the objects
		cout << "Processing Shaders ... \n";
		while (strcmp(string, ":shaders")) {
			inputFile >> string;
		}
		while (inputFile >> string && strcmp(string,":objects")) {
			//new object
			if (!strcmp(string, "begin")) {
				
				// create object type
				inputFile >> string;
				
				
				//============================================
				//					 FLAT
				//============================================
				
				if (!(strcmp(string, "flat"))) {
					FlatShader* temp = new FlatShader(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				//============================================
				//					 PLASMA
				//============================================
				
				if (!(strcmp(string, "plasma"))) {
					Plasma* temp = new Plasma(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				//============================================
				//					 MIRROR
				//============================================
				
				if (!(strcmp(string, "mirror"))) {
					Mirror* temp = new Mirror(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->c.r = atof(fileName);
								inputFile >> temp->c.g;
								inputFile >> temp->c.b;
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "diff";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
                            
						}
						
                        //ks
						if (!(strcmp(string, "ks")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->ks = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "ks";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
                        //glossy
						if (!(strcmp(string, "glossy")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->glossy = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "glossy";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
						
						//thickness
						if (!(strcmp(string, "ki")))
						{
                            
							char fileName[100];
							inputFile >> fileName;
                            cout << filename << endl;
                            
							if (isdigit(fileName[0])) {
								temp->ki = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "iri";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
						
						if (!(strcmp(string, "normal"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Normal(fileName);
							tex->name = "normal";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
                        
                        // bumpmap
						if (!(strcmp(string, "bump"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Bump(fileName);
							tex->name = "bump";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
                
                //============================================
				//					 GLASS
				//============================================
				
				if (!(strcmp(string, "glass"))) {
					Glass* temp = new Glass(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->c.r = atof(fileName);
								inputFile >> temp->c.g;
								inputFile >> temp->c.b;
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "diff";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
                            
						}
						
                        //ks
						if (!(strcmp(string, "ks")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->ks = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "ks";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
                        //glossy
						if (!(strcmp(string, "glossy")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->glossy = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "glossy";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
                        //transclucence
						if (!(strcmp(string, "translucent")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->translucence = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "translucent";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
                        //eta_i
						if (!(strcmp(string, "eta_i")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->eta_i = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "eta_i";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
                        //eta_t
						if (!(strcmp(string, "eta_t")))
						{
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->eta_t = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "eta_t";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
                        
						
						//thickness
						if (!(strcmp(string, "ki")))
						{
                            
							char fileName[100];
							inputFile >> fileName;
                            cout << filename << endl;
                            
							if (isdigit(fileName[0])) {
								temp->ki = atof(fileName);
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "iri";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
						}
						
						if (!(strcmp(string, "normal"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Normal(fileName);
							tex->name = "normal";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
                        
                        // bumpmap
						if (!(strcmp(string, "bump"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Bump(fileName);
							tex->name = "bump";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
                
				//============================================
				//					 CLOUD
				//============================================
				
				if (!(strcmp(string, "cloud"))) {
					Cloud* temp = new Cloud(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					LAMBERT
				//============================================
				
				if (!(strcmp(string, "lambert"))) {
					Lambert* temp = new Lambert(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->c.r = atof(fileName);
								inputFile >> temp->c.g;
								inputFile >> temp->c.b;
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "diff";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								//temp->image = tex;
								temp->addTexture(tex);
							}
;
														
						}
						
						// bumpmap
						if (!(strcmp(string, "bump"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Bump(fileName);
							tex->name = "bump";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						if (!(strcmp(string, "normal"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Normal(fileName);
							tex->name = "normal";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						// ambient
						if (!(strcmp(string, "ka"))){
							inputFile >> temp->ka;
						}
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					PHONG
				//============================================
				
				if (!(strcmp(string, "phong"))) {
					Phong* temp = new Phong(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->c.r = atof(fileName);
								inputFile >> temp->c.g;
								inputFile >> temp->c.b;
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "diff";
								tex->setWrapMode(Texture::REPEAT);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->addTexture(tex);
							}
							
						}
						
						// bump
						if (!(strcmp(string, "bump"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Bump(fileName);
							tex->name = "bump";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						if (!(strcmp(string, "normal"))){
							char fileName[100];
							inputFile >> fileName;
							Texture* tex = new Normal(fileName);
							tex->name = "normal";
							tex->setWrapMode(Texture::REPEAT);
							tex->setInterpolationMode(Texture::LINEAR);
							temp->addTexture(tex);
							
						}
						
						// specular shape
						if (!(strcmp(string, "crisp"))){
							inputFile >> temp->crisp;
						}
						
						// ambient
						if (!(strcmp(string, "ka"))){
							inputFile >> temp->ka;
						}
						
						// specular
						if (!(strcmp(string, "ke"))){
							inputFile >> temp->ke;
						}
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					GOOCH
				//============================================
				
				if (!(strcmp(string, "gooch"))) {
					Gooch* temp = new Gooch(s);
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// color1
						if (!(strcmp(string, "color1"))){
							inputFile >> temp->c1.r;
							inputFile >> temp->c1.g;
							inputFile >> temp->c1.b;
						}
						
						// color2
						if (!(strcmp(string, "color2"))){
							inputFile >> temp->c2.r;
							inputFile >> temp->c2.g;
							inputFile >> temp->c2.b;
						}
						
						// specular shape
						if (!(strcmp(string, "crisp"))){
							inputFile >> temp->crisp;
						}
						
						// specular
						if (!(strcmp(string, "ke"))){
							inputFile >> temp->ke;
						}
						
						
					}
					shaders.push_back(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
			}
		}
		cout << "Done Processing Shaders\n";
		while (strcmp(string, ":objects")) {
			inputFile >> string;
			cout << string << endl;
		}
		cout << "Processing Objects ... \n";
		while (inputFile >> string && strcmp(string, ":lights")) {
			//new object
			if (!strcmp(string, "begin")) {
				s->numberOfObjects++;
				
				// create object type
				inputFile >> string;
				
				
				//============================================
				//					SPHERE
				//============================================
				
				if (!(strcmp(string, "sphere"))) {
					Sphere* temp = new Sphere();
					int shaderIndex;
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp->name;
						}
						
						// raius
						if (!(strcmp(string, "radius"))){
							inputFile >> temp->radius;
						}
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					
					Object* sphere = AddSphere(temp, *s, false);
					sphere->setShader(shaders[shaderIndex]);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					OBJ
				//============================================
				
				if (!(strcmp(string, "obj"))) {
					char file[100];
					int shaderIndex;
					Object::PrimitiveFactory *factory;
					Vector3f position;
					char name[30];
					
                    strcpy(name, "");
                    
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> position[0];
							inputFile >> position[1];
							inputFile >> position[2];
						}
						
                        // name
						if (!(strcmp(string, "name"))) {
							inputFile >> name;
						}
						
						// fileName
						if(!(strcmp(string, "file")))
						{
							inputFile >> file;
						}
						
						// factory
						if (!(strcmp(string, "factory"))) {
							int f = 0;
							inputFile >> f;
							if (f == 1) {
								factory = new Object::SmoothTriangleFactory();
							}
							else if (f == 2) {
								factory = new Object::TexturedSmoothTriangleFactory();
							}
							else {
								factory = new Object::TriangleFactory();
							}

						}
						
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					Object* obj = AddObj(file, *s, true, factory, position);
                    strcpy(obj->name, name);
					obj->setShader(shaders[shaderIndex]);

					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					VCLOUD
				//============================================
				
				if (!(strcmp(string, "vcloud"))) {
					VCloud* temp = new VCloud();
					int shaderIndex;
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp->name;
						}
						
						// raius
						if (!(strcmp(string, "radius"))){
							inputFile >> temp->radius;
						}
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					
					Object* cloud = AddVCloud(temp, *s, false);
					cloud->setShader(shaders[shaderIndex]);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//					PLANE
				//============================================
				
				if (!(strcmp(string, "plane"))) {
					Plane* temp = new Plane();
					int shaderIndex;
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->origin[0];
							inputFile >> temp->origin[1];
							inputFile >> temp->origin[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp->name;
						}
						
						// raius
						if (!(strcmp(string, "normal"))){
							inputFile >> temp->normal[0];
							inputFile >> temp->normal[1];
							inputFile >> temp->normal[2];
						}
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					
					Object* plane = AddPlane(temp, *s, false);
					plane->setShader(shaders[shaderIndex]);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				
				//============================================
				//				   ELLIPSOID
				//============================================
				
				if (!(strcmp(string, "ellipsoid"))) {
					Ellipsoid* temp = new Ellipsoid();
					int shaderIndex;
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp->name;
						}
						
						// s0
						if (!(strcmp(string, "s0"))){
							inputFile >> temp->s0;
						}
						
						// s1
						if (!(strcmp(string, "s1"))){
							inputFile >> temp->s1;
						}
						
						// s2
						if (!(strcmp(string, "s2"))){
							inputFile >> temp->s2;
						}
						
						// n0
						if(!(strcmp(string, "n0")))
						{
							inputFile >> temp->n0[0];
							inputFile >> temp->n0[1];
							inputFile >> temp->n0[2];
						}
						
						// n1
						if(!(strcmp(string, "n1")))
						{
							inputFile >> temp->n1[0];
							inputFile >> temp->n1[1];
							inputFile >> temp->n1[2];
						}
						
						// n2
						if(!(strcmp(string, "n2")))
						{
							inputFile >> temp->n2[0];
							inputFile >> temp->n2[1];
							inputFile >> temp->n2[2];
						}
						
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					
					Object* sphere = AddEllipsoid(temp, *s, false);
					sphere->setShader(shaders[shaderIndex]);
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				//============================================
				//				   CONE
				//============================================
				
				if (!(strcmp(string, "cone"))) {
					Cone* temp = new Cone();
					int shaderIndex;
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp->name;
						}
						
						// s0
						if (!(strcmp(string, "s0"))){
							inputFile >> temp->s0;
						}
						
						// s1
						if (!(strcmp(string, "s1"))){
							inputFile >> temp->s1;
						}
						
						// s2
						if (!(strcmp(string, "s2"))){
							inputFile >> temp->s2;
						}
						
						// n0
						if(!(strcmp(string, "n0")))
						{
							inputFile >> temp->n0[0];
							inputFile >> temp->n0[1];
							inputFile >> temp->n0[2];
						}
						
						// n1
						if(!(strcmp(string, "n1")))
						{
							inputFile >> temp->n1[0];
							inputFile >> temp->n1[1];
							inputFile >> temp->n1[2];
						}
						
						// n2
						if(!(strcmp(string, "n2")))
						{
							inputFile >> temp->n2[0];
							inputFile >> temp->n2[1];
							inputFile >> temp->n2[2];
						}
						
						
						// color
						if (!(strcmp(string, "shader"))){
							char tmpShaderName[30];
							inputFile >> tmpShaderName;
							shaderIndex = findShader(tmpShaderName);
						}
						
					}
					
					Object* sphere = AddCone(temp, *s, false);
					sphere->setShader(shaders[shaderIndex]);
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
			}
		}
		cout << "Done Processing Objects\n";
		//----------------------------------------
		// Process Lights
		//--------------------------
		//find the line that contains the objects
		while (strcmp(string, ":lights")) {
			inputFile >> string;
		}
		cout << "Processing Lights ... \n";
		while (inputFile >> string && (strcmp(string, ":end"))) {
			//new object
			if (!strcmp(string, "begin")) {
				s->numberOfLights++;
				
				// create object type
				inputFile >> string;
				
				
				//============================================
				//				 POINT LIGHT
				//============================================
				
				if (!(strcmp(string, "pointLight"))) {
					PointLight* temp = new PointLight();
					
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// raius
						if (!(strcmp(string, "brightness"))){
							inputFile >> temp->brightness;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					
					s->AddLight(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				//============================================
				//				  AREA LIGHT
				//============================================
				
				if (!(strcmp(string, "areaLight"))) {
					AreaLight* temp = new AreaLight();
					
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// normal
						if(!(strcmp(string, "normal")))
						{
							inputFile >> temp->normal[0];
							inputFile >> temp->normal[1];
							inputFile >> temp->normal[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// brightness
						if (!(strcmp(string, "brightness"))){
							inputFile >> temp->brightness;
						}
						
						// width
						if (!(strcmp(string, "width"))){
							inputFile >> temp->w;
						}
						
						// height
						if (!(strcmp(string, "height"))){
							inputFile >> temp->h;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					s->AddLight(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				
				
				//============================================
				//				DIRECTIONAL LIGHT
				//============================================
				
				if (!(strcmp(string, "directionalLight"))) {
					DirectionalLight* temp = new DirectionalLight();
					
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// normal
						if(!(strcmp(string, "normal")))
						{
							inputFile >> temp->normal[0];
							inputFile >> temp->normal[1];
							inputFile >> temp->normal[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// brightness
						if (!(strcmp(string, "brightness"))){
							inputFile >> temp->brightness;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					s->AddLight(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//				  SPOT LIGHT
				//============================================
				
				if (!(strcmp(string, "spotLight"))) {
					SpotLight* temp = new SpotLight();
					
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// position
						if(!(strcmp(string, "position")))
						{
							inputFile >> temp->position[0];
							inputFile >> temp->position[1];
							inputFile >> temp->position[2];
						}
						
						// normal
						if(!(strcmp(string, "normal")))
						{
							inputFile >> temp->normal[0];
							inputFile >> temp->normal[1];
							inputFile >> temp->normal[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// brightness
						if (!(strcmp(string, "brightness"))){
							inputFile >> temp->brightness;
						}
						
						// width
						if (!(strcmp(string, "coneAngle"))){
							inputFile >> temp->coneAngle;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							inputFile >> temp->c.r;
							inputFile >> temp->c.g;
							inputFile >> temp->c.b;
						}
						
					}
					s->AddLight(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
				
				//============================================
				//				PROJECTION LIGHT
				//============================================
				
				if (!(strcmp(string, "projection"))) {
					Projection* temp = new Projection();
					
					while ((inputFile >> string) && (strcmp(string, "end"))) {
						
						// normal
						if(!(strcmp(string, "normal")))
						{
							inputFile >> temp->normal[0];
							inputFile >> temp->normal[1];
							inputFile >> temp->normal[2];
						}
						
						// name
						if (!(strcmp(string, "name"))) {
							inputFile >> temp ->name;
						}
						
						// brightness
						if (!(strcmp(string, "brightness"))){
							inputFile >> temp->brightness;
						}
						
						// color
						if (!(strcmp(string, "color"))){
							char fileName[100];
							inputFile >> fileName;
							if (isdigit(fileName[0])) {
								temp->c.r = atof(fileName);
								inputFile >> temp->c.g;
								inputFile >> temp->c.b;
							}
							else {
								Texture* tex = new Texture2D(fileName);
								tex->name = "image";
								tex->setWrapMode(Texture::CLAMP);
								tex->setInterpolationMode(Texture::LINEAR);
								temp->image = tex;
							}
							
						}
					}
					s->AddLight(temp);
					
					if (!(strcmp(string, "end"))) {
						continue;
					}
					
				}
				
			}
		}
		cout << "Done Processing Lights\n";
		cout << "Done reading file\n";
		
	}
	return s;
}
