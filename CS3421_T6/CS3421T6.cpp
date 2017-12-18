// CS3421T6.cpp - Simulation and analysis of LKN caches
//

#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "Cache.h"
#include <chrono>

#define TRACE_FILE "./gcc.trace"

std::vector<AddressRecord> LoadTracefile(std::string _Path);

//Allow for easier analysis printing
std::ostream& operator<<(std::ostream& os, const Cache::CacheAnalysis &data) {
	return os	<< "Acceses: " << data.Accesses << ". Hits: " << data.Hits 
				<< ". Misses: " << data.Misses << ". Hit rate: " 
				<< (float)data.Hits / (float)data.Accesses;
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
	//Create the 2 required caches
	Cache L16_K1_N1024{ 16, 1, 1024 };
	Cache L16_K8_N256{ 16, 8, 256 };
	
	//Start the timer and analyse the two cache performances
	auto start = std::chrono::high_resolution_clock::now();
	auto resultsA = L16_K1_N1024.Analyse(records);
	auto resultsB = L16_K8_N256.Analyse(records);
	
	//Get the execution time
	auto end = std::chrono::high_resolution_clock::now();
	auto ExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//Print out the results
	std::cout << resultsA << std::endl << resultsB << std::endl;
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