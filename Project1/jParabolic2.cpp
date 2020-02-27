#include "jParabolic2.h"
#include "junks.h"

void jParabolic2::Init(Vector2 center, Vector2 point)
{
	double mod = (point.x - center.x) * (point.x - center.x);
	mod = max(0.0000001, mod);
	double k = (point.y - center.y) / mod;
	coeffA = k;
	coeffB = -2 * k * center.x;
	coeffC = k * center.x * center.x + center.y;
	AccX = 0;
}

double jParabolic2::CalcY(double x)
{
	double x2 = x * x;
	return coeffA * x2 * x2 + coeffB * x2 + coeffC;
}

double jParabolic2::CalcYAcc(double dx)
{
	AccX += dx;
	return CalcY(AccX);
}
