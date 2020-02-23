#pragma once
#include <vector>
#include <Eigen\eigen>

#include "Vector3D.h"
#include "Random.h"
#include "Molecule.h"
#include "VQMCOrbital.h"

class Wavefunction
{
public:
	Wavefunction() : m_molecule(nullptr) {}

	void Init(const Systems::Molecule& molecule, Random& random, double radiusInit = 2);

	void ComputeSlaterInv();

	inline bool IsSpinUp(unsigned int numParticle) const
	{
		return numParticle < m_molecule->alphaElectrons;
	}

	inline static double Alpha(bool Spin1Up, bool Spin2Up)
	{
		return Spin1Up == Spin2Up ? 0.25 : 0.5; // electrons with the same spin orientation avoid more than the ones with opposed spins
	}

	void UpdateInverses(const Vector3D<double>& newPos, unsigned int p, double ratio);

	double SlaterRatio(const Vector3D<double>& newPos, unsigned int p) const;
	double JastrowRatio(const Vector3D<double>& newPos, unsigned int p, double beta) const;

	Vector3D<double> HalfQuantumForce(const Vector3D<double>& newPos, unsigned int p, double beta, double ratio = 1.) const;

	double LocalKineticEnergy(double beta) const;
	double LocalEnergy(double beta) const;

	double log_deriv_beta_wf(double beta) const;

	std::vector<Vector3D<double>> currentParticles;

protected:
	std::vector<Orbitals::VQMCOrbital> orbitals;

	Eigen::MatrixXd SpinUpInvSlater;
	Eigen::MatrixXd SpinDownInvSlater;

	const Systems::Molecule* m_molecule;
};

