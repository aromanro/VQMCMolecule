#pragma once

#define _USE_MATH_DEFINES // for C++  
#include <cmath>  

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

#include <array>

#include "QuantumNumbers.h"
#include "Vector3D.h"

class MathUtils
{
private:
	static std::array<unsigned long long int, 21> factorialsTable;
	static std::array<unsigned long long int, 21> doubleFactorialsTable;

public:
	static double Factorial(long int n)
	{
		// using precalculated (at compile time) values
		if (n < 0) return 1;
		else if (n < 21) return static_cast<double>(factorialsTable[n]);

		double val = static_cast<double>(factorialsTable[20]);
		for (long int i = 21; i <= n; ++i)
			val *= i;

		return val;
	}

	static double DoubleFactorial(long int n)
	{
		if (n < 0) return 1;
		else if (n < 21) return static_cast<double>(doubleFactorialsTable[n]);

		double res = 1;
		for (; n > 20; n -= 2)
			res *= n;

		return res * static_cast<double>(doubleFactorialsTable[n]);
	}

	static unsigned int BinomialCoefficient(unsigned int n, unsigned int k)
	{
		//return Factorial(n) / (Factorial(k) * Factorial(n-k));

		unsigned int numerator = 1;
		unsigned int denominator = 1;

		for (unsigned int i = 1; i <= k; ++i)
		{
			numerator *= n - i + 1;
			denominator *= i;
		}

		return numerator / denominator;
	}


private:
	static double Abscissa(int n, int i)
	{
		const double val = i * M_PI / (1. + n);
		const double sinVal = sin(val);
		const double cosVal = cos(val);

		return (n + 1. - 2. * i) / (n + 1.) +
			2. / M_PI * (1. + 2. / 3. * sinVal * sinVal) * cosVal * sinVal;
	}


	static double Omega(int n, int i)
	{
		const double sinVal = sin(i * M_PI / (n + 1.));

		return 16. / (3 * (1. + n)) * sinVal * sinVal * sinVal * sinVal;
	}

public:

	class FunctionFunctor {
	public:
		virtual double operator()(double x) const = 0;
	};



};

