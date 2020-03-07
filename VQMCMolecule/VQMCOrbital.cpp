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

			// the problem is that the combined orbitals are not orthogonal
			// they come from different atoms (from the last shell, the valence one)
			// so they have overlap
			// so you cannot simply normalize the combination by 1 / sqrt(2)
			// you have to take into account the cross terms, too
			// that could be done by using the computation of the overlap integrals from the Hartree-Fock project
			// but I won't bother, I'll just make the 'average' between them and leave it like that

			// for the whole wave function the normalization constant doesn't matter, because it cancels out in the formulae
			// that's why you don't see the 1 / sqrt(N!) factor for the Slater determinant in the code, for example
			// as for an individual component, keep in mind that we're trying to have a trial wavefunction as good as possible
			// and that's it
			// it's just a trial function - ideally it would be the one we're looking for, but if we would know it, we wouldn't have to do the computation :)

			val *= 0.5; //M_SQRT1_2;
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

			val *= 0.5; // M_SQRT1_2;
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

			val *= 0.5; // M_SQRT1_2;
		}

		return val;
	}

}