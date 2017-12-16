#include "Cache.h"



Cache::Cache(uint16_t _L, uint16_t _K, uint16_t _N){
	this->L = _L;
	this->K = _K;
	this->N = _N;
	cache.resize(N);	//N sets in cache
	for (auto set : cache) {
		set.Tags.resize(K, { 0x0 });	//K Tags, initialised to 0
		set.Data.resize(K);				//K Data blocks
		for (auto dataBlock : set.Data) {
			dataBlock.CachedBytes.resize(L, 0x0);	//L bytes per cache line, initialised to 0x0
		}
	}
}


Cache::~Cache(){
}

Cache::CacheAnalysis Cache::Analyse(const std::vector<AddressRecord>& Records){
	uint16_t hits = 0, misses = 0, accesses = 0;
	for (auto record : Records) {
		uint32_t word0Temp = record.word0;
		uint32_t PhyAddr = word0Temp & 0x7FFFFF;
		word0Temp >>= 23;
		uint8_t BusEnableSig = word0Temp & 0xF;
		word0Temp >>= 4;
		uint8_t BurstCount = (word0Temp & 0x3) + 1;
		word0Temp >> 2;
		bool write_read = word0Temp & 0x1;
		word0Temp >> 1;
		bool data_control = word0Temp & 0x1;
		word0Temp >> 1;
		bool mem_io = word0Temp & 0x1;
		uint8_t IntervalCount = record.word1 & 0xFF;
		uint8_t BusCycleType = (mem_io << 2) | (data_control << 1) | (write_read);
		if (BusCycleType == 0x0 || BusCycleType == 0x1 || BusCycleType == 0x5) {
			continue;
		}
		accesses++;

	}
	return { hits, misses, accesses };
}
