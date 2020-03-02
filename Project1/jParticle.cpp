#include "jParticle.h"
#include "jTime.h"

jParticle::jParticle()
{
	OnCreateParticle = nullptr;
}


jParticle::~jParticle()
{
	Clear();
}

void jParticle::OnLoad()
{
	mTime = mBurstIntervalSec;
	mCurrnetBurstIndex = 0;
	mDirection.normalize();
	Vector3 axis = Vector3(0, 0, 1).cross(mDirection);
	double deg = RadToDeg(asin(axis.length()));
	mRotateMat.identity();
	mRotateMat.rotate(deg, axis);
	if (OnCreateParticle == nullptr)
		OnCreateParticle = []() { return new Particle(); };
}

void jParticle::OnUpdate()
{
	if (mStart == false)
		return;

	if (mBurstCount != 0) // burst mode 
	{
		mTime += jTime::Delta();
		if (mBurstIntervalSec < mTime)
		{
			if(mBurstCount < 0) 
				Burst(); //infinite loop
			else if (mCurrnetBurstIndex < mBurstCount)
			{
				Burst();
				mCurrnetBurstIndex++;
			}
			mTime = 0;
		}
	}

	for (auto iter = mParticles.begin(); iter != mParticles.end(); )
	{
		Particle* parti = *iter;
		parti->Time += jTime::Delta();
		UpdateDynamics(parti);
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
		particle->Pos = mPosition;
		particle->LifeTime = mDuration * RandomRate();
		Vector3 force = RandomForce() * mForce * RandomRate();
		particle->Force(force);
		mParticles.push_back(particle);
	}
}

void jParticle::Clear()
{
	for (auto iter : mParticles)
		delete iter;
	mParticles.clear();
}

Vector3 jParticle::RandomForce()
{
	int x = jUtils::Random() % 100 - 50;
	int y = jUtils::Random() % 100 - 50;
	Vector3 axis(x, y, 0);
	axis.normalize();
	int degree = jUtils::Random() % mDegree;
	Matrix4 mat;
	mat.identity();
	mat.rotate(degree, axis);
	Vector3 transVec = Vector3(0, 0, 1) * mat;
	if (mDirection != Vector3(0, 0, 1))
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

void jParticle::UpdateDynamics(Particle * particle)
{
	Vector3 dragDir = -particle->Vel;
	double dragMag = particle->Vel.length();
	dragDir.normalize();
	Vector3 force = mGravity * particle->mass + mCoeffDrag * dragMag * dragMag * dragDir;
	Vector3 acc = force / particle->mass;
	particle->Vel += acc * jTime::Delta();
	particle->Pos += particle->Vel * jTime::Delta();
}

Particle::Particle()
{
	Pos = Vector3();
	Vel = Vector3();
	Time = 0;
	LifeTime = 0;
	Death = false;
	mass = 1;
}

void Particle::Force(Vector3 force)
{
	Vector3 acc = force / mass;
	Vel += acc * jTime::Delta();
	Pos += Vel * jTime::Delta();
}

void Particle::OnUpdate()
{
	if (LifeTime < Time)
		Death = true;
}
