#pragma once

#include "share.h"

#include <string>
#include <vector>

#include "interface.h"
#include "observers.h"
#include "commands.h"

using std::string;
using std::vector;

template<typename ...OutObservers>
class BulkBase {
	CommandsState state;
	CommandObservable cmdObservable;
	vector<std::unique_ptr<OutputObserver>> observers;
	CommandsHandler handler{ cmdObservable, state };
public:
	BulkBase() = default;
	using fnInit = std::function<void(OutputObserver*)>;
	BulkBase(int limit, vector<fnInit> fnsInit = vector<fnInit>{}) : BulkBase{} {
		state.init(limit);
		obsFactory();
		for (int i = 0; i < fnsInit.size(); ++i) {
			fnsInit[i](observers[i].get());
		}
	}

private:
	template<int obsIdx = 0>
	void obsFactory() {
		if constexpr (obsIdx < sizeof...(OutObservers)) {
			using ObsType = std::tuple_element_t<obsIdx, std::tuple<OutObservers...>>;
			auto ptr_obs = std::make_unique<ObsType>(cmdObservable, state);
			observers.push_back(std::move(ptr_obs));
			obsFactory<obsIdx + 1>();
		}
	}

public:
	void operator()(string& line) {
		handler(line);
	}
	void eof() { cmdObservable.notify(ECommand::END); }
};

using Bulk = BulkBase< ConsoleOutputObserver, FileOutputObserver<true>>;
