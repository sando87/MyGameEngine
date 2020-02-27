#include "jLinear.h"

void jLinear::Init(Vector2 ptA, Vector2 ptB)
{
	coeffA = (ptB.x == ptA.x) ? 0 : (ptB.y - ptA.y) / (ptB.x - ptA.x);
	coeffB = ptA.y - ptA.x * coeffA;
}
void jLinear::Init(double _coeffA, double _coeffB)
{
	coeffA = _coeffA;
	coeffB = _coeffB;
	AccX = 0;
}
double jLinear::CalcY(double x)
{
	return coeffA * x + coeffB;
}
double jLinear::CalcYAcc(double dx)
{
	AccX += dx;
	return CalcY(AccX);
}
