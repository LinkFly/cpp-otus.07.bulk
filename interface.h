#pragma once

#include <string>
#include <iostream>

using std::string;

enum class ECommand : int {
	ADD, START, END
};

struct IObserver {
	virtual void update(ECommand cmd) = 0;
	virtual void update(ECommand cmd, string data) = 0;
	virtual ~IObserver() { }
};

struct IObservable {
	virtual void subscribe(IObserver*) = 0;
	virtual void notify(ECommand cmd) = 0;
	virtual void notify(ECommand cmd, string data) {};
	virtual ~IObservable() {}
};

template<typename T>
struct ICommand {
	virtual void exec([[maybe_unused]] const T& data = T{}) = 0;
	virtual ~ICommand() {}
};

struct IFilenameGetter {
	virtual string operator()(time_t time) = 0;
};