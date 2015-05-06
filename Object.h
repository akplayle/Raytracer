/*
 *  Object.h
 *  Raytracer
 *
 *  Created by Amber on 2/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef OBJECT_H
#define OBJECT_H

#include <fstream>
#include <iomanip>
#include <iostream>

#include "KDTree.h"
#include "Shader.h"
#include "Triangle.h"

class Object
{
	vector<Primitive *> primitive;
private:
	
	KDTree* kdtree;
	
	bool m_useAcc;
	
	
	
	color c;
	
	bool _visible;
    
public:
	void Add(Primitive *p)
	{
        p->position = p->position + position;
        
        if (!strcmp(p->name, "")) {
            strcpy(p->name, name);
        }
        else
        {
            strcpy(name, p->name);
        }
		
        primitive.push_back(p);
	};
    
    
	

	Vector3f position;
	char name[30];
    
	class PrimitiveFactory 
	{
	public:
		virtual ~PrimitiveFactory() {}
		virtual Primitive* Create(Vector3f va, Vector3f vb, Vector3f vc, Vector3f na, Vector3f nb, Vector3f nc, PAIR ta, PAIR tb, PAIR tc) const = 0;
	};
	
	class TriangleFactory : public PrimitiveFactory
	{
		Primitive* Create(Vector3f va, Vector3f vb, Vector3f vc, Vector3f na, Vector3f nb, Vector3f nc, PAIR ta, PAIR tb, PAIR tc) const
		{
			Vector3f nullVec;
			nullVec << NULL, NULL, NULL;
			
			PAIR nullP;
			nullP.first = NULL;
			nullP.second = NULL;
			
			return new Triangle(va, vb, vc, nullVec, nullVec, nullVec, nullP, nullP, nullP);
		}
	};
	
	class SmoothTriangleFactory : public PrimitiveFactory
	{
		Primitive* Create(Vector3f va, Vector3f vb, Vector3f vc, Vector3f na, Vector3f nb, Vector3f nc, PAIR ta, PAIR tb, PAIR tc) const
		{
			PAIR nullP;
			nullP.first = NULL;
			nullP.second = NULL;
			
			return new Triangle(va, vb, vc, na, nb, nc, nullP, nullP, nullP);
		}
	};
	
	class TexturedSmoothTriangleFactory : public PrimitiveFactory
	{
		Primitive* Create(Vector3f va, Vector3f vb, Vector3f vc, Vector3f na, Vector3f nb, Vector3f nc, PAIR ta, PAIR tb, PAIR tc) const
		{
			return new Triangle(va, vb, vc, na, nb, nc, ta, tb, tc);
		}
	};
	
	
	Object(bool useAcc)
	{
		m_useAcc = useAcc;
		kdtree = NULL;
		_visible = true;
		c.set(0.0, 0.0, 0.0);
		position << 0.0, 0.0, 0.0;
        strcpy(name, "");
	}
	
	~Object()
	{
		for (int i = 0; i<primitive.size(); i++) {
			delete primitive[i];
		}
		if (kdtree) {
			delete kdtree;
		}
	}
	
	void setVisible(bool val) {_visible = val;}
	
	
	bool Intersect(Ray &r)
	{
		
		
		if (_visible) {
			if (m_useAcc) {
				return kdtree->Intersect(r);
			}
			else {
				bool hit = false;
				float th = r.tmax;
				for (int i = 0; i < primitive.size(); i++) {
					if (primitive[i]->Intersect(r)) {
						if (r.t < th && r.t > Epsilon) {
							th = r.t;
							hit = true;
							r.o = this;
						}
					}
				}
				return hit;
			}
			return false;
		}
		else {
			return false;
		}

	};
	
	Box CalcBounds()
	{
        Box boundingBox;
		boundingBox.Clear();
		for (int i = 0; i < primitive.size(); i++) {
			Box box = primitive[i]->GetBounds();
			boundingBox.Extend(box);
		}
		position = boundingBox.min;
		return boundingBox;
	}
	
	void BuildAccelStructure()
	{
        
        
		if (!m_useAcc) {
			return;
		}
		
		Box boundingBox = CalcBounds();
		
		if (kdtree) {
			delete kdtree;
		}
        
		kdtree = new KDTree();
		
		kdtree->BuildTree(primitive, boundingBox);
	}
    
    KDTree* BuildKDTree()
	{
        KDTree* tree;
        
		Box boundingBox = CalcBounds();
		
		tree = new KDTree();
		
		tree->BuildTree(primitive, boundingBox);
        return tree;
	}
	
    void Update()
    {
        
        for (int i = 0; i < primitive.size(); i++) {
            Primitive* p = primitive[i];
            
            //update primitive with object attributes
            Vector3f delta = position - p->position;
            p->position = p->position + delta;
            
            //calculate the new bounding box
            p->calcBounds();
            p->setBounds(true);
            
        }
        //rebuild KDTree
        kdtree = BuildKDTree();
        
    }
	
	void setShader(Shader* sh)
	{
		for (int i = 0; i < primitive.size(); i++) {
			primitive[i]->setShader(sh);
		}
	}
	
	
	struct FaceId {
		int v[4], vn[4], vt[4];
		FaceId()
		{ memset(this, 0, sizeof(*this)); }
		
		FaceId(istream &aStream)
		{
			memset(this, 0, sizeof(*this));
			char c;
			
			for (int i = 0; i < 3; i++) {
				aStream >> ws >> v[i];
				
				if(aStream.peek() != '/')
				{
					continue;
				}
				aStream >> c >> ws;
				if(aStream.peek() == '/')
				{
					aStream >> c >> ws;
					continue;
				}
				else {
					aStream >> vt[i];
				}
				if (aStream.peek() != '/')
				{
					continue;
				}
				aStream >> c >> vn[i];
			}
		}
	};
	
	
	void ParseObj(char *fileName, PrimitiveFactory *factory = new TriangleFactory())
	{
		char string[100];
		char inputFilename[200];
		
		strcpy(inputFilename, fileName);
		ifstream inputFile(inputFilename, ios::in);
		
		//--------------------------------------------
		// if not a valid file
		if (!inputFile)
		{
			cerr << "ERROR opening file: " << inputFilename << endl;
		}
		//--------------------------------------------
		// process scene file
		else {
			cout << "Parsing OBJ File: " << fileName << endl;
			
			vector<Vector3f> vert, norm;
			vector<PAIR> tex;
			vector<FaceId> faces;
			
			
			while ((inputFile >> string) && !inputFile.eof()) 
			{
				if(!strcmp(string, "v"))
				{
					Vector3f v;
					inputFile >> v[0];
					inputFile >> v[1];
					inputFile >> v[2];
					vert.push_back(v);
				}   
				if(!strcmp(string, "vn"))
				{
					Vector3f n;
					inputFile >> n[0];
					inputFile >> n[1];
					inputFile >> n[2];
					norm.push_back(n);
				}
				if(!strcmp(string, "vt"))
				{
					PAIR t;
					inputFile >> t.first;
					inputFile >> t.second;
					tex.push_back(t);
				}
				
				if(!strcmp(string, "f"))
				{
					faces.push_back(inputFile);
				}
				   
			}
			
			
			for (vector<FaceId>::iterator it = faces.begin(); it != faces.end(); it++) {
				Vector3f v[3], vn[3];
				PAIR vt[3];
				
				for (int i = 0; i < 3; i++) {
					v[i] = vert[it->v[i]-1];
					if (it->vn[i] >= 1 && it->vn[i] <= (int)norm.size())
						vn[i] = norm[it->vn[i] - 1];
					if (it->vt[i] >= 1 && it->vt[i] <= (int)tex.size())
						vt[i] = tex[it->vt[i] - 1];
					
				}
				
				Add(factory->Create(v[0], v[1], v[2], vn[0], vn[1], vn[2], vt[0], vt[1], vt[2]));
				
			}
			
			cout << "Verticies: " << vert.size() << " Faces: " << faces.size() << endl;
			
			
		}
		
	}
	
	
	
	
	
};

#endif