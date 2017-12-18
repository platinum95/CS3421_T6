// CS3421T6.cpp - Simulation and analysis of LKN caches
//

#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "Cache.h"
#include <chrono>

#define TRACE_FILE "./gcc.trace"

struct CacheAnalysis {
	uint32_t DataHits, DataMisses, DataAccesses;
	uint32_t InstrHits, InstrMisses, InstrAccesses;
};

std::vector<AddressRecord> LoadTracefile(std::string _Path);

//Function called to actually analyse the cache on a given data set
CacheAnalysis Analyse(const std::vector<AddressRecord> &Records);

//Allow for easier analysis printing
std::ostream& operator<<(std::ostream& os, const CacheAnalysis &data) {
	return os	<< "Data:\nAccesses: " << data.DataAccesses << ". Hits: " << data.DataHits
				<< ". Misses: " << data.DataMisses << ". Hit rate: "
				<< (float)data.DataHits / (float)data.DataAccesses << std::endl << std::endl
				<< "Instruction:\nAccesses: " << data.InstrAccesses << ". Hits: " << data.InstrHits
				<< ". Misses: " << data.InstrMisses << ". Hit rate: "
				<< (float)data.InstrHits / (float)data.InstrAccesses << std::endl;
}

int main(){
	//Catch any error with the file loading
	std::vector<AddressRecord> records;
	try {
		records = LoadTracefile(TRACE_FILE);
	}
	catch (std::exception &e) {
		std::cout << e.what();
		return 1;
	}
	
	//Start the timer and analyse the cache performance
	auto start = std::chrono::high_resolution_clock::now();
	auto results = Analyse(records);
	auto end = std::chrono::high_resolution_clock::now();

	//Get the execution time
	auto ExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//Print out the results
	std::cout << results << std::endl;
	std::cout << "Took " << ExecTime << " ms" << std::endl;
	
    return 0;
}

std::vector<AddressRecord> LoadTracefile(std::string _Path) {
	using namespace std;
	ifstream traceFile(_Path, ios::in | ios::binary | ios::ate);
	if (!traceFile) {
		throw std::exception("Error: Trace file does not exist!");
	}
	auto fileSize = traceFile.tellg();
	traceFile.seekg(0, ios::beg);

	//Get number of records in the file
	unsigned int RecordCount = fileSize / sizeof(AddressRecord);
	vector<AddressRecord> records(RecordCount);

	//Read the entire file into a vector
	if (!traceFile.read((char*)records.data(), fileSize)) {
		throw std::exception("Error loading trace file!");
	}
	traceFile.close();
	return records;		//RVO should move the vector rather than copy
}


CacheAnalysis Analyse(const std::vector<AddressRecord>& Records) {
	//Create the 2 required caches
	Cache InstrCache{ 16, 1, 1024 };
	Cache DataCache{ 16, 8, 256 };

	uint32_t dhits = 0, dmisses = 0, daccesses = 0;
	uint32_t mhits = 0, mmisses = 0, maccesses = 0;
	//Get pointer to vector data for speed
	auto RecordData = Records.data();
	auto RecordCount = Records.size();

	for (int i = 0; i < RecordCount; i++) {
		auto record = RecordData[i];
		//Ignore all but data R/W and instruction R
		uint32_t d_c = record.word0 & 0x40000000;
		if ((record.word0 & 0x80000000) == 0 || (((d_c) == 0) && ((record.word0 & 0x20000000) != 0)))
			continue;

		//Get the Physical address and burst count
		uint32_t word0Temp = record.word0;
		uint32_t PhyAddr = word0Temp & 0x7FFFFF;
		word0Temp >>= 27;
		uint8_t BurstCount = (word0Temp & 0x3) + 1;

		if (d_c == 0) {	//Instruction read
			maccesses += BurstCount + 1;
			auto hit = InstrCache.MemoryAccess(PhyAddr);
			mhits += BurstCount + hit;
			mmisses += 1 - hit;
		}
		else {			//Data read/write
			daccesses += BurstCount + 1;
			auto hit = DataCache.MemoryAccess(PhyAddr);
			dhits += BurstCount + hit;
			dmisses += 1 - hit;
		}
		continue;
	}
	return { dhits, dmisses, daccesses, mhits, mmisses, maccesses };
}
