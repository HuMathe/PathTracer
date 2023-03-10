#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  bool found_one = false;
  double hit_time;
  Vector3D hit_point;
  int d0 = 0, d1 = 1, d2 = 2;
  if(r.d[d0] != 0) {
    hit_time = (min[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
    hit_time = (max[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
  }
  std::swap(d0, d1);
  if(r.d[d0] != 0) {
    hit_time = (min[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
    hit_time = (max[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
  }
  std::swap(d0, d2);
  if(r.d[d0] != 0) {
    hit_time = (min[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
    hit_time = (max[d0]-r.o[d0]) / r.d[d0];
    hit_point = r.at_time(hit_time);
    if(min[d1] <= hit_point[d1] && hit_point[d1] <= max[d1]) {
      if(min[d2] <= hit_point[d2] && hit_point[d2] <= max[d2]) {
        if(found_one) {
          t1 = hit_time;
          if(t1 < t0) std::swap(t0, t1);
          return true;
        }
        t0 =hit_time;
        found_one = true;
      }
    }
  }
  return found_one;
}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
