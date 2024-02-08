
#define _QUANTUM_NUMBERS_IMPL

#include "QuantumNumbers.h"

namespace Orbitals {
	namespace QuantumNumbers {

		QuantumNumbers::QuantumNumbers(unsigned int L, unsigned int M, unsigned int N)
			: l(L), m(M), n(N)
		{
		}

	}
}

char Orbitals::QuantumNumbers::QuantumNumbers::AtomicOrbital() const
{
	switch (AngularMomentum())
	{
		case 0: return 's';
		case 1: return 'p';
		case 2: return 'd';
		case 3: return 'f';
		case 4: return 'g';
		case 5: return 'h';
		default: break;
	}

	return -1;
}

