#pragma once

#include "share.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

enum class ECmd: int {
	START, END
};
struct Command {
	ECmd cmd;
};

struct StateBase {
	Command command;
};



class Bulk {

public:
	void operator()(string& line) {
		cout << "line: " << line << endl;
	}
};