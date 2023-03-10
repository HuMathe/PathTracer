#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {
Vector3D getBarycentricCoodinates(const Vector3D &p1, const Vector3D &p2, const Vector3D &p3, const Vector3D &p) {
  Vector3D normal = cross(p2 - p1, p3 - p1).unit();
  double u, v, w; 
  u = dot(cross(p2 - p, p3 - p), normal);
  v = dot(cross(p3 - p, p1 - p), normal);
  w = dot(cross(p1 - p, p2 - p), normal);
  return ( 1.0 / (u + v + w) ) * Vector3D(u, v, w);
}
Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }

bool Triangle::has_intersection(const Ray &r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.
  Vector3D Normal = cross( ( p2 - p1 ), ( p3 - p1 ) );
  double up = dot( Normal, p1 - r.o );
  double dw = dot( Normal, r.d );
  if ( fabs(dw) < 1e-7 ) return false;
  double hit_time = up / dw;
  if ( hit_time < 0 || hit_time < r.min_t || hit_time > r.max_t ) return false;
  Vector3D hit_point = r.at_time(hit_time);
  Vector3D temp = getBarycentricCoodinates(p1, p2, p3, hit_point);
  double u (temp.x), v (temp.y), w (temp.z);
  if( u < 0 || v < 0 || w < 0 ) return false;
  r.max_t = hit_time;
  return true;
}

bool Triangle::intersect(const Ray &r, Intersection *isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  Vector3D Normal = cross( ( p2 - p1 ), ( p3 - p1 ) );
  double up = dot( Normal, p1 - r.o );
  double dw = dot( Normal, r.d );
  if ( fabs(dw) < 1e-7 ) return false;
  double hit_time = up / dw;
  //std::cout << "Debug INFO: " << hit_time << "|" << r.d << "|" << r.min_t << " " << r.max_t << std::endl;
  if ( hit_time < 0 || hit_time < r.min_t || hit_time > r.max_t ) return false;
  Vector3D hit_point = r.at_time(hit_time);
  Vector3D temp = getBarycentricCoodinates(p1, p2, p3, hit_point);
  double u (temp.x), v (temp.y), w (temp.z);
  //std::cout << "Debug INFO: " << temp << std::endl;
  if( u < 0 || v < 0 || w < 0 ) return false;
  r.max_t = hit_time;
  isect->primitive = this;
  isect->bsdf = this->bsdf;
  isect->t = hit_time;
  isect->n = u * n1 + v * n2 + w * n3;
  return true;


}

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
