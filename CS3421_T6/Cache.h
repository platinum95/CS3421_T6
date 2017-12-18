#pragma once
#include <stdint.h>
#include <vector>
#include <list>
#include <unordered_map>
#include "PetesList.h"
#include <thread>

struct AddressRecord {
	uint32_t word0;
	uint32_t word1;
};

class Cache
{
private:
	struct Directory {
		uint16_t Tag;
		uint8_t CachedBytes;
	};
	struct Set {
		PetesList<Directory*> pList;
		std::vector<PetesList<Directory*>::PetesNode*> NodeArr;

	};
	std::vector<Set> cache;

public:
	struct CacheAnalysis {
		uint32_t Hits, Misses, Accesses;
	};
	Cache(uint16_t _L, uint16_t _K, uint16_t _N);
	~Cache();

	CacheAnalysis Analyse(const std::vector<AddressRecord> &Records);
private:
	uint8_t MemoryAccess(uint32_t PhyAddr);
	uint16_t L, K, N;
	uint8_t Lbm, Kbm, Nbm;
};

