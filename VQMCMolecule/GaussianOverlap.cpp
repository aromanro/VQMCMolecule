#include "GaussianOverlap.h"

#include "MathUtils.h"


namespace GaussianIntegrals {

	GaussianOverlap::GaussianOverlap()
		: factor(0)
	{
	}

	GaussianOverlap::GaussianOverlap(double alpha1, double alpha2, const Vector3D<double>& center1, const Vector3D<double>& center2, const Orbitals::QuantumNumbers::QuantumNumbers& maxQN1, const Orbitals::QuantumNumbers::QuantumNumbers& maxQN2)
		: factor(0)
	{
		Reset(alpha1, alpha2, center1, center2, maxQN1, maxQN2);
	}


	void GaussianOverlap::Reset(double alpha1, double alpha2, const Vector3D<double>& center1, const Vector3D<double>& center2, const Orbitals::QuantumNumbers::QuantumNumbers& maxQN1, const Orbitals::QuantumNumbers::QuantumNumbers& maxQN2)
	{
		matrixX = Eigen::MatrixXd::Zero(2ULL + maxQN1.l + maxQN2.l, maxQN2.l + 1ULL);
		matrixY = Eigen::MatrixXd::Zero(2ULL + maxQN1.m + maxQN2.m, maxQN2.m + 1ULL);
		matrixZ = Eigen::MatrixXd::Zero(2ULL + maxQN1.n + maxQN2.n, maxQN2.n + 1ULL);

		CalculateOverlap(matrixX, alpha1, alpha2, center1.X, center2.X, maxQN1.l, maxQN2.l);
		CalculateOverlap(matrixY, alpha1, alpha2, center1.Y, center2.Y, maxQN1.m, maxQN2.m);
		CalculateOverlap(matrixZ, alpha1, alpha2, center1.Z, center2.Z, maxQN1.n, maxQN2.n);

		const Vector3D<double> dif = center1 - center2;
		factor = exp(-alpha1 * alpha2 / (alpha1 + alpha2) * dif * dif) * pow(M_PI / (alpha1 + alpha2), 3. / 2.);
	}

	double GaussianOverlap::getOverlap(const Orbitals::QuantumNumbers::QuantumNumbers& QN1, const Orbitals::QuantumNumbers::QuantumNumbers& QN2) const
	{
		return factor * matrixX(QN1.l, QN2.l) * matrixY(QN1.m, QN2.m) * matrixZ(QN1.n, QN2.n);
	}

	void GaussianOverlap::CalculateOverlap(Eigen::MatrixXd& matrix, double alpha1, double alpha2, double center1, double center2, unsigned int maxQN1, unsigned int maxQN2)
	{
		const double alpha = alpha1 + alpha2;
		const double productCenter = (alpha1 * center1 + alpha2 * center2) / alpha;
		const double dif = center1 - center2;
		const double difCenter = productCenter - center1;

		matrix(0, 0) = 1;
		matrix(1, 0) = difCenter;

		// recurrence index
		unsigned int limit = maxQN1 + maxQN2 + 1;

		for (unsigned int i = 2; i <= limit; ++i)
			matrix(i, 0) = difCenter * matrix(i - 1, 0) + (i - 1.) / (2. * alpha) * matrix(i - 2, 0);

		// transfer equation
		--limit;
		for (unsigned int j = 1; j <= maxQN2; ++j, --limit)
		{
			for (unsigned int i = 0; i <= limit; ++i)
				matrix(i, j) = matrix(i + 1ULL, j - 1ULL) + dif * matrix(i, j - 1ULL);
		}

		matrix = matrix.block(0, 0, maxQN1 + 1ULL, maxQN2 + 1ULL).eval();
	}

}