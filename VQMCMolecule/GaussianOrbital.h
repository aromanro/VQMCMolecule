#pragma once

#include "Orbital.h"

namespace Orbitals {

	class GaussianOrbital : public Orbital
	{
	public:
		virtual double getCoefficient() const;
		virtual double getAlpha() const;

		double operator()(const Vector3D<double>& r) const override;

		Vector3D<double> getGradient(const Vector3D<double>& r) const override;
		double getLaplacian(const Vector3D<double>& r) const override;
		Vector3D<double> ProductCenter(const GaussianOrbital& other) const;
		void Normalize();

		double coefficient = 1;
		double alpha = 1;

		double normalizationFactor = 1;

	protected:
		double getNormalizationFactor() const;

		double coeffProdNorm = 0;
	};

}