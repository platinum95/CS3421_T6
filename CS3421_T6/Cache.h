#pragma once
#include <stdint.h>
#include <vector>

struct AddressRecord {
	uint32_t word0;
	uint32_t word1;
};

class Cache
{
private:
	struct Directory {
		uint16_t Tag;
	};
	struct CacheLine {
		std::vector<uint8_t> CachedBytes;
	};
	struct Set {
		std::vector<Directory> Tags;
		std::vector<CacheLine> Data;
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
	uint16_t L, K, N;
};

