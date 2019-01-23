#pragma once

#define jPI 3.141592654
#define DegToRad(deg) ((deg)*jPI/180.0)
#define RadToDeg(rad) ((rad)*180.0/jPI)

#define IsOutofOne(x) ((0.9999<(x) && (x)<1.0001) ? false : true)