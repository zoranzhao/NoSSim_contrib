#include <systemc.h>

double power_cli[10];
double CliEnergy[10];
unsigned char debug_flags;

//unsigned char debug_flags=LWIP_DBG_ON;
//unsigned char debug_flags;
int sc_main(int, char *[]){

    std::cout << "The sc_main should not be called ..." <<std::endl;
    return 0;
}

