#pragma once


#include "Wavefunction.h"

class VQMC
{
public:
	VQMC(int addseed = 0, double deltat = 0.005) 
		: dt(deltat)
	{
		sdt = sqrt(dt);
	}

	void SetDeltat(double deltat)
	{
		dt = deltat;
		sdt = sqrt(dt);
	}

	std::tuple<double, double, double> SamplingFokkerPlanck(int thermalSteps, int cycles, double beta);

	void Init(const Systems::Molecule& mol, int seed);

	Systems::Molecule molecule;

protected:
	double dt;
	double sdt;

	Random random;
	Wavefunction wavefunction;
};

