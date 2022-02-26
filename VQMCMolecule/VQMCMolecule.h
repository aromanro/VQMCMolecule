#pragma once

#include "Options.h"

class VQMCMolecule
{
public:
	static void Compute(const Options& opt, const Chemistry::Basis& basisSTOXG);

protected:
	static void InitMolecule(const Options& opt, const Chemistry::Basis& basisSTOXG, Systems::AtomWithShells& atom1, Systems::AtomWithShells& atom2, Systems::Molecule& molecule);
	static void InitWalkers(std::vector<VQMC>& vqmcWalkers, Systems::Molecule& molecule, double deltat);
};

