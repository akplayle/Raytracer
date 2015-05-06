/*
 *  KDTree.h
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef KDTREE_H
#define KDTREE_H

#include <Eigen/Dense>
#include "Primitive.h"
#include <vector>

using namespace std;

class KDTree
{
	enum Axis {
		X_AXIS = 0,
		Y_AXIS = 1,
		Z_AXIS = 2,
		NO_AXIS = 3
	};
	
	struct BoundEdge {
		float t;
		
		int primNum;
		
		enum {START, END} type;
		
		BoundEdge() { }
		
		BoundEdge(float tt, int pn, bool starting)
		{
			t = tt;
			primNum = pn;
			type = starting ? START : END;
		}
		
		bool operator<(const BoundEdge &e) const
		{
			if (t == e.t) {
				return (int)type < (int)e.type;
			}
			else {
				return t < e.t;
			}

		}
	};
	
	class KDNode {
		friend class KDTree;
		private:
		
			Axis _splitAxis;
			float _splitPlane;
			KDNode* _left;
			KDNode* _right;
		
			vector<Primitive*> _primitives;
			
			KDNode();
			~KDNode();
		
			inline bool isLeafNode() const {return _splitAxis == NO_AXIS; }
			bool leafIntersect(Ray &r, float min, float max);
		
			inline void getNearFar(const Ray& r, KDNode*& near, KDNode*& far)
			{
				if (_splitPlane >= r.origin[_splitAxis]) {
					near = _left;
					far = _right;
				}
				else {
					near = _right;
					far = _left;
				}
			}
		
			inline float distanceToSplitPlane(const Ray& r) const
			{
				return (_splitPlane - r.origin[_splitAxis]) / r.direction[_splitAxis];
			}
	};
	
	class KDNodeSmall {
		
		union  {
			unsigned int _flags; //Interior/Leaf
			float _split; //Interior
			unsigned int _nPrims; //Leaf
		};
		union {
			unsigned int _aboveChild; //Interior
			Primitive* _onePrimitive; //leaf
			Primitive** _primitives;
		};
		
		KDNodeSmall()
		{
		}
		
		~KDNodeSmall()
		{
			if (_primitives != 0) {
				delete[] _primitives;
			}
		}
		
		void initLeaf(int *primNums, int np, vector<Primitive*> primitives)
		{
			_nPrims = np << 2;
			_flags |= 3;
			
			if (np == 0) {
				_onePrimitive = 0;
				_primitives = 0;
			}
			else if (np == 1) {
				_onePrimitive = primitives[primNums[0]];
				_primitives = 0;
			}
			else {
				_primitives = new Primitive*[np];
				for (int i = 0; i < np; ++i) {
					_primitives[i] = primitives[primNums[i]];
				}
			}

		}
		
		void initInterior(Axis axis, float s)
		{
			_split = s;
			_flags &= ~3;
			_flags |= (int)axis;
		}
		
		float splitPos() const {return _split;}
		int nPrimitives() const {return _nPrims >> 2;}
		Axis splitAxis() const {return Axis(_flags & 3);}
		bool isLeaf() const {return (_flags & 3) == 3;}
	};
	
private:
	int _maxPrimitives;
	int _maxDepth;
	KDNode* _root;
	int _isectCost;
	int _traversalCost;
	float _emptyBonus;
	BoundEdge *_edges[3];
	
	bool Intersect(KDNode* node, Ray &r, float min, float max);
	void BuildTree(KDNode* node, const Box& nodeBounds, int depth, int badRefines);
	void retrySplit(KDNode* node, Box nodeBounds, Axis& bestAxis, float& bestCost, int& bestOffset, int retries);
	
public:
	Box _boundingBox;
	bool Intersect(Ray& r);
	void BuildTree(const vector<Primitive*>& list, const Box& bbox);
	KDTree();
	~KDTree();
	
};

#endif