#pragma once

#define JMATH_PI 3.141592654
#define DegToRad (JMATH_PI / 180.0)
#define RadToDeg (180.0 / JMATH_PI)

#define IsOutofOne(x) ((0.9999<(x) && (x)<1.0001) ? false : true)