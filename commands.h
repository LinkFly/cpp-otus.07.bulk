#pragma once

#include <map>

#include "interface.h"
#include "state.h"

class CmdBase :public ICommand<string> {
	IObservable* cmdObservable;
public:
	void init(IObservable* cmdObservable, CommandsState* state) {
		this->cmdObservable = cmdObservable;
		this->state = state;
	}
protected:
	CommandsState* state;
	void notifyStart() {
		cmdObservable->notify(ECommand::START);
	}
	void notifyEnd() {
		cmdObservable->notify(ECommand::END);
	}
	void notifyAdd() {
		cmdObservable->notify(ECommand::ADD);
	}
};

class CmdStart : public CmdBase {
public:
	void exec([[maybe_unused]] string& data) override {
		if (state->newCount == 0) {
			notifyEnd();
			state->clear();
			notifyStart();
		}
		++state->newCount;
	}
};

class CmdEnd : public CmdBase {
public:
	void exec([[maybe_unused]] string& data) override {
		--state->newCount;
		if (state->newCount == 0) {
			notifyEnd();
			state->clear();
		}
	}
};

class CmdAdd : public CmdBase {
public:
	void exec(string& data) override {
		state->add(data);
		if (state->newCount == 0) {
			if (state->size() == state->limit) {
				notifyEnd();
				state->clear();
			}
		}
	}
};

class CommandsHandler {
	IObservable& observable;
	CommandsState& state;
	CmdStart start;
	CmdEnd end;
	CmdAdd add;
	std::map<ECommand, CmdBase*> cmdsDict{
		{ECommand::START, &start},
		{ECommand::END, &end},
		{ECommand::ADD, &add},
	};
public:
	CommandsHandler(IObservable& observable, CommandsState& state) : observable{ observable }, state{ state } {
		for (auto& keyval : cmdsDict) {
			keyval.second->init(&observable, &state);
		}
	}
	void operator()(string& line) {
		if (line == "{") {
			cmdsDict[ECommand::START]->exec();
		}
		else if (line == "}") {
			cmdsDict[ECommand::END]->exec();
		}
		else {
			cmdsDict[ECommand::ADD]->exec(line);
		}
	}
};