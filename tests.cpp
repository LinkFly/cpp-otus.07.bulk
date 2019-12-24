#include "share.h"

#define BOOST_TEST_MODULE bulk_test_module

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <fstream>
#include <functional>
#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>

#include "bulk.h"
#include "observers.h"

using std::string;
using std::cout;
using std::endl;
using std::clock;
using std::vector;
using std::function;
using std::ifstream;
namespace fs = std::filesystem;

bool call_test(string name, std::function<bool(void)> fntest) {
	cout << "------------------------------\n";
	cout << endl << name << ":\n";
	//test_data test_data = create_test_data();

	auto startTime = clock();
	bool res = fntest();
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	cout << "------------------------------\n";
	return res;
}

BulkBase<StreamOutputObserver> createBulk(std::unique_ptr<std::stringstream>& psout, int size = 3) {
	using FnInit = function<void(OutputObserver*)>;
	auto fnsInit = vector<FnInit>{ [&psout](OutputObserver* obs) {
			obs->setOutStream(psout.get());
		} };
	return BulkBase<StreamOutputObserver>(3, fnsInit);
}

string send(vector<string>& cmdLines) {
	auto psout = std::make_unique<std::stringstream>();
	auto bulk = createBulk(psout);
	for (auto& cmd : cmdLines) {
		bulk(cmd);
	}
	bulk.eof();
	return psout->str();
}

string readfile(string file) {
	string res;
	string line;
	ifstream fin(file);
	bool isFirst = true;
	if (fin.is_open()) {
		if (std::getline(fin, line)) {
			res += line;
		}
		while (std::getline(fin, line)) {
			res += "\n" + line;
		}
	}
	fin.close();
	return res;
}

string get_filename() {
	FileOutputObserver::
	return "";
}

bool trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		auto res = send(vector<string>{ "cmd1", "cmd2", "cmd3", "cmd4", "cmd5" });
		return res == "bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n";
	});
}

bool dynamic_size_test() {
	/*cout << get_filename(time_t */
	return call_test(__PRETTY_FUNCTION__, []() {
		auto res = send(vector<string>{ "cmd1", "cmd2", "{", "cmd3", "cmd4", "cmd5", "cmd6", "}", "cmd7" });
		auto waitres = "bulk: cmd1, cmd2\nbulk: cmd3, cmd4, cmd5, cmd6\nbulk: cmd7\n";
		return res == waitres;
		});
}

bool nested_dynamic_size_test() {
	
	return call_test(__PRETTY_FUNCTION__, []() {
		auto res = send(vector<string>{ "cmd1", "cmd2", "{", "cmd3", "{", "cmd4", "cmd5", "}", "cmd6", "}", "cmd7" });
		auto waitres = "bulk: cmd1, cmd2\nbulk: cmd3, cmd4, cmd5, cmd6\nbulk: cmd7\n";
		return res == waitres;
		});
}

//struct Init {
//	Init(std::function<void()> init_func) {
//		init_func();
//	}
//};
//#define INIT(init_func) struct Init init(init_func);


BOOST_AUTO_TEST_SUITE(allocator_test_suite)
//INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_matrix)
{
	BOOST_CHECK(trivial_test());
	BOOST_CHECK(dynamic_size_test());
	BOOST_CHECK(nested_dynamic_size_test());
	
}

BOOST_AUTO_TEST_SUITE_END()
