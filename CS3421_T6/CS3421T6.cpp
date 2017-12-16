// CS3421T6.cpp : Defines the entry point for the console application.
//

#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "Cache.h"

#define TRACE_FILE "./gcc.trace"


const std::vector<AddressRecord> LoadTracefile(std::string _Path);

std::ostream& operator<<(std::ostream& os, const Cache::CacheAnalysis &data) {
	return os << "Acceses: " << data.Accesses << ". Hits: " << data.Hits << ". Misses: " << data.Misses << ". Hit rate: " << (float)data.Hits / (float)data.Accesses;
}

int main(){
	auto records = LoadTracefile(TRACE_FILE);

	Cache L16_K1_N1024{ 16, 1, 1024 };
	Cache L16_K8_N256{ 16, 8, 256 };
	
	auto resultsA = L16_K1_N1024.Analyse(records);
	auto resultsB = L16_K8_N256.Analyse(records);
	std::cout << resultsA << std::endl << resultsB << std::endl;

    return 0;
}

const std::vector<AddressRecord> LoadTracefile(std::string _Path) {
	using namespace std;
	ifstream traceFile(_Path, ios::in | ios::binary | ios::ate);
	auto fileSize = traceFile.tellg();
	traceFile.seekg(0, ios::beg);
	vector<AddressRecord> records(fileSize);
	if (!traceFile.read((char*)records.data(), fileSize)) {
		throw std::exception("Error loading trace file");
	}
	return records;
}