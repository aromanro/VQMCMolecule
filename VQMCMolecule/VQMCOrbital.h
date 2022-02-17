#pragma once

#include "Orbital.h"
#include "ContractedGaussianOrbital.h"

namespace Orbitals
{

	class VQMCOrbital :
		public OrbitalBase
	{
	public:
		VQMCOrbital(const ContractedGaussianOrbital& orb)
			: single(true), minus(false), norm(1.)
		{
			m_orb1 = orb;			
		}

		VQMCOrbital(const ContractedGaussianOrbital& orb1, const ContractedGaussianOrbital& orb2, double Overlap, bool m)
			: single(false), minus(m), norm(sqrt(2. * (m ? 1. - Overlap : 1. + Overlap)))
		{
			m_orb1 = orb1;
			m_orb2 = orb2;
		}

		double operator()(const Vector3D<double>& r) const override;

		Vector3D<double> getGradient(const Vector3D<double>& r) const override;
		double getLaplacian(const Vector3D<double>& r) const override;

	protected:
		ContractedGaussianOrbital m_orb1;
		ContractedGaussianOrbital m_orb2;
		bool single;
		bool minus;

		double norm;
	};

}