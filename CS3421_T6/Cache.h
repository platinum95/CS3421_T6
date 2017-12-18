#pragma once
#include <stdint.h>
#include <vector>
#include "PetesList.h"

//Defines the data structure
// of a given trace record
struct AddressRecord {
	uint32_t word0;
	uint32_t word1;
};

class Cache{
private:
	struct Directory {
		uint16_t Tag{ 0 };
		//#Cached bytes is not important, as we're not
		// actually caching any data.
		// Instead, this is 0 if the directory has not yet been used
		// or 1 if it has
		uint8_t CachedBytes{ 0 };
	};
	struct Set {
		//Linked list of directories in a set
		PetesList<Directory*> DirectoryList;

		//Vector of directory nodes too, for faster searching
		std::vector<PetesList<Directory*>::PetesNode*> NodeVector;
		PetesList<Directory*>::PetesNode* *NodeArray;
	};

	//Vector of the cache sets
	std::vector<Set> CacheSetVector;

	//Pointer that is set to the raw array of the above vector
	Set *CacheSets;

	uint16_t L, K, N;		//Number of cached bytes, directories, and sets
	uint8_t Lb, Kb, Nb;		//The number of bits required to express each of the above
	uint16_t Lbm, Kbm, Nbm;	//The bitmasks required to compute the above

public:
	Cache(uint16_t _L, uint16_t _K, uint16_t _N);
	~Cache();

	uint8_t MemoryAccess(uint32_t PhyAddr);
	
};

