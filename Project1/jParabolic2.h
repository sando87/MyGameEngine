#pragma once
#include "Vectors.h"


class jAccVelPos
{
public:
	jAccVelPos() : AccTime(0) {}
	double AccTime;
	Vector3 Acc;
	Vector3 Vel;
	Vector3 Pos;
	Vector3 Next(double dt)
	{
		AccTime += dt;
		Vel += Acc * dt;
		Pos += Vel * dt;
	}
};


class jParabolic2
{
public:
	void Init(Vector2 center, Vector2 point);
	double CalcY(double x);
	double CalcYAcc(double dx);
	double CalcVelAcc(double dx);
	double AccX;

private:
	double coeffA;
	double coeffB;
	double coeffC;
	
};

