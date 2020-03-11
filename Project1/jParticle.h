#pragma once
#include "jComponent.h"
#include "jLinear.h"
#include "jParabolic2.h"


class Particle
{
public:
	Particle();

	vector<Vector3> forces;
	Vector3 Vel;
	Vector3 Pos;
	
	double mass;
	double AccTime;
	double LifeTime;
	bool Death;
	float size;
	int texIdx;
	float refDiscard;
	Vector2f reserve;
	Vector4f color;

	void Force(Vector3 force);
	virtual void OnUpdate();
};

class jParticle :
	public jComponent
{
public:
	jParticle();
	virtual ~jParticle();

	list<Particle*>& GetParticles() { return mParticles; }
	void Start() { mStart = true; }
	void Stop() { mStart = false; Reset(); }
	void Burst();
	void Burst(Particle* particle);
	bool IsFinished();
	function<Particle*(void)> OnCreateParticle;

protected:
	virtual void OnLoad();
	virtual void OnUpdate();
	
	void Reset();
	Vector3 RandomForce();
	double RandomRate();
	void ForceGravity(Particle* particle);
	void ForceAirDrag(Particle* particle);
	
	list<Particle*> mParticles;
	double mWaitTime;
	int mCurrnetBurstIndex;
	Matrix4 mRotateMat;


	public: void SetDirection(Vector3 dir);
	Property_Getter(Vector3, Direction, Vector3(0, 0, 1))
	Property_GetSetter(Vector3, Position, Vector3())
	Property_GetSetter(Vector3, Gravity, Vector3(0, 0, -9.8))
	Property_GetSetter(double, CoeffDrag, 0.01)
	Property_GetSetter(int, Reserve, 40)
	Property_GetSetter(int, Count, 10)
	Property_GetSetter(int, Degree, 45)
	Property_GetSetter(double, MassRate, 1)
	Property_GetSetter(double, StartDelay, 0)
	Property_GetSetter(double, Duration, 5)
	Property_GetSetter(double, RandomRate, 0.1)
	Property_GetSetter(double, Force, 100)
	Property_GetSetter(int, BurstCount, 1)
	Property_GetSetter(double, BurstIntervalSec, 1)
	Property_Getter(bool, Start, true)
};
