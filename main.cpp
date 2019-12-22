#include "share.h"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "bulk.h"

using std::cout;
using std::endl;
using std::string;
namespace fs = std::filesystem;

template<class InStream>
bool readline(InStream& in, string& line) {
	line = "";
	while (true) {
		char ch;
		// TODO!!! Optimize it
		/*std::istream fin;*/
		in.read(&ch, 1);
		if (in.eof()) {
			return false;
		}
		if (ch == '\0') {
			return false;
		}
		else if (ch == '\n') {
			return true;
		}
		else {
			line += ch;
			continue;
		}
	}
}

class Processing {
	Bulk bulk;
public:
	Processing(int limit) : bulk{ limit } {}

	void process(string& line) {
		bulk(line);
	}

	template<typename InStream>
	void processInput(InStream& in) {
		string line;
		while (readline(in, line)) {
			process(line);
		}
		bulk.eof();
	}
};

//void processInput(std::ifstream& in) {
//	cout << "----- OK" << endl;
//}

int main(int argc, char** argv) {
	/*const int bufsize = 1024;
	char buf[bufsize];*/
	
	Processing proc{ std::atoi(argv[1]) };
	if (argc == 3) {
		/*auto fin = fopen(argv[2], "r");*/
		/*fread(buf, 1, bufsize, fin);
		fclose(fin);*/
		string line;
		std::ifstream fin(argv[2], std::ios::in);
		if (fs::exists(argv[2])) {
			
		}
		else {
			std::cerr << "file " << argv[2] << " does not exists" << endl;
			return 1;
		}

		proc.processInput(fin);
	}
	else {
		proc.processInput(std::cin);
	}
	
	return 0;
}
