#include "Cache.h"
#include <iostream>
#include <algorithm>

Cache::Cache(uint16_t _L, uint16_t _K, uint16_t _N){
	this->L = _L;
	this->K = _K;
	this->N = _N;
	this->Lbm = (int)log2(L);
	this->Kbm = (int)log2(K);
	this->Nbm = (int)log2(N);
	cache.resize(N);	//N sets in cache
	for (auto &set : cache) {
		set.NodeArr.resize(K);
		for (int i = 0; i < K; i++) {
			Directory *newDir = new Directory();
			newDir->CachedBytes = 0x0;
			newDir->Tag = 0x0;
			set.pList.push_front(newDir);
			set.NodeArr[i] = set.pList.Front;
		}
	}
}


Cache::~Cache(){
}

Cache::CacheAnalysis Cache::Analyse(const std::vector<AddressRecord>& Records){
	uint32_t hits = 0, misses = 0, accesses = 0;
	for (auto &record : Records) {
		if ((record.word0 & 0x80000000) == 0 || (((record.word0 & 0x40000000) == 0) && ((record.word0 & 0x20000000) != 0)))
			continue;

		uint32_t word0Temp = record.word0;
		uint32_t PhyAddr = word0Temp & 0x7FFFFF;
		word0Temp >>= 27;
		uint8_t BurstCount = (word0Temp & 0x3) + 1;

		accesses += BurstCount + 1;
		auto hit = MemoryAccess(PhyAddr);

		hits += BurstCount + hit;
		misses += 1 - hit;

	}
	return { hits, misses, accesses };
}


uint8_t Cache::MemoryAccess(uint32_t PhyAddr){
	uint32_t TrueAddr = PhyAddr << 2;	//Last 2 bits not in trace
	TrueAddr >>= Lbm;
	uint32_t SetNumber = TrueAddr & ~((~0x0) << Nbm);	//Set
	TrueAddr >>= Nbm;
	uint32_t Tag = TrueAddr;		//Tag is what's left

	auto &Set = cache[SetNumber];
	auto &pList = Set.pList;

	for (auto &dir : Set.NodeArr) {
		auto data = dir->Data;
		if (Tag == data->Tag && 0 != data->CachedBytes) {	//hit
			pList.move_to_front(dir);
			return 1;
		}
	}

	pList.Back->Data->Tag = Tag;
	pList.Back->Data->CachedBytes = 0x1;
	pList.back_to_front();
	return 0;

}

