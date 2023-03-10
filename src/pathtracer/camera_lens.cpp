#include "camera.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR, double rndTheta) const {

  // TODO Assignment 7: Part 4
  // compute position and direction of ray from the input sensor sample coordinate.
  // Note: use rndR and rndTheta to uniformly sample a unit disk.
  
  Vector3D pLen (lensRadius * sqrt(rndR) * cos(rndTheta), lensRadius * sqrt(rndR) * sin(rndTheta), 0);
  Vector3D direction (
    2.0 * (x - 0.5) * tan(hFov / 180.0 * PI / 2.0), 
    2.0 * (y - 0.5) * tan(vFov / 180.0 * PI / 2.0),
    -1
  );
  direction = direction - pLen / focalDistance;
  Ray ans (pos + c2w * pLen, (c2w * direction).unit());
  ans.min_t = fabs(nClip / direction.unit().z);
  ans.max_t = fabs(fClip / direction.unit().z);
  return ans;
}


} // namespace CGL
