//Jessica Chammas- 826401167
//Baraa Erras -

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <unistd.h> //needed for getopt to work
#include <cstdint>
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


    // calculate masks + shifts to be able to make Page Table object
     for (int i = 0; i < bitsPerLevel.size(); i++) {
        unsigned int mask = ((1u << bitsPerLevel[i]) - 1) << shiftAccumulator;
        bitMasks.push_back(mask);
        shifts.push_back(shiftAccumulator);
        entryCounts.push_back(1 << bitsPerLevel[i]);
        shiftAccumulator += bitsPerLevel[i];
    }

    // Initialize the PageTable object
    PageTable pageTable(bitsPerLevel.size(), bitMasks, shifts, entryCounts);
    pageTable.setBitstringInterval(bitstring_interval);
    
   

    // required to log offset mode
    int addressesProcessed = 0;
    int offsetBits = 32 - total_bits; // Calculate bits used for offset
    unsigned int offsetMask = (1u << offsetBits) - 1; // Mask to extract offset
    //printf("Page number = 0x%x\n", offsetMask); for debugging purposes to show that offsetmask is 0xfff


     // Process addresses based on log mode
    if (log_mode == "offset") {
        while (NextAddress(traceFile, &addr) && (addressesProcessed < num_accesses || num_accesses == -1)) {
        // Extract the offset by performing a bitwise AND with the offset mask
        unsigned int offset = addr.addr & offsetMask;
        // Print the offset using the provided print_num_inHex function
        print_num_inHex(offset);
        addressesProcessed++;
     }
    } else if (log_mode == "vpns_pfn") {
    while (NextAddress(traceFile, &addr) && (addressesProcessed < num_accesses || num_accesses == -1)) {
        // Extract the virtual address
        unsigned int virtualAddress = addr.addr;

        // dynamic size and initialization
        std::vector<uint32_t> vpns(bitsPerLevel.size(), 0); // Initialized to 0

        unsigned int fullVpn = 0; // To hold the full VPN for accessPage
        int startBit = 32; //  32-bit virtual address space

        for (size_t i = 0; i < bitsPerLevel.size(); ++i) {
            startBit -= bitsPerLevel[i]; // Update start bit for the current level
            unsigned int mask = (1 << bitsPerLevel[i]) - 1;
            unsigned int vpnPart = (virtualAddress >> startBit) & mask;
            vpns[i] = vpnPart;
           
            fullVpn = (fullVpn << bitsPerLevel[i]) | vpnPart;
        }

        // Retrieve the actual PFN using the accessPage method with the fullVpn
        unsigned int pfn = pageTable.accessPage(fullVpn);

        // Log the VPNs for each level and the corresponding PFN
        log_vpns_pfn(bitsPerLevel.size(), vpns.data(), pfn);

        addressesProcessed++;
    }
}

else if (log_mode == "va2pa"){
         while (NextAddress(traceFile, &addr) && (addressesProcessed < num_accesses || num_accesses == -1)) {
            unsigned int virtualAddress = addr.addr;
            unsigned int physicalAddress = virtualAddress & offsetMask; // Apply mask to retain lower 20 bits (offset mask is 0xfff)
        
            // Log the virtual to physical address translation
            log_va2pa(virtualAddress, physicalAddress);
            
            addressesProcessed++;
        }
    }else if(log_mode == "vpn2pfn_pr"){
        //need to do this still
    }else{
        //summary case
    }



    //close file
    fclose(traceFile);
    return EXIT_SUCCESS;
}
