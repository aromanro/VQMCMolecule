#pragma once

#include <string>
#include <vector>

#include "Atom.h"

namespace Chemistry {

	class Basis
	{
	public:
		std::vector<Systems::AtomWithShells> atoms;

		void Load(const std::string& fileName);
		void Save(const std::string& name);
		void Normalize();

	protected:
		void SaveAtom(std::ofstream& file, const Systems::AtomWithShells& atom);
		void SaveShell(std::ofstream& file, const Orbitals::ContractedGaussianShell& shell);
		void CountOrbitals(const Systems::AtomWithShells& atom, int& sNr, int& pNr, int& dNr, int& fNr, int& gNr, int& hNr);
		void CountContractedOrbitals(const Systems::AtomWithShells& atom, int& sNr, int& pNr, int& dNr, int& fNr, int& gNr, int& hNr);
	};

}