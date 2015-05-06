/*
 *  KDTree.cpp
 *  Raytracer
 *
 *  Created by Amber on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "KDTree.h"
#include <iostream>


KDTree::KDNode::KDNode()
{
	_left = 0;
	_right = 0;
	_splitAxis = NO_AXIS;
	_splitPlane = 0.0f;
}

KDTree::KDNode::~KDNode()
{
	if (_left) {
		delete _left;
	}
	if (_right) {
		delete _right;
	}
}

bool KDTree::KDNode::leafIntersect(Ray& r, float min, float max)
{
	//store a copy of the ray
	Ray ray = r;
	bool found = false;
	
	//search nearest intersection
	
	for (int i = 0; i < _primitives.size(); i++) {
		//if the is intersection
		if (_primitives[i]->Intersect(ray)) {
			//only proceed if the intersection is closer than the previous
			if (ray.t >= min - Epsilon && ray.t < max) {
				r = ray;
				r.hit = _primitives[i];
				
				max = ray.t;
				found = true;
			}
		}
	}
	return found;
}

KDTree::KDTree()
{
	_root = 0;
	_maxPrimitives = 10;
	_isectCost = 80;
	_traversalCost = 1;
	_emptyBonus = 0.5f;
}

KDTree::~KDTree()
{
	if (_root) {
		delete _root;
	}
}

bool KDTree::Intersect(Ray& r)
{
	
	//intersect with bounding box of the kdtree
	pair<float, float> tmm = _boundingBox.Intersect(r);
	if (tmm.first >= FLT_MAX - Epsilon) {
		return false;
	}
	return Intersect(_root, r, tmm.first, tmm.second);
}

bool KDTree::Intersect(KDNode* node, Ray& r, float min, float max)
{
	
	//if node is null dont do anything
	if (node == NULL) {
		return false;
	}
	
	//if the leaf node intersects
	if (node->isLeafNode()) {
		return node->leafIntersect(r, min, max);
	}
	//get the near and far
	KDNode* near = 0;
	KDNode* far = 0;
	node->getNearFar(r, near, far);
	
	//compute the distance to the split plane
	float distance = node->distanceToSplitPlane(r);
	
	//the whole interval is on the near side
	if (distance < 0 || distance > max) {
		return Intersect(near, r, min, max);
	}
	//the whole intervalis on the far side
	else if(distance <= min)
	{
		return Intersect(far, r, min, max);
	}
	//the interval intersects the plane
	else {
		//first test the near side
		if (Intersect(near, r, min, distance)) {
			return true;
		}
		//then test the far side
		return Intersect(far,r,distance,max);
	}
	// bug in the system after this point
	assert(0);
	return false;
}

void KDTree::BuildTree(KDNode* node, const Box& nodeBounds, int depth, int badRefines)
{
	assert(node->_left == NULL);
	assert(node->_right == NULL);
	int nPrims = node->_primitives.size();
	
	//initialize lead node if certain criteria
	if (nPrims <= _maxPrimitives || depth >= _maxDepth) {
		node->_splitAxis = NO_AXIS;
		return;
	}
	
	//choose split axis for interior node
	Axis bestAxis = NO_AXIS;
	int bestOffset = -1;
	
	float bestCost = INFINITY;
	float oldCost = _isectCost * float(nPrims);
	
	//try to find a good split position
	retrySplit(node, nodeBounds, bestAxis, bestCost, bestOffset, 0);
	
	if (bestCost > oldCost) {
		++badRefines;
	}
	
	//create lead if no good splits were found
	if ((bestCost > 4.f * oldCost && nPrims < 16) || bestAxis == NO_AXIS ||badRefines == 3) {
		//give up and create a leaf node
		node->_splitAxis = NO_AXIS;
		return;
	}
	
	//classify primitives with respect to split
	//create children
	node->_left = new KDNode();
	node->_right = new KDNode();
	
	for (int i = 0; i < bestOffset; ++i) {
		if (_edges[bestAxis][i].type == BoundEdge::START) {
			node->_left->_primitives.push_back(node->_primitives[_edges[bestAxis][i].primNum]);
		}
	}
	for (int i = bestOffset + 1; i < 2 * nPrims; ++i) {
		if (_edges[bestAxis][i].type == BoundEdge::END) {
			node->_right->_primitives.push_back(node->_primitives[_edges[bestAxis][i].primNum]);
		}
	}
	
	//recursivly initialize children
	float tsplit = _edges[bestAxis][bestOffset].t;
	node->_splitAxis = bestAxis;
	node->_splitPlane = tsplit;
	
	//calculate new bounds
	Box bounds0 = nodeBounds;
	Box bounds1 = nodeBounds;
	bounds0.max[bestAxis] = bounds1.min[bestAxis] = tsplit;
	
	//clear old primitives
	node->_primitives.clear();
	
	//recursivly build tree
	BuildTree(node->_left, bounds0, depth + 1, badRefines);
	BuildTree(node->_right, bounds1, depth + 1, badRefines);
}

void KDTree::retrySplit(KDNode* node, Box nodeBounds, Axis& bestAxis, float& bestCost, int& bestOffset, int retries)
{
	int nPrims = node->_primitives.size();
	Vector3f d = nodeBounds.max - nodeBounds.min;
	
	//choose which axis to split along
	Axis axis;
	if (d(0) > d(1) && d(0) > d(2)) {
		axis = X_AXIS;
	}
	else {
		axis = (d(1) > d(2)) ? Y_AXIS : Z_AXIS;
	}
	
	float totalSA = 2.f * (d(0)*d(1) + d(0)*d(2) + d(1)*d(2));
	float invTotalSA = 1.f / totalSA;
	
	//initialize edges for the axis
	for (int i = 0; i < nPrims; ++i) {
		const Box& bbox = node->_primitives[i]->GetBounds();
		_edges[axis][2*i] = BoundEdge(bbox.min[axis], i, true);
		_edges[axis][2*i+1] = BoundEdge(bbox.max[axis],i,false);
	}
	
	std::sort(&_edges[axis][0], &_edges[axis][2*nPrims]);
	
	// compute the cost for the axis to find the best
	int nBelow = 0;
	int nAbove = nPrims;
	for (int i = 0; i < 2*nPrims; ++i) {
		if (_edges[axis][i].type == BoundEdge::END) {
			--nAbove;
		}
		float edge_t = _edges[axis][i].t;
		if (edge_t > nodeBounds.min[axis] && edge_t < nodeBounds.max[axis]) {
			//compute the cost of the split at the ith edge
			int otherAxis[3][2] = {{1,2}, {0,2}, {0,1}};
			Axis otherAxis0 = (Axis) otherAxis[axis][0];
			Axis otherAxis1 = (Axis) otherAxis[axis][1];
			float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] + (edge_t -nodeBounds.min[axis]) * (d[otherAxis0] + d[otherAxis1]));
			float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] + (nodeBounds.max[axis] - edge_t) * (d[otherAxis0] + d[otherAxis1]));
			
			float pBelow = belowSA * invTotalSA;
			float pAbove = aboveSA * invTotalSA;
			float eb = (nAbove == 0 || nBelow == 0) ? _emptyBonus : 0.f;
			float cost = _traversalCost + _isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);
			
			//update best split if this is lowest
			if (cost < bestCost) {
				bestCost = cost;
				bestAxis = axis;
				bestOffset = i;
			}
		}
		if (_edges[axis][i].type == BoundEdge::START) {
			++nBelow;
		}
	}
	if (bestAxis == NO_AXIS && retries < 2) {
		axis = Axis((axis+1) % 3);
		retrySplit(node, nodeBounds, bestAxis, bestCost, bestOffset, retries+1);
	}

}
					
void KDTree::BuildTree(const vector<Primitive*>& list, const Box& bbox)
{
	//delete root node and rebuild
	if (_root) {
		delete _root;
	}
	
	//compute a reasonable tree depth
	_maxDepth = (int) (8 + 1.3f * log(double(list.size())));
	
	//debug
	//cout << "Start building KD-Tree..." << endl;
	
	//create empty root node
	_root = new KDNode();
	
	//reset the bounding box
	_boundingBox = bbox;
	
	//copy the primitives
	_root->_primitives = list;
	
	for (int i = 0; i < 3; i++) {
		_edges[i] = new BoundEdge[2*list.size()];
	}
	//allocated memory for KD-Tree
	
	//create tree recursively
	BuildTree(_root, bbox, 0, 0);
	for (int i = 0; i < 3; i++) {
		delete[] _edges[i];
	}
	
	//debug
	//cout << "KD-Tree Built" << endl;
}
						
						
						
						
						
						





