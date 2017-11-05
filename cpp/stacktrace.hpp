#include ".private/backward.hpp"

#include <iostream>
#include <string>

//print stack trace to stream
#define STS(S){\
	backward::StackTrace st;\
	st.load_here(32);\
	backward::Printer p;\
	p.print(st, S);\
}

//print stack trace to std::cout
#define STP STS(std::cout)

#ifdef __GNUC__
	#include <signal.h>
	#include <cstdlib>
	
	static void stseghandler(int sig){
		STP
		exit(1);
	}

	//install signal handler for segmentation fault that prints stack trace to std::cout
	static void stseg(){
		signal(SIGSEGV, stseghandler);
	}
#endif
