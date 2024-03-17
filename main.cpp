#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pagetable.h"



int main(int argc, char** argv){
    //how to process the command line args
    int option;
    while(( option = getopt(argc,argv, "n:o"))!=-1){
        switch(option){
            case 'n':
            //process -n option
            break;
            case 'o':
            //process -o opt
            break;
            default:
            //invalid option
            break;
        }
    }
    //mandatoray args
    int idx = optind;
    if(idx <argc){
        //process args
        //open trace file
        FILE* traceFile = fopen(argv[idx], "r");
        if(traceFile==NULL){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}