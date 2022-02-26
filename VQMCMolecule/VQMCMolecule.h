#pragma once

#include <future>

#include "Options.h"
#include "Basis.h"
#include "Molecule.h"
#include "VQMC.h"

class VQMCMolecule
{
public:
	static void Compute(const Options& opt, const Chemistry::Basis& basisSTOXG);

protected:
	static void InitMolecule(const Options& opt, const Chemistry::Basis& basisSTOXG, Systems::AtomWithShells& atom1, Systems::AtomWithShells& atom2, Systems::Molecule& molecule);
	static void InitWalkers(std::vector<VQMC>& vqmcWalkers, Systems::Molecule& molecule, double deltat);
	static double GetValuesFromThreads(std::vector<std::future<std::tuple<double, double, double>>>& tasks, const unsigned int NrE, int cycleSteps, int nrWalkers, int i, double nuclearRepulsionEnergy, double beta);
};

