#pragma once
#include "Vectors.h"

class jParabolic2
{
public:
	void Init(Vector2 center, Vector2 point);
	double CalcY(double x);
	double CalcYAcc(double dx);
	double AccX;

private:
	double coeffA;
	double coeffB;
	double coeffC;
	
};

