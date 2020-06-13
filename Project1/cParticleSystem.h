#pragma once
#include "jComponent.h"
#include "jLinear.h"
#include "jParabolic2.h"


class Particle
{
public:
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
	float rotate;
	float reserve;
	Vector4f color;
};

class cParticleSystem :
	public jComponent
{
public:
	cParticleSystem();
	virtual ~cParticleSystem();

	void Start() { mStart = true; }
	void Stop() { mStart = false; Reset(); }
	list<Particle*>& GetParticles() { return mParticles; };
	function<void(Particle&)> EventStartParticle;
	function<void(Particle&)> EventUpdateParticle;
	function<void(void)> EventFinished;

protected:
	virtual void OnLoad();
	virtual void OnUpdate();

	void Burst();
	void Reset();
	bool IsFinished();
	double RandomRate();
	Vector3 RandomForce();
	void ForceGravity(Particle& particle);
	void ForceAirDrag(Particle& particle);
	void InitParticle(Particle& particle);
	void UpdateParticle(Particle& particle);
	
	list<Particle*> mParticles;
	double mWaitTime;
	int mCurrnetBurstIndex;
	Matrix4 mRotateMat;

	public: void SetDirection(Vector3 dir);
	Property_Getter(Vector3, Direction, Vector3(0, 0, 1))
	Property_GetSetter(Vector3, Position, Vector3())
	Property_GetSetter(Vector3, Gravity, Vector3(0, 0, -9.8))
	Property_GetSetter(double, CoeffDrag, 0.01)
	Property_GetSetter(int, MaxCount, 40)
	Property_GetSetter(int, Count, 10)
	Property_GetSetter(int, Degree, 45)
	Property_GetSetter(double, MassRate, 1)
	Property_GetSetter(double, StartDelay, 0)
	Property_GetSetter(double, Duration, 5)
	Property_GetSetter(double, RandomRate, 0.1)
	Property_GetSetter(double, Force, 100)
	Property_GetSetter(int, BurstCount, 1) // BurstCount < 0 => infinite loop mode, BurstCount == 0 => user brust mode.
	Property_GetSetter(double, BurstIntervalSec, 1)
	Property_Getter(bool, Start, true)
};
