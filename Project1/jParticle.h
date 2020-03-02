#pragma once
#include "jComponent.h"
#include "jLinear.h"
#include "jParabolic2.h"


class Particle
{
public:
	Particle();

	double mass;
	Vector3 Pos;
	Vector3 Vel;
	double Time;
	double LifeTime;
	bool Death;

	void Force(Vector3 force);
	virtual void OnUpdate();
};

class jParticle :
	public jComponent
{
public:
	jParticle();
	virtual ~jParticle();
	const list<Particle*>& GetParticles() { return mParticles; }
	function<Particle*(void)> OnCreateParticle;

protected:
	virtual void OnLoad();
	virtual void OnUpdate();
	
	void Burst();
	void Clear();
	Vector3 RandomForce();
	double RandomRate();
	void UpdateDynamics(Particle* particle);
	
	list<Particle*> mParticles;
	double mTime;
	int mCurrnetBurstIndex;
	Matrix4 mRotateMat;

	Property_GetSetter(Vector3, Position, Vector3())
	Property_GetSetter(Vector3, Direction, Vector3(0, 0, 1))
	Property_GetSetter(Vector3, Gravity, Vector3(0, 0, -9.8))
	Property_GetSetter(double, CoeffDrag, 0.01)
	Property_GetSetter(int, Count, 10)
	Property_GetSetter(int, Degree, 45)
	Property_GetSetter(double, Duration, 5)
	Property_GetSetter(double, RandomRate, 0.1)
	Property_GetSetter(double, Force, 100)
	Property_GetSetter(int, BurstCount, 1)
	Property_GetSetter(double, BurstIntervalSec, 1)
	Property_GetSetter(bool, Start, true)
};
