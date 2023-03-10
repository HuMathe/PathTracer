#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out;
  double rate = (2.0 * PI / ns_area_light);
  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading 
  for(int i = 0; i < num_samples; ++ i) {
    Vector3D w_in;
    Vector3D new_d;
    Vector3D delta_L;
    Intersection new_isect;
    double pdf;
    Vector3D f = isect.bsdf->sample_f(w_out, &w_in, &pdf);
    new_d = o2w * w_in;
    Ray new_r(hit_p, new_d);
    new_r.min_t = EPS_F;
    new_r.max_t = INF_D;
    if(bvh->intersect(new_r, &new_isect)) {
      delta_L =  f *  new_isect.bsdf->get_emission() * cos_theta(w_in);
    }
    L_out += rate * delta_L;
  }
  return L_out;

}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {

  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
  Vector3D w_in, L_out;
  int num_sample;
  double pdf, dist;
  Intersection new_isect;
  for(auto it = scene->lights.begin(); it != scene->lights.end(); it++ ) {
    if( (*it)->is_delta_light() ) num_sample = 1;
    else  num_sample = ns_area_light;
    for( int i = 0; i < num_sample; i++ ) {
      Vector3D Li = (*it)->sample_L(hit_p, &w_in, &dist, &pdf);
      Ray new_r (hit_p, w_in.unit());
      new_r.min_t = EPS_F;
      new_r.max_t = dist - EPS_F;
      w_in = w2o * w_in;
      if(w_in[2] < 0) continue;
      if(!bvh->intersect(new_r, &new_isect)) {
        Vector3D dL = isect.bsdf->f(w_out, w_in) * Li * cos_theta(w_in) / pdf;
        L_out += (1.0 / num_sample) * dL;
      }
    }
  }
  return L_out;
}
Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light
  return isect.bsdf->get_emission();
}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`
if(direct_hemisphere_sample) return estimate_direct_lighting_hemisphere(r, isect);
  return estimate_direct_lighting_importance(r, isect);
}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
  if (r.depth >= max_ray_depth) return Vector3D();
  Vector3D L_out = one_bounce_radiance(r, isect);
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();
  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);
  Vector3D wi, w_in; 
  double pdf;
  Intersection new_isect;
  double russian_p = 0.7;
  double rate = (1.0 / russian_p);
  Vector3D f = isect.bsdf->sample_f(w_out, &w_in, &pdf);
  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.
  Vector3D new_d;
  Vector3D delta_L(0, 0, 0);
  new_d = o2w * w_in;
  Ray new_r(hit_p, new_d);
  new_r.min_t = EPS_F;
  new_r.max_t = INF_D;
  new_r.depth = r.depth + 1;
  if(bvh->intersect(new_r, &new_isect) && coin_flip(russian_p)) {
    delta_L =  f * at_least_one_bounce_radiance(new_r, new_isect) * cos_theta(w_in) / pdf;
  }
  L_out += rate * delta_L;


  return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect))
    return envLight ? envLight->sample_dir(r) : L_out;


  L_out = (isect.t == INF_D) ? debug_shading(r.d) : zero_bounce_radiance(r, isect);

  // TODO (Part 3): Return the direct illumination.
  // L_out += one_bounce_radiance(r, isect);
  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct
  L_out += at_least_one_bounce_radiance(r, isect);
  return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa, i;       // total samples to evaluate
  Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel
  double pos_x, pos_y, dx, dy, illu;
  Vector3D color(0, 0, 0), d_color;
  double s1 = 0, s2 = 0;
  for(i = 1; i <= num_samples; ++i) {
    Vector2D temp = gridSampler->get_sample();
    dx = temp.x, dy = temp.y;
    //std::cout << "INFO: " << ns_aa << " " << dx << " " << dy << std::endl;
    pos_x = 1.0 * (dx + x) / sampleBuffer.w;
    pos_y = 1.0 * (dy + y) / sampleBuffer.h;
    Ray r = camera->generate_ray(pos_x, pos_y);
    d_color = est_radiance_global_illumination(r);
    color += d_color;
    illu = d_color.illum();
    s1 += illu;
    s2 += illu * illu;
    if( i % samplesPerBatch == 0 ) {
      if(s1 < EPS_F) break;
      if(1.96 * sqrt((s2 - s1*s1/i) / (i-1)) / sqrt(i) < maxTolerance * (s1 / i))
        break;
    }
  }
  if(i > num_samples) i = num_samples;
  color = (1.0 / i) * color;
  sampleBuffer.update_pixel(color, x, y);
  sampleCountBuffer[x + y * sampleBuffer.w] = i;
}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL
