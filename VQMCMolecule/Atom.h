#pragma once

#include <string>
#include <algorithm>

#include "Vector3D.h"
#include "PrimitiveShell.h"

namespace Systems {

	class Atom
	{
	public:
		Vector3D<double> position;

		unsigned int Z;
		unsigned int electronsNumber;

		unsigned int ID;

		Atom(unsigned int nrZ = 0, int nrElectrons = -1);
		virtual ~Atom();
	};

	class AtomWithShells : public Atom
	{
	public:
		std::vector<Orbitals::ContractedGaussianShell> shells;
		unsigned int alphaElectrons;
		unsigned int betaElectrons;

		AtomWithShells(unsigned int nrZ = 0, unsigned int nrElectrons = -1) : Atom(nrZ, nrElectrons) 
		{
			// this is temporary, should be adjusted later when the molecule is initialized
			alphaElectrons = static_cast<unsigned int>(nrZ / 2);
			betaElectrons = (nrElectrons > -1 ? nrElectrons : nrZ) - alphaElectrons;
		}

		void AddShell(const std::string& name)
		{
			Orbitals::ContractedGaussianShell shell;

			for (auto c : name)	shell.AddOrbital(c);

			shells.push_back(std::move(shell));
		}

		void SetCenterForShells()
		{
			for (auto &shell : shells) shell.SetCenters(position);
		}

		void SetPosition(const Vector3D<double>& pos)
		{
			position = pos;
			SetCenterForShells();
		}

		unsigned int CountNumberOfContractedGaussians() const;
		unsigned int CountNumberOfGaussians() const;

		void GetMaxQN(double alpha, Orbitals::QuantumNumbers::QuantumNumbers& maxQN) const
		{
			maxQN.l = maxQN.m = maxQN.n = 0;

			for (const auto &shell : shells)
				for (const auto &orbital : shell.basisFunctions)
					for (const auto &gaussian : orbital.gaussianOrbitals)
						if (alpha == gaussian.alpha)
						{
							maxQN.l = std::max(maxQN.l, orbital.angularMomentum.l);
							maxQN.m = std::max(maxQN.m, orbital.angularMomentum.m);
							maxQN.n = std::max(maxQN.m, orbital.angularMomentum.n);
						}
		}

		unsigned int GetMaxAngularMomentum() const
		{
			unsigned int L = 0;

			for (const auto &shell : shells)
				for (const auto &orbital : shell.basisFunctions)
					L = std::max(L, (unsigned int)orbital.angularMomentum);

			return L;
		}

		unsigned int GetMaxAngularMomentum(double alpha) const
		{
			unsigned int L = 0;

			for (const auto &shell : shells)
				for (const auto &orbital : shell.basisFunctions)
					for (const auto &gaussian : orbital.gaussianOrbitals)
						if (alpha == gaussian.alpha)
					         L = std::max(L, (unsigned int)orbital.angularMomentum);

			return L;
		}

		void Normalize()
		{
			for (auto &shell : shells) shell.Normalize();
		}
	};

}