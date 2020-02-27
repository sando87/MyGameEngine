#pragma once
#include "Vectors.h"

class jLinear
{
public:
	void Init(Vector2 ptA, Vector2 ptB);
	void Init(double _coeffA, double _coeffB);
	double CalcY(double x);
	double CalcYAcc(double dx);
	double AccX;

private:
	double coeffA;
	double coeffB;
};

