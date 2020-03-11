#include "jParticle.h"
#include "jTime.h"
#include "jTransform.h"

jParticle::jParticle()
{
	OnCreateParticle = nullptr;
}


jParticle::~jParticle()
{
	Reset();
}

void jParticle::OnLoad()
{
	Reset();

	if (OnCreateParticle == nullptr)
		OnCreateParticle = []() { return new Particle(); };
}

void jParticle::OnUpdate()
{
	if (mStart == false)
		return;

	mWaitTime += jTime::Delta();
	if (mWaitTime <= 0)
		return;

	if (mBurstCount != 0) // burst mode 
	{
		if (mBurstIntervalSec < mWaitTime)
		{
			if(mBurstCount < 0) 
				Burst(); //infinite loop
			else if (mCurrnetBurstIndex < mBurstCount)
			{
				Burst();
				mCurrnetBurstIndex++;
			}
			mWaitTime = 0;
		}
	}

	for (auto iter = mParticles.begin(); iter != mParticles.end(); )
	{
		Particle* parti = *iter;
		parti->AccTime += jTime::Delta();
		ForceGravity(parti);
		ForceAirDrag(parti);
		parti->OnUpdate();
		if (parti->Death)
		{
			delete parti;
			mParticles.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void jParticle::Burst()
{
	for (int i = 0; i < mCount; ++i)
	{
		Particle* particle = OnCreateParticle();
		particle->Pos += mPosition;
		particle->LifeTime += mDuration * RandomRate();
		particle->mass *= mMassRate;
		Vector3 force = RandomForce() * mForce * RandomRate();
		particle->Force(force);
		mParticles.push_back(particle);
	}
}
void jParticle::Burst(Particle* particle)
{
	particle->Pos += mPosition;
	particle->LifeTime += mDuration * RandomRate();
	particle->mass *= mMassRate;
	Vector3 force = RandomForce() * mForce * RandomRate();
	particle->Force(force);
	mParticles.push_back(particle);
}

bool jParticle::IsFinished()
{
	if (mBurstCount < 0)
		return false;

	if(mCurrnetBurstIndex < mBurstCount)
		return false;

	if (!mParticles.empty())
		return false;

	return true;
}

void jParticle::Reset()
{
	for (auto iter : mParticles)
		delete iter;
	mParticles.clear();

	mWaitTime -= mStartDelay;
	mCurrnetBurstIndex = 0;
}

Vector3 jParticle::RandomForce()
{
	int deg = jUtils::Random() % 360;
	Vector3 axis(cos(DegToRad(deg)), 0, sin(DegToRad(deg)));
	int degree = mDegree == 0 ? 0 : jUtils::Random() % abs(mDegree);
	Matrix4 mat;
	mat.rotate(degree, axis);
	Vector3 transVec = Vector3(0, 1, 0) * mat;
	transVec = transVec * mRotateMat;
	
	transVec.normalize();
	return transVec;
}

double jParticle::RandomRate()
{
	int a = (jUtils::Random() % 200) - 100;
	double b = (double)a / 100.0;
	b *= mRandomRate;
	return 1 - b;
}

void jParticle::ForceGravity(Particle * particle)
{
	particle->Force(mGravity * particle->mass);
}
void jParticle::ForceAirDrag(Particle* particle)
{
	Vector3 dragDir = particle->Vel * -1.0;
	double dragMag = dragDir.length();
	dragDir.normalize();
	Vector3 force = mCoeffDrag * dragMag * dragMag * dragDir;
	particle->Force(force);
}

void jParticle::SetDirection(Vector3 dir)
{
	mDirection = dir;
	mDirection.normalize();
	jTransform trans;
	if (mDirection == Vector3(0, 0, 1))
		trans.lookat(Vector3(), mDirection, Vector3(0, -1, 0));
	else
		trans.lookat(Vector3(), mDirection, Vector3(0, 0, 1));
	mRotateMat = trans.getLocalMatrix();
}

Particle::Particle()
{
	Pos = Vector3();
	Vel = Vector3();
	AccTime = 0;
	LifeTime = 0;
	Death = false;
	mass = 1;
	size = 1;
	texIdx = 0;
	refDiscard = 0;
	color = Vector4f(1,1,1,1);
}

void Particle::Force(Vector3 force)
{
	forces.push_back(force);
}

void Particle::OnUpdate()
{
	Vector3 forceTotal;
	for (Vector3 force : forces)
		forceTotal += force;

	Vector3 acc = mass == 0 ? Vector3() : forceTotal / mass;
	Vel += acc * jTime::Delta();
	Pos += Vel * jTime::Delta();
	forces.clear();

	if (LifeTime < AccTime)
		Death = true;
}
