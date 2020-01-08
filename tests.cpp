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
/*#include <filesystem>*/
#include <boost/filesystem.hpp>
#include <ctime>

#include "bulk.h"
#include "observers.h"
#include "interface.h"

using std::string;
using std::cout;
using std::endl;
using std::clock;
using std::vector;
using std::function;
using std::ifstream;
/*namespace fs = std::filesystem;*/
namespace fs = boost::filesystem;

const char temp_dir[] = "_tmp_bulk";

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

template<class BulkType>
string base_send_to(BulkType& bulk, std::unique_ptr<std::stringstream>& psout, vector<string> cmdLines) {
	for (auto& cmd : cmdLines) {
		bulk(cmd);
	}
	bulk.eof();
	return psout->str();
}

template<class BulkType>
string send_to(BulkType& bulk, vector<string> cmdLines) {
	auto psout = std::make_unique<std::stringstream>();
	return base_send_to(bulk, psout, cmdLines);
}

string send(vector<string> cmdLines) {
	auto psout = std::make_unique<std::stringstream>();
	auto bulk = createBulk(psout);
	return base_send_to(bulk, psout, cmdLines);
}

string readfile(string file) {
	string res;
	string line;
	ifstream fin(file);
	bool isFirst = true;
	if (fin.is_open()) {
		while (true) {
			std::getline(fin, line);
			res += line;
			if (fin.eof()) {
				break;
			}
			else res += '\n';
		}
		fin.close();
	}
	return res;
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

bool create_files_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		constexpr int is_inc_file = true;
		struct MockFilenameGetter : public IFilenameGetter {
			FilenameGetter<is_inc_file> fnameGetter;
			string operator()([[maybe_unused]] time_t time) override {
				time_t fixed_time = 100;
				auto cur_file = get_temp_dir() / fnameGetter(fixed_time);
				return cur_file.string();
			}
			static fs::path get_temp_dir() {
				return fs::current_path() / temp_dir;
			}
		};
		//// Prepare
		fs::path dir = MockFilenameGetter::get_temp_dir();
		if (fs::exists(dir))
			fs::remove_all(dir);
		fs::create_directory(dir);
		//// end Prepare

		string content1, content2;
		bool isResult = false;
		{
			BulkBase<FileOutputObserver<is_inc_file, MockFilenameGetter>> bulk{ 3 };
			auto res = send_to(bulk, vector<string>{ "cmd1", "cmd2", "cmd3", "cmd4", "cmd5" });
			auto waitedFile1 = dir / "bulk100";
			auto waitedFile2 = dir / "bulk101";
			bool isExists1 = fs::exists(waitedFile1);
			bool isExists2 = fs::exists(waitedFile1);
			if (!isExists1 || !isExists2) {
				cout << "--- now exit ---\n";
				goto exitBlock;
			}
			content1 = readfile(waitedFile1.string());
			content2 = readfile(waitedFile2.string());
			if (content1 != "cmd1\ncmd2\ncmd3")
				goto exitBlock;
			if (content2 != "cmd4\ncmd5")
				goto exitBlock;
			// we here - without errors
			isResult = true;
		exitBlock:
			;
		}
		fs::remove_all(dir);
		return isResult;
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
	BOOST_CHECK(create_files_test());


}

BOOST_AUTO_TEST_SUITE_END()
