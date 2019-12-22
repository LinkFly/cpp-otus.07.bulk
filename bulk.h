#pragma once

#include "share.h"
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <ctime>

using std::cout;
using std::endl;
using std::string;
using std::vector;

//template<typename T>
//struct IState {
//	virtual void add(T& cmdline) = 0;
//	virtual void forEach(std::function<void(T& cmd)> fn) = 0;
//	virtual void clear() = 0;
//	virtual T& getLast() = 0;
//	virtual size_t size() = 0;
//	// TODO remove
//	virtual unsigned getNewCount() = 0;
//	virtual void setNewCount(unsigned) = 0;
//	virtual unsigned getLimit() = 0;
//	virtual void setLimit(unsigned) = 0;
//};
//
//
//
//using ICommandsState = CommandsStateBase<string>;
//
//struct CommandsStateBase : public IState<string> {

//};

class CommandsState/*: public CommandsStateBase*/ {
	vector<string> cmdLines;
public:
	unsigned limit = 0;
	unsigned newCount = 0;
	std::time_t start_read = 0; // time
	void init(int limit) { this->limit = limit; }
	void add(string& cmdline)  {
		if (cmdLines.size() == 0) {
			start_read = std::time(0);
		}
		cmdLines.push_back(cmdline);
	}
	void removeLast() {
		cmdLines.pop_back();
	}
	void forEach(std::function<void(string& cmd)> fn)  {
		for (auto& cmd : cmdLines) fn(cmd);
	}
	void clear()  {
		cmdLines.clear();
		newCount = 0;
		start_read = 0; // for simplify debugging
	}
	string& getLast()  {
		return cmdLines[cmdLines.size() - 1];
	}
	size_t size() { return cmdLines.size(); }
	//unsigned getNewCount() { return newCount; }
	//void setNewCount(unsigned newCount) {
	//	this->newCount = newCount;
	//}
	//unsigned getLimit() { return limit; }
	//void setLimit(unsigned limit) {
	//	this->newCount = limit;
	//}
//private:
//	int getUnixTime() {
//		return std::time();
//	}
};


//struct ICommandHandler {
//	virtual void exec() = 0;
//};
//
//class CmdStartHandler : ICommandHandler {
//	CommandsStateBase<string>& state;
//public:
//	CmdStartHandler(CommandsStateBase<string>& state) : state{ state } {}
//	void exec() override {
//		state.clear();
//	}
//};
//
//class CmdEndHandler : ICommandHandler {
//	ICommandsState& state;
//public:
//	CmdEndHandler(ICommandsState& state) : state{ state } {}
//	void exec() override {
//		state.clear();
//	}
//};

///////// Interfaces part ////////////
enum class ECommand : int {
	ADD, START, END
};

struct IObserver {
	virtual void update(ECommand cmd) = 0;
};

struct IObservable {
	virtual void subscribe(IObserver*) = 0;
	virtual void notify(ECommand cmd) = 0;
};

class CommandObservable : public IObservable {
	std::vector<IObserver*> observers;
public:
	void subscribe(IObserver* observer) override {
		observers.push_back(observer);
	}
	void notify(ECommand cmd) override {
		for (IObserver* observer : observers) {
			observer->update(cmd);
		}
	}
};
///////// end Interfaces part ////////////

struct ObserverBase : public IObserver {
	ObserverBase(IObservable& observable, CommandsState& state) : state{ state } {
		observable.subscribe(this);
	}
protected:
	CommandsState& state;

	/*virtual std::ostream& getOutStream() = 0;*/
	virtual void outBulk() = 0;

	void update(ECommand cmd) {
		switch (cmd) {
		case ECommand::END:
			outBulk();
		}
	}
};

struct ConsoleOutputObserver : public ObserverBase {
	ConsoleOutputObserver(IObservable& observable, CommandsState& state) : ObserverBase{ observable, state } {}
	std::ostream& getOutStream() {
		return cout;
	}
	void outBulk() {
		std::ostream& out = getOutStream();
		out << "bulk: ";
		int idx = 0;
		state.forEach([&out, &idx](string& line) {
			if (idx++) out << ", ";
			out << line;
			});
		out << endl;
	}
};

struct FileOutputObserver : public ObserverBase {
	FileOutputObserver(IObservable& observable, CommandsState& state) : ObserverBase{ observable, state } {}

	std::ostream& getOutStream() {
		if (ptr_fout != nullptr) {
			delete ptr_fout;
		}
		ptr_fout = new std::ofstream(genFileName());
		return *ptr_fout;
		return cout;
	}
	void outBulk() {
		std::ostream& out = getOutStream();
		state.forEach([&out](string& line) {
			out << line << endl;
			});
	}
	~FileOutputObserver() {
		delete ptr_fout;
	}
private:
	// TODO rewrite to shared_ptr
	std::ofstream* ptr_fout = nullptr;
	
	string genFileName() {
		// TODO del tmp
		static int diff = 0;
		std::time_t result = state.start_read + ++diff;
		char buf[20];
		itoa(result, buf, 10);
		return string("bulk") + string(buf);
	}
};

class Bulk {
	CommandsState state;
	CommandObservable cmdObservable;
	ConsoleOutputObserver cmdConsoleObserver{ cmdObservable, state };
	FileOutputObserver cmdFileObserver{ cmdObservable, state };
	
public:
	Bulk(int limit) {
		state.init(limit);
	}

	void operator()(string& line) {
		if (line == "{") {
			if (state.newCount == 0) {
				notifyAdd();
			}
			++state.newCount;
		}
		else if (line == "}") {
			--state.newCount;
			if (state.newCount == 0) {
				notifyAdd();
			}
		}
		else {
			state.add(line);
			if (state.newCount == 0) {
				if (state.size() == state.limit) {
					notifyEnd();
					state.clear();
				}
			}
		}
	}
	/*using eof_t = decltype(std::ios::eof());
	void operator()(decltype{std::ios::eof()), std::ios::eof()) {

	}*/
	void eof() { cmdObservable.notify(ECommand::END); }

private:
	void notifyAdd() {
		cmdObservable.notify(ECommand::ADD);
	}
	void notifyEnd() {
		cmdObservable.notify(ECommand::END);
	}
};