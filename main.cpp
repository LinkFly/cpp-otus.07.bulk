#include "share.h"

#include <iostream>

#include "bulk.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
	const int bufsize = 1024;
	char buf[bufsize];
	
	if (argc == 3) {
		auto fin = fopen(argv[2], "r");
		fread(buf, 1, bufsize, fin);
		fclose(fin);
	}
	else {
		std::cin >> buf;
	}

	cout << buf[0] << endl;
	
	return 0;
}
