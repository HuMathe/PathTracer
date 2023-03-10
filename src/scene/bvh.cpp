#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>
using namespace std;

namespace CGL {
namespace SceneObjects {
bool using_stack = false;
BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}
struct State{
  std::vector<Primitive *>::iterator start;
  std::vector<Primitive *>::iterator end;
  BVHNode * top_node;
};

void stack_construct( std::stack<State> &S, size_t max_leaf_size) {
  std::vector<Primitive *>::iterator start = S.top().start;
  std::vector<Primitive *>::iterator end = S.top().end;
  BBox bbox;
  int cnt = 0;
  Vector3D avg_centroid(0, 0, 0), sqr_centroid(0, 0, 0);
  for (auto p = start; p != end; p++) {
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
    avg_centroid += bb.centroid();
    sqr_centroid += bb.centroid() * bb.centroid();
    cnt ++;
  }
  BVHNode *node = S.top().top_node;
  S.pop();
  node->bb = bbox;
  node->start = start;
  node->end = end;
  if(cnt <= max_leaf_size) return ;
  avg_centroid = (1.0 / cnt) * avg_centroid;
  sqr_centroid = (1.0 / cnt) * sqr_centroid;
  sqr_centroid = sqr_centroid - avg_centroid * avg_centroid;
  int dim = 0;
  if(sqr_centroid[1] > sqr_centroid[dim]) dim = 1;
  if(sqr_centroid[2] > sqr_centroid[dim]) dim = 2;
  double mid_val = avg_centroid[dim];
  std::vector<Primitive *> leftp, rightp;
  for(std::vector<Primitive *>::iterator it = start; it != end; it++ ) {
    BBox bb = (*it)->get_bbox();
    if( bb.centroid()[dim] < mid_val ) leftp.push_back(*it);
    else rightp.push_back(*it);
  }
  if((leftp.size() | rightp.size()) == 0) return ;
  int i = 0, lenl = leftp.size();
  std::vector<Primitive *>::iterator mid;
  for(std::vector<Primitive *>::iterator it = start; it != end; it++, i ++ ) {
    if(i < lenl) {
      (*it) = leftp[i];
      mid = it;
    } else *it = rightp[i - lenl];
  }
  node->l = new BVHNode(BBox());
  node->r = new BVHNode(BBox());
  S.push(State{start, mid+1, node->l});
  S.push(State{mid+1, end, node->r});
  return ;
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {
  
  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.
  if(using_stack) {
    std::stack<State> S;
    BVHNode* node = new BVHNode(BBox());
    S.push(State{start, end, node});
    while(!S.empty()) 
      stack_construct( S, max_leaf_size );
    return node;
  }
  BBox bbox;
  int cnt = 0;
  Vector3D avg_centroid(0, 0, 0), sqr_centroid(0, 0, 0);
  for (auto p = start; p != end; p++) {
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
    avg_centroid += bb.centroid();
    sqr_centroid += bb.centroid() * bb.centroid();
    cnt ++;
  }
  BVHNode *node = new BVHNode(bbox);
  node->start = start;
  node->end = end;
  if(cnt <= max_leaf_size) return node;
  avg_centroid = (1.0 / cnt) * avg_centroid;
  sqr_centroid = (1.0 / cnt) * sqr_centroid;
  sqr_centroid = sqr_centroid - avg_centroid * avg_centroid;
  int dim = 0;
  if(sqr_centroid[1] > sqr_centroid[dim]) dim = 1;
  if(sqr_centroid[2] > sqr_centroid[dim]) dim = 2;
  double mid_val = avg_centroid[dim];
  std::vector<Primitive *> leftp, rightp;
  for(std::vector<Primitive *>::iterator it = start; it != end; it++ ) {
    BBox bb = (*it)->get_bbox();
    if( bb.centroid()[dim] < mid_val ) leftp.push_back(*it);
    else rightp.push_back(*it);
  }
  if((leftp.size() | rightp.size()) == 0) return node;
  int i = 0, lenl = leftp.size();
  std::vector<Primitive *>::iterator mid;
  for(std::vector<Primitive *>::iterator it = start; it != end; it++, i ++ ) {
    if(i < lenl) {
      (*it) = leftp[i];
      mid = it;
    } else *it = rightp[i - lenl];
  }
  node->l = construct_bvh(start, mid+1, max_leaf_size);
  node->r = construct_bvh(mid+1, end, max_leaf_size);
  return node;
}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  if( node == NULL ) return false;    
  double t1, t2;
  if(node->bb.intersect(ray, t1, t2)){
    bool hit = false;
    if(node->isLeaf()) {
      for (auto p = node->start; p != node->end; p++) {
        total_isects++;
        hit = (*p)->has_intersection(ray) || hit;
      }
      return hit;
    }
    if(has_intersection(ray, node->l)) hit = true;
    if(has_intersection(ray, node->r)) hit = true;
    return hit;
  }
  return false;
}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  if( node == NULL ) return false;    
  double t1, t2;
  if(node->bb.intersect(ray, t1, t2)){
    bool hit = false;
    if(node->isLeaf()) {
      for (auto p = node->start; p != node->end; p++) {
        total_isects++;
        hit = (*p)->intersect(ray, i) || hit;
      }
      return hit;
    }
    if(intersect(ray, i, node->l)) hit = true;
    if(intersect(ray, i, node->r)) hit = true;
    return hit;
  }
  return false;
}

} // namespace SceneObjects
} // namespace CGL
