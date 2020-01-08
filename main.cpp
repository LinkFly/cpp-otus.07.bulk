#include "share.h"

#include <iostream>
#include <string>
#include <fstream>
/*#include <filesystem>*/
#include <boost/filesystem.hpp>
//#include <memory>
#include <string>

#include "bulk.h"

using std::cout;
using std::endl;
using std::string;
//namespace fs = std::filesystem;
namespace fs = boost::filesystem;

template<class InStream, size_t bufsize = 20>
bool readline(InStream& in, string& line) {
	std::getline(in, line);
	return in.eof();
}

class Processing {
	Bulk bulk;
public:
	Processing(int limit) : bulk{ limit } {}

	template<typename InStream>
	void processInput(InStream& in) {
		string line;
		bool isEof;
		do {
			isEof = readline(in, line);
			bulk(line);
		} while (!isEof);
		bulk.eof();
	}
};

int main(int argc, char** argv) {
	Processing proc{ std::atoi(argv[1]) };
	if (argc == 3) {
		if (fs::exists(argv[2])) {
			std::ifstream fin(argv[2], std::ios::in);
			proc.processInput(fin);
		}
		else {
			std::cerr << "file " << argv[2] << " does not exists" << endl;
			return 1;
		}
	}
	else {
		proc.processInput(std::cin);
	}

	return 0;
}
