#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "application/visual_debugger.h"

using std::max;
using std::min;
using std::swap;

namespace CGL {

// Mirror BSDF //

Vector3D MirrorBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D MirrorBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {

  // TODO Assignment 7: Part 1
  // Implement MirrorBSDF
  this->reflect(wo, wi);
  *pdf = 1.0;
  return this->reflectance / abs_cos_theta(*wi);
}

void MirrorBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Mirror BSDF"))
  {
    DragDouble3("Reflectance", &reflectance[0], 0.005);
    ImGui::TreePop();
  }
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D wo, const Vector3D wi) {
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D h) {
  // TODO Assignment 7: Part 2
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
  double theta = acos(dot(h.unit(), Vector3D(0, 0, 1)));
  return exp(-tan(theta)*tan(theta)/alpha/alpha)/PI/alpha/alpha/cos(theta)/cos(theta)/cos(theta)/cos(theta);
}

Vector3D MicrofacetBSDF::F(const Vector3D wi) {
  // TODO Assignment 7: Part 2
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Vector3D.
  double ci = cos_theta(wi);
  Vector3D Rs, Rp;
  for (int i = 0; i < 3; i++) {
    Rs[i] = ((eta[i]*eta[i] + k[i]*k[i]) - 2*eta[i]* ci + ci*ci)/((eta[i]*eta[i] + k[i]*k[i]) + 2*eta[i]* ci + ci*ci);
    Rp[i] = ((eta[i]*eta[i] + k[i]*k[i])*ci*ci - 2*eta[i]* ci + 1)/((eta[i]*eta[i] + k[i]*k[i])*ci*ci + 2*eta[i]* ci + 1);
  }
  return (Rs + Rp) / 2;
}

Vector3D MicrofacetBSDF::f(const Vector3D wo, const Vector3D wi) {
  // TODO Assignment 7: Part 2
  // Implement microfacet model here.
  if(wi.z < 0 || wo.z < 0) return Vector3D();
  return F(wi) * G(wo, wi) * D((wo + wi).unit()) / 4.0 / wi.z / wo.z;
}

Vector3D MicrofacetBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
  // TODO Assignment 7: Part 2
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.
  // *wi = cosineHemisphereSampler.get_sample(pdf);
  // return MicrofacetBSDF::f(wo, *wi);
  Vector2D r = sampler.get_sample();
  double inside = -alpha*alpha*log(1-r[0]);
  double theta = atan(sqrt(inside));
  double phi = 2.0*PI*r[1];
  Vector3D h (cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
  *wi = 2 * dot(wo, h) / dot(h, h) * h - wo;
  double a2 = alpha * alpha;
  *pdf = 1.0 / cos(theta) / cos(theta) / cos(theta) / a2 * exp(-tan(theta)*tan(theta)/a2) / PI  / 4.0 / dot(*wi, h);
  if(*pdf < 0 || wi->z < 0 || wo.z < 0) {
    *pdf = 0;
    return Vector3D();
  }
  return MicrofacetBSDF::f(wo, *wi);
}

void MicrofacetBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Micofacet BSDF"))
  {
    DragDouble3("eta", &eta[0], 0.005);
    DragDouble3("K", &k[0], 0.005);
    DragDouble("alpha", &alpha, 0.005);
    ImGui::TreePop();
  }
}

// Refraction BSDF //

Vector3D RefractionBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D RefractionBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
  // TODO Assignment 7: Part 1
  // Implement RefractionBSDF
  *pdf = 1;
  double eta;
  if (wo.z > 0) eta = 1. / ior;
  else  eta = ior;
  if(this->refract(wo, wi, this->ior))
    return this->transmittance / abs_cos_theta(*wi) / eta / eta;
  return Vector3D();
}

void RefractionBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Refraction BSDF"))
  {
    DragDouble3("Transmittance", &transmittance[0], 0.005);
    DragDouble("ior", &ior, 0.005);
    ImGui::TreePop();
  }
}

// Glass BSDF //

Vector3D GlassBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D GlassBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {

  // TODO Assignment 7: Part 1
  // Compute Fresnel coefficient and either reflect or refract based on it.

  // compute Fresnel coefficient and use it as the probability of reflection
  // - Fundamentals of Computer Graphics page 305
  if ( this->refract(wo, wi, this->ior) ) {
    double eta;
    if (wo.z > 0) eta = 1. / this->ior;
    else  eta = this->ior;
    double temp = 1 - std::fabs(wo.z);
    double R0 = (eta - 1) * (eta - 1) / (eta + 1) / (eta + 1);
    double R = R0 + ( 1 - R0 ) * temp * temp * temp * temp * temp;
    if (coin_flip(R)) {
      *pdf = R;
      this->reflect(wo, wi);
      return R * this->reflectance / abs_cos_theta(*wi);
    } else {
      *pdf = 1 - R;
      return ( 1 - R ) * this->transmittance / abs_cos_theta(*wi) / eta / eta;
    }
  }
  *pdf = 1;
  this->reflect(wo, wi);
  return this->reflectance / abs_cos_theta(*wi);
}

void GlassBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Refraction BSDF"))
  {
    DragDouble3("Reflectance", &reflectance[0], 0.005);
    DragDouble3("Transmittance", &transmittance[0], 0.005);
    DragDouble("ior", &ior, 0.005);
    ImGui::TreePop();
  }
}

void BSDF::reflect(const Vector3D wo, Vector3D* wi) {

  // TODO Assignment 7: Part 1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  wi->x = -wo.x;
  wi->y = -wo.y;
  wi->z = wo.z;
}

bool BSDF::refract(const Vector3D wo, Vector3D* wi, double ior) {

  // TODO Assignment 7: Part 1
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  double eta;
  if (wo.z > 0) eta = 1. / ior;
  else  eta = ior;
  
  double temp = 1 - eta * eta * ( 1 - wo.z * wo.z );
  if( temp < 0 ) return false;

  wi->x = - eta * wo.x;
  wi->y = - eta * wo.y;
  if ( wo.z > 0 ) wi->z = - sqrt( temp );
  else  wi->z = sqrt( temp );
  return true;

}

} // namespace CGL
