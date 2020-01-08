#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include "interface.h"
#include "state.h"

using std::cout;
using std::endl;

struct ObserverBase : public IObserver {
	ObserverBase(IObservable& observable, CommandsState& state) : state{ state } {
		observable.subscribe(this);
	}
protected:
	CommandsState& state;

	virtual void outBulk() = 0;

	void update(ECommand cmd)  override {
		switch (cmd) {
		case ECommand::END:
			outBulk();
			break;
		default:
			;
		}
	}
	void update([[maybe_unused]] ECommand cmd, [[maybe_unused]] string data) override {}
};

struct CollectorObserver : public ObserverBase {
	CollectorObserver(IObservable& observable, CommandsState& state) : ObserverBase{ observable, state } {}
	void update(ECommand cmd, string data) {
		switch (cmd) {
		case ECommand::ADD:
			state.add(data);
			break;
		default:
			;
		}
	}
};

struct OutputObserver : public ObserverBase {
	OutputObserver(IObservable& observable, CommandsState& state) : ObserverBase{ observable, state } {}
	virtual void setOutStream(std::ostream* out) {
		this->out = out;
	}
	virtual std::ostream& getOutStream() {
		return *out;
	}
protected:
	std::ostream* out;
};

struct StreamOutputObserver : public OutputObserver {
	StreamOutputObserver(IObservable& observable, CommandsState& state) : OutputObserver{ observable, state } {}
	void outBulk() override {
		std::ostream& out = getOutStream();
		state.forEach([&out](string& line, [[maybe_unused]] bool isFirst, [[maybe_unused]] bool isLast) {
			if (isFirst) {
				out << "bulk: ";
			}
			out << line;
			if (!isLast) {
				out << ", ";
			}
			});
		out << endl;
	}
};

struct ConsoleOutputObserver : public StreamOutputObserver {
	ConsoleOutputObserver(IObservable& observable, CommandsState& state) : StreamOutputObserver{ observable, state } {
		setOutStream(&std::cout);
	}
};

template<bool use_inc_file = false>
struct FilenameGetter : public IFilenameGetter {
	string operator()(time_t time) override {
		// For tests and other (when input data come in without delay)
		if constexpr (use_inc_file) {
			time += diff++;
		}
		return string("bulk") + timeToString(time);
	}

	static string timeToString(time_t time) {
		std::stringstream sin;
		sin << time;
		return sin.str();
	}
private:
	int diff = 0;
};

template<bool use_inc_file = false, class FNameGetter = FilenameGetter<use_inc_file>>
struct FileOutputObserver : public OutputObserver {
	FileOutputObserver(IObservable& observable, CommandsState& state) : OutputObserver{ observable, state } {}

	std::ostream& getOutStream() override {
		closeFile();
 		ptr_fout = std::make_unique<std::ofstream>(genFileName());
		return *ptr_fout;
	}
	void outBulk() override {
		setOutStream(&getOutStream());
		state.forEach([this](string& line, [[maybe_unused]] bool isFirst, [[maybe_unused]] bool isLast) {
			*out << line;
			if (!isLast) 
				*out << endl;
			});
		out->flush();
	}

	~FileOutputObserver() {
		closeFile();
	}
private:
	std::unique_ptr<std::ofstream> ptr_fout;
	static inline FNameGetter fnameGetter{};

	void closeFile() {
		if (ptr_fout.get() != nullptr) {
			ptr_fout->close();
		}
	}
	string genFileName() {
		std::time_t result = state.start_read;
		return genFileName(result);
	}
public:
	static string genFileName(time_t time) {
		return fnameGetter(time);
	}
};