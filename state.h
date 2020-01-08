#pragma once

#include <vector>
#include <string>
#include <functional>
#include <ctime>

#include "interface.h"

using std::vector;
using std::string;

class CommandsState {
	vector<string> cmdLines;
public:
	unsigned limit = 0;
	unsigned newCount = 0;
	std::time_t start_read = 0; // time
	void init(int limit) { this->limit = limit; }
	void add(const string& cmdline) {
		if (cmdLines.size() == 0) {
			start_read = std::time(0);
		}
		cmdLines.push_back(cmdline);
	}
	void removeLast() {
		cmdLines.pop_back();
	}
	void forEach(std::function<void(string & cmd)> fn) {
		for (auto& cmd : cmdLines) fn(cmd);
	}
	void forEach(std::function<void(string & cmd, bool isFirst, bool isLast)> fn) {
		auto size = cmdLines.size();
		for (decltype(size) i = 0; i < size; ++i) {
			auto& cmd = cmdLines[i];
			fn(cmd, i == 0, i == (size - 1));
		}
	}
	void clear() {
		cmdLines.clear();
		newCount = 0;
		start_read = 0; // for simplify debugging
	}
	string& getLast() {
		return cmdLines[cmdLines.size() - 1];
	}
	size_t size() { return cmdLines.size(); }
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
	void notify(ECommand cmd, string data) override {
		for (IObserver* observer : observers) {
			observer->update(cmd, data);
		}
	}
};

