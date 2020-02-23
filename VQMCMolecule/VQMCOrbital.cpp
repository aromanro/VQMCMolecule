#include "VQMCOrbital.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

namespace Orbitals
{

	double VQMCOrbital::operator()(const Vector3D<double>& r) const
	{
		double val = m_orb1(r);

		if (!single)
		{
			if (minus)
				val -= m_orb2(r);
			else
				val += m_orb2(r);

			val *= M_SQRT1_2;
		}

		return val;
	}

	Vector3D<double> VQMCOrbital::getGradient(const Vector3D<double>& r) const
	{
		Vector3D<double> val = m_orb1.getGradient(r);

		if (!single)
		{
			if (minus)
				val -= m_orb2.getGradient(r);
			else
				val += m_orb2.getGradient(r);

			val *= M_SQRT1_2;
		}

		return val;
	}

	double VQMCOrbital::getLaplacian(const Vector3D<double>& r) const
	{
		double val = m_orb1.getLaplacian(r);

		if (!single)
		{
			if (minus)
				val -= m_orb2.getLaplacian(r);
			else
				val += m_orb2.getLaplacian(r);

			val *= M_SQRT1_2;
		}

		return val;
	}

}