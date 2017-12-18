#include "Cache.h"
#include <iostream>

Cache::Cache(uint16_t _L, uint16_t _K, uint16_t _N){
	this->L = _L;
	this->N = _N;
	this->K = _K;

	this->Lb = (int)log2(L);
	this->Nb = (int)log2(N);
	this->Kb = 32 - Lb - Nb;	//Tag bits are what's left
	
	//Compute the bitmasks for each of the above.
	//i.e for Lb = 1 ... 
	//~0x0 = 1111 1111 ..<<.. 1111 1110 ..~.. = 0000 0001
	//This is not relative to the whole address
	//and requires the bits to be right-aligned
	this->Lbm = ~((~0x0) << Lb);
	this->Kbm = ~((~0x0) << Kb);
	this->Nbm = ~((~0x0) << Nb);

	//Set up the cache
	CacheSetVector.resize(N);	//N sets in cache
	for (auto &set : CacheSetVector) {
		set.NodeVector.resize(K);	//K Directories in set
		for (int i = 0; i < K; i++) {
			//Add new directory to linked list
			Directory *newDir = new Directory();
			newDir->CachedBytes = 0x0;
			newDir->Tag = 0x0;
			set.DirectoryList.push_front(newDir);
			//Log the new list node in contiguous vector
			set.NodeVector[i] = set.DirectoryList.Front;
		}
		//Store the raw vector array.
		set.NodeArray = set.NodeVector.data();
	}
	//Store the raw set array 
	CacheSets = CacheSetVector.data();
}


Cache::~Cache(){
	for (auto &set : CacheSetVector) {
		for (auto &node : set.NodeVector) {
			delete node->Data;
		}
	}
}

uint8_t Cache::MemoryAccess(uint32_t PhyAddr){

	//Ignore the offset line. Take into account
	// that the last 2 bits of the address are not stored
	uint32_t TrueAddr = PhyAddr >> (Lb - 2);
	uint32_t SetNumber = TrueAddr & Nbm;
	TrueAddr >>= Nb;
	uint32_t Tag = TrueAddr;	//Tag is what's left

	//Get corresponding set from the raw set array
	auto &Set = CacheSets[SetNumber];
	auto &pList = Set.DirectoryList;
	
	//Check if the data required is in the cache
	for (int i = 0; i < K; i++) {
		auto &dir = Set.NodeArray[i];
		auto data = dir->Data;
		if (Tag == data->Tag && 0 != data->CachedBytes) {	//hit
			pList.move_to_front(dir);
			return 1;
		}
	}	
	//If not in cache, replace the LRU directory
	pList.Back->Data->Tag = Tag;
	pList.Back->Data->CachedBytes = 0x1;
	pList.back_to_front();
	return 0;

}

