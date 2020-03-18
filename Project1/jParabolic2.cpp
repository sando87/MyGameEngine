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
	return coeffA * x * x + coeffB * x + coeffC;
}

double jParabolic2::CalcYAcc(double dx)
{
	AccX += dx;
	return CalcY(AccX);
}

double jParabolic2::CalcVelAcc(double dx)
{
	AccX += dx;
	return 2 * coeffA * AccX + coeffB;
}
