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
    //DEFAULT VARIABLES
    int num_frames = 999999; // Default number of frames, implying infinite frames/no page replacement needed
    int num_accesses = -1; // By default, process all addresses
    int bitstring_interval = 10; // Default bitstring update interval for NFU with aging
    std::string log_mode = "summary"; // Default logging mode
    std::vector<int> bitsPerLevel; // To store bits for each page table level


    // FOR CALCULATING BIT MASKS (idk about this part)
    std::vector<unsigned int> bitMasks;
    std::vector<int> shifts;
    std::vector<int> entryCounts;
    int shiftAccumulator = 0;
    int accesses_processed =0;
    int page_hits = 0;
    int page_faults = 0; // To count page faults
    p2AddrTr addr; // Simulate processing of addresses from the trace file
    



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
   FILE* traceFile = fopen(trace_file_name.c_str(), "rb");
    if (!traceFile) {
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

    //BitMask aka "-l" option logging is only case where you do not need to traverse through file
    //thus do this before making Page Table object
    if(log_mode =="bitmasks"){
         std::vector<uint32_t> masks(bitsPerLevel.size());
        int shift = 32;

        for (size_t i = 0; i < bitsPerLevel.size(); ++i) {
            shift -= bitsPerLevel[i];
            masks[i] = ((1U << bitsPerLevel[i]) - 1) << shift;
        }

        log_bitmasks(masks.size(), masks.data());
        return EXIT_SUCCESS; // Terminate after logging for 'bitmasks' mode
    }




    // Initialize the PageTable object here with the parameters we just got
    // needs this info PageTable(int lc, const std::vector<unsigned int>& bitmasks, const std::vector<int>& shifts, const std::vector<int>& entryCounts);
    //should calculate bitmasks and shifts and entry counts before making this page table
    //could make functions here to do so?
    //I will clean this up lol sorry its ugly code
    
    
    for (int i = 0; i < bitsPerLevel.size(); i++) {
        unsigned int mask = ((1u << bitsPerLevel[i]) - 1) << shiftAccumulator;
        bitMasks.push_back(mask);
        shifts.push_back(shiftAccumulator);
        entryCounts.push_back(1 << bitsPerLevel[i]);
        shiftAccumulator += bitsPerLevel[i];
    }

    // Initialize the PageTable object
    PageTable pageTable(bitsPerLevel.size(), bitMasks, shifts, entryCounts);
   
   // go through file (NextAddress is how he says to do it)
    while (NextAddress(traceFile, &addr) && (num_accesses == -1 )) {
     unsigned int vAddr = addr.addr;
    if (pageTable.searchMappedPfn(vAddr) != nullptr) {
        page_hits++;
    } else {
        pageTable.insertMapForVpn2Pfn(vAddr, accesses_processed % num_frames);
        page_faults++;
    }
    accesses_processed++;
    }

    //close the file
    fclose(traceFile);


    // Implement the logging based on the chosen log_mode using the functions provided in log_helpers.h/cpp
    // Summary logging at the end of processing

    if (log_mode == "summary") {
    //can't implement this until we do page replacement
    }


    return EXIT_SUCCESS;
}
