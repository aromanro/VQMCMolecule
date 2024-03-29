#include "Basis.h"

#include "ChemUtils.h"
#include "QuantumNumbers.h"

#include <fstream>
#include <regex>

#include <iostream>
#include <sstream>

namespace Chemistry {



	void Basis::Load(const std::string& fileName)
	{
		std::ifstream file(fileName);

		if (!file) return;

		atoms.clear();

		std::string line;
		std::regex ignore("^\\s*(#|BASIS|END).*$|^\\s*$");
		std::regex number("[+-]?[0-9]*\\.?[0-9]+((E|D)[+-][0-9]+)?");

		std::string shellName;

		while (std::getline(file, line))
		{
			if (std::regex_match(line, ignore)) continue;
			else if (!LoadShell(line, shellName))
			{
				std::sregex_iterator it(line.begin(), line.end(), number);
				std::sregex_iterator end;

				bool first = true;
				int orbital = 0;
				int totalGaussians = 0;

				Orbitals::ContractedGaussianShell& contractedShell = atoms.back().shells.back();

				for (; it != end; ++it)
				{
					std::string strNr = it->str();
					std::replace(strNr.begin(), strNr.end(), 'D', 'E');

					//TRACE("%s\n", strNr.c_str());

					const double value = std::stod(strNr);

					if (first)
					{
						first = false;

						// the first value in the line is the exponent

						// add a new gaussian to each contracted gaussian in the shell and set its exponent
						// for example, for a STOnG basis set there will be n lines with values following the 'atom orbitals' line
						// here the gaussians are added for a particular line
						contractedShell.AddGaussians(value);
						// in the end for each 'shell' such as 'SP' we'll end up with a shell object
						// containing 4 ContractedGaussianOrbital, 1 for 'S', 3 for 'P'
						// for a STOnG basis set, each ContractedGaussianOrbital will contain n gaussians
					}
					else
					{
						// the next values in the line are the coefficients
						// one for each letter, for example for 'SP', one for S, one for P

						int numberOfGaussiansInOrbital = GetNumberOfGaussiansInOrbital(shellName, orbital);

						for (int i = 0; i < numberOfGaussiansInOrbital; ++i)
						{
							Orbitals::ContractedGaussianOrbital& contractedOrbital = contractedShell.basisFunctions[static_cast<long long int>(totalGaussians) + i];

							contractedOrbital.gaussianOrbitals.back().coefficient = value;
							contractedOrbital.gaussianOrbitals.back().Normalize();
						}

						totalGaussians += numberOfGaussiansInOrbital;

						++orbital; // each value in the line (except the first, which is the exponent) corresponds to one orbital, for exemple for 'S' and 'P' for "SP"
					}
				}
			}
		}
	}

	int Basis::GetNumberOfGaussiansInOrbital(const std::string& shellName, int orbital)
	{
		int numberOfGaussiansInOrbital = 1;
		switch (tolower(shellName.at(orbital)))
		{
		case 's':
			numberOfGaussiansInOrbital = 1;
			break;
		case 'p':
			numberOfGaussiansInOrbital = Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(1);
			break;
		case 'd':
			numberOfGaussiansInOrbital = Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(2);
			break;
		case 'f':
			numberOfGaussiansInOrbital = Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(3);
			break;
		case 'g':
			numberOfGaussiansInOrbital = Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(4);
			break;
		case 'h':
			numberOfGaussiansInOrbital = Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(5);
			break;
		}

		return numberOfGaussiansInOrbital;
	}


	bool Basis::LoadShell(std::string& line, std::string& shellName)
	{
		std::regex shell("^(\\w+)\\s+(\\w+)\\s*$");
		std::smatch match;

		if (std::regex_match(line, match, shell))
		{
			const unsigned int Z = ChemUtils::GetZForAtom(match[1].str().c_str());

			assert(Z > 0);

			//if (Z <= 0) AfxMessageBox(L"Unknown atom!");

			// it's a new atom
			if (!atoms.size() || atoms.back().Z != Z)
			{
				Systems::AtomWithShells atom(Z);
				atoms.emplace_back(std::move(atom));
			}

			shellName = match[2].str();

			//TRACE("Atom: %s\n", match[1].str().c_str());
			//TRACE("Shell: %s\n", shellName.c_str());

			// this doesn't simply add the shell object, it adds the orbitals in it as well
			atoms.back().AddShell(shellName);

			return true;
		}

		return false;
	}


	// just for tests
	void Basis::Save(const std::string& fileName)
	{
		std::ofstream file(fileName);

		file.precision(10);

		file << "BASIS \"ao basis\" PRINT" << std::endl;

		for (const auto &atom : atoms)
		{
			SaveAtom(file, atom);
		}

		file << "END" << std::endl;
	}

	void Basis::SaveAtom(std::ofstream& file, const Systems::AtomWithShells& atom)
	{
		file << "#BASIS SET: (";

		int sNr = 0;
		int pNr = 0;
		int dNr = 0;
		int fNr = 0;
		int gNr = 0;
		int hNr = 0;

		CountOrbitals(atom, sNr, pNr, dNr, fNr, gNr, hNr);

		std::stringstream orbitalsStr;

		GenerateOrbitalsString(orbitalsStr, sNr, pNr, dNr, fNr, gNr, hNr);

		file << orbitalsStr.str() << ") -> [";

		sNr = 0;
		pNr = 0;
		dNr = 0;
		fNr = 0;
		gNr = 0;
		hNr = 0;

		CountContractedOrbitals(atom, sNr, pNr, dNr, fNr, gNr, hNr);

		orbitalsStr.seekp(0);

		GenerateOrbitalsString(orbitalsStr, sNr, pNr, dNr, fNr, gNr, hNr);

		file << orbitalsStr.str() << "]" << std::endl;

		for (const auto& shell : atom.shells)
		{
			file << ChemUtils::GetAtomNameForZ(atom.Z) << "    " << shell.GetShellString() << std::endl;

			SaveShell(file, shell);
		}
	}

	void Basis::CountOrbitals(const Systems::AtomWithShells& atom, int& sNr, int& pNr, int& dNr, int& fNr, int& gNr, int& hNr)
	{
		for (const auto& shell : atom.shells)
		{
			sNr += shell.CountOrbitals('s');
			pNr += shell.CountOrbitals('p');
			dNr += shell.CountOrbitals('d');
			fNr += shell.CountOrbitals('f');
			gNr += shell.CountOrbitals('g');
			hNr += shell.CountOrbitals('h');
		}
	}

	void Basis::CountContractedOrbitals(const Systems::AtomWithShells& atom, int& sNr, int& pNr, int& dNr, int& fNr, int& gNr, int& hNr)
	{
		for (const auto& shell : atom.shells)
		{
			sNr += shell.CountContractedOrbitals('s');
			pNr += shell.CountContractedOrbitals('p');
			dNr += shell.CountContractedOrbitals('d');
			fNr += shell.CountContractedOrbitals('f');
			gNr += shell.CountContractedOrbitals('g');
			hNr += shell.CountContractedOrbitals('h');
		}
	}

	void Basis::GenerateOrbitalsString(std::stringstream& orbitalsStr, int sNr, int pNr, int dNr, int fNr, int gNr, int hNr)
	{
		bool addComma = false;
		if (sNr > 0) {
			orbitalsStr << sNr << 's';
			addComma = true;
		}
		if (pNr > 0) {
			if (addComma) orbitalsStr << ',';
			orbitalsStr << pNr << 'p';
		}
		if (dNr > 0) {
			if (addComma) orbitalsStr << ',';
			orbitalsStr << dNr << 'd';
		}
		if (fNr > 0) {
			if (addComma) orbitalsStr << ',';
			orbitalsStr << fNr << 'f';
		}
		if (gNr > 0) {
			if (addComma) orbitalsStr << ',';
			orbitalsStr << gNr << 'g';
		}
		if (hNr > 0) {
			if (addComma) orbitalsStr << ',';
			orbitalsStr << hNr << 'h';
		}
	}



	void Basis::SaveShell(std::ofstream& file, const Orbitals::ContractedGaussianShell& shell)
	{
		for (int i = 0; i < static_cast<int>(shell.basisFunctions.front().gaussianOrbitals.size()); ++i)
		{
			bool first = true;
			for (int j = 0; j < static_cast<int>(shell.basisFunctions.size());)
			{
				auto& orbital = shell.basisFunctions[j];

				file << "      ";
				if (first)
				{
					first = false;
					file << orbital.gaussianOrbitals[i].alpha << "      ";
				}

				file << orbital.gaussianOrbitals[i].coefficient;

				switch (orbital.gaussianOrbitals[i].AtomicOrbital())
				{
				default:
				case 's':
					++j;
					break;
				case 'p':
					j += Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(1);
					break;
				case 'd':
					j += Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(2);
					break;
				case 'f':
					j += Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(3);
					break;
				case 'g':
					j += Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(4);
					break;
				case 'h':
					j += Orbitals::QuantumNumbers::QuantumNumbers::NumOrbitals(5);
					break;
				}
			}

			file << std::endl;
		}
	}

}

void Chemistry::Basis::Normalize()
{
	// here just in case, it should be normalized when loading!
	for (auto& atom : atoms) atom.Normalize();
}
