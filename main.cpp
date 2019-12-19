#include "share.h"

#include <iostream>
#include <string>
#include <fstream>

#include "bulk.h"

using std::cout;
using std::endl;
using std::string;

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



void process(string& line) {
	static Bulk bulk{};
	bulk(line);

}

template<typename InStream>
void processInput(InStream& in) {
	string line;
	while (readline(in, line)) {
		process(line);
	}
}

//void processInput(std::ifstream& in) {
//	cout << "----- OK" << endl;
//}

int main(int argc, char** argv) {
	/*const int bufsize = 1024;
	char buf[bufsize];*/
	
	if (argc == 3) {
		/*auto fin = fopen(argv[2], "r");*/
		/*fread(buf, 1, bufsize, fin);
		fclose(fin);*/
		string line;
		auto fin = new std::ifstream(argv[2], std::ios::in);
		processInput(*fin);
		delete fin;
	}
	else {
		processInput(std::cin);
	}
	
	return 0;
}
