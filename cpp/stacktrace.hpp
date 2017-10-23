#include ".private/backward.hpp"

#include <iostream>
#include <string>

#define STS(S){\
	backward::StackTrace st;\
	st.load_here(32);\
	backward::Printer p;\
	p.print(st, S);\
}

#define STP STS(std::cout)

