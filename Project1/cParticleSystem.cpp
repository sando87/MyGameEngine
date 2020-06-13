#include "cParticleSystem.h"
#include "jTime.h"
#include "jTransform.h"

cParticleSystem::cParticleSystem()
{
}


cParticleSystem::~cParticleSystem()
{
	Reset();
}

void cParticleSystem::OnLoad()
{
	Reset();
}

void cParticleSystem::OnUpdate()
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
		UpdateParticle(*parti);

		if(EventUpdateParticle)
			EventUpdateParticle(*parti);

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

	if (EventFinished && IsFinished())
		EventFinished();
}

void cParticleSystem::Burst()
{
	for (int i = 0; i < mCount; ++i)
	{
		Particle* particle = new Particle();
		InitParticle(*particle);

		if (EventStartParticle)
			EventStartParticle(*particle);

		mParticles.push_back(particle);
	}
}
void cParticleSystem::InitParticle(Particle & particle)
{
	particle.Pos = mPosition;
	particle.Vel = Vector3();
	particle.AccTime = 0;
	particle.LifeTime = mDuration * RandomRate();
	particle.Death = false;
	particle.mass = mMassRate * RandomRate();
	particle.size = 1;
	particle.texIdx = 0;
	particle.refDiscard = 0;
	particle.rotate = 0;
	particle.reserve = 0;
	particle.color = Vector4f(1, 1, 1, 1);

	Vector3 force = RandomForce() * mForce * RandomRate();
	particle.forces.push_back(force);
}

void cParticleSystem::UpdateParticle(Particle & particle)
{
	particle.AccTime += jTime::Delta();
	particle.Death = (particle.LifeTime < particle.AccTime) ? true : false;

	ForceGravity(particle);
	ForceAirDrag(particle);

	Vector3 forceTotal;
	for (Vector3 force : particle.forces)
		forceTotal += force;

	Vector3 acc = particle.mass == 0 ? Vector3() : forceTotal / particle.mass;
	particle.Vel += acc * jTime::Delta();
	particle.Pos += particle.Vel * jTime::Delta();
	particle.forces.clear();
}

bool cParticleSystem::IsFinished()
{
	if (mBurstCount < 0)
		return false;

	if(mCurrnetBurstIndex < mBurstCount)
		return false;

	if (!mParticles.empty())
		return false;

	return true;
}

void cParticleSystem::Reset()
{
	for (auto iter : mParticles)
		delete iter;
	mParticles.clear();

	mWaitTime = -mStartDelay;
	mCurrnetBurstIndex = 0;
}

Vector3 cParticleSystem::RandomForce()
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

double cParticleSystem::RandomRate()
{
	int a = (jUtils::Random() % 200) - 100;
	double b = (double)a / 100.0;
	b *= mRandomRate;
	return 1 - b;
}

void cParticleSystem::ForceGravity(Particle& particle)
{
	particle.forces.push_back(mGravity * particle.mass);
}
void cParticleSystem::ForceAirDrag(Particle& particle)
{
	Vector3 dragDir = particle.Vel * -1.0;
	double dragMag = dragDir.length();
	dragDir.normalize();
	Vector3 force = mCoeffDrag * dragMag * dragMag * dragDir;
	particle.forces.push_back(force);
}

void cParticleSystem::SetDirection(Vector3 dir)
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
