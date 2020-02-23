#pragma once

#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector3D.h"

#include <limits>

class Random
{
public:
	Random(double stddev = 1.);

	void InitSeed(int addseed);

	inline double getZeroOne()
	{
		
		return uniformZeroOne(rng);
	}

	inline double getNormal()
	{
		return normalDistribution(rng);
	}

	inline Vector3D<double> getRandomInUnitSphere()
	{
		Vector3D<double> res;

		do
		{
			res = 2. * Vector3D<double>(getZeroOne(), getZeroOne(), getZeroOne()) - Vector3D<double>(1, 1, 1);
		} while (res * res >= 1);

		return res;
	}


	inline Vector3D<double> getRandomVector(double stepLen = 1.)
	{
		return Vector3D<double>((getZeroOne() - 0.5) * stepLen, (getZeroOne() - 0.5) * stepLen, (getZeroOne() - 0.5) * stepLen);
	}

	inline Vector3D<double> getRandomGaussianVector()
	{
		return Vector3D<double>(normalDistribution(rng), normalDistribution(rng), normalDistribution(rng));
	}


	inline Vector3D<double> moveVector(const Vector3D<double>& v, double stepLen)
	{
		return Vector3D<double>(v.X + (getZeroOne() - 0.5) * stepLen, v.Y + (getZeroOne() - 0.5) * stepLen, v.Z + (getZeroOne() - 0.5) * stepLen);
	}

	inline Vector3D<double> getRandomOnUnitSphere()
	{
		return getRandomInUnitSphere().Normalize();
	}


	inline Vector3D<double> getRandomCosineDirection()
	{
		const double r1 = getZeroOne();
		const double r2 = getZeroOne();
		const double z = sqrt(1. - r2);
		const double phi = 2. * M_PI * r1;
		const double twosqrt = 2. * sqrt(r2);
		const double x = cos(phi) * twosqrt;
		const double y = sin(phi) * twosqrt; 

		return Vector3D<double>(x, y, z);
	}

	std::mt19937_64 rng;

protected:
	std::uniform_real_distribution<double> uniformZeroOne;	
	std::normal_distribution<double> normalDistribution;
};

