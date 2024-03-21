#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <unistd.h> //needed for getopt to work
#include "PageTable.h"
#include "vaddr_tracereader.h"
#include "log_helpers.h" 

int main(int argc, char** argv) {
    int num_frames = 999999; // Default number of frames, implying infinite frames/no page replacement needed
    int num_accesses = -1; // By default, process all addresses
    int bitstring_interval = 10; // Default bitstring update interval for NFU with aging
    std::string log_mode = "summary"; // Default logging mode
    std::vector<int> bitsPerLevel; // To store bits for each page table level

    // Parsing optional arguments
    int opt;
    while ((opt = getopt(argc, argv, "n:f:b:l:")) != -1) {
        switch (opt) {
            case 'n':
                num_accesses = std::stoi(optarg);
                if (num_accesses < 1) {
                    std::cerr << "Number of memory accesses must be a number and greater than 0\n"; // error handling according to a3.pdf
                    return EXIT_FAILURE;
                }
                break;
            case 'f':
                num_frames = std::stoi(optarg);
                if (num_frames < 1) {
                    std::cerr << "Number of available frames must be a number and greater than 0\n"; // error handling according to a3.pdf
                    return EXIT_FAILURE;
                }
                break;
            case 'b':
                bitstring_interval = std::stoi(optarg);
                if (bitstring_interval < 1) {
                    std::cerr << "Bit string update interval must be a number and greater than 0\n"; // error handling according to a3.pdf
                    return EXIT_FAILURE;
                }
                break;
            case 'l':
                log_mode = optarg;
                break;
            default:
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        std::cerr << "Trace file name and page table levels required.\n"; // error handling according to a3.pdf
        return EXIT_FAILURE;
    }

    // Mandatory argument: trace file name
    std::string trace_file_name = argv[optind++];

    // Opening trace file
    std::ifstream traceFile(trace_file_name);
    if (!traceFile.is_open()) {
        std::cerr << "Unable to open " << trace_file_name << "\n";  // error handling according to a3.pdf
        return EXIT_FAILURE;
    }

    // Mandatory arguments: bits for each page table level
    while (optind < argc) {
        int bits = std::stoi(argv[optind++]);
        if (bits < 1) {
            std::cerr << "Level " << bitsPerLevel.size() << " page table must have at least 1 bit\n";  // error handling according to a3.pdf
            return EXIT_FAILURE;
        }
        bitsPerLevel.push_back(bits);
    }

    int total_bits = std::accumulate(bitsPerLevel.begin(), bitsPerLevel.end(), 0);
    if (total_bits > 28) {
        std::cerr << "Too many bits used for the page table\n";  // error handling according to a3.pdf
        return EXIT_FAILURE;
    }


    // Initialize the PageTable object here with the parameters we just got
    //unsure abt some of the info within ur constructor??
   // PageTable pageTable(bitsPerLevel, num_frames);

    


    // Simulate processing of addresses from the trace file
    unsigned int vAddr;
    int accesses_processed = 0;
    while (traceFile >> std::hex >> vAddr && (num_accesses == -1 || accesses_processed < num_accesses)) {
        // calling on specific functions TODO
    //    if (!pageTable.searchMappedPfn(vAddr)) {
    //         // Page fault handling 
    //         pageTable.insertMapForVpn2Pfn(vAddr, accesses_processed % num_frames);
    //     }
        
       accesses_processed++;
    }

    traceFile.close();

    // Implement the logging based on the chosen log_mode using the functions provided in log_helpers.h/cpp
    

    return EXIT_SUCCESS;
}
