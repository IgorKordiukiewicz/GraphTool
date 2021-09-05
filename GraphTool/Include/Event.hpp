#pragma once

#include <functional>
#include <map>

template<typename... ParameterTypes>
class Event
{
public:
	template<typename T>
	inline void connect(std::string delegateName, T* object, void (T::* function)(ParameterTypes...))
	{
		delegates.insert(std::make_pair(std::move(delegateName), [=](ParameterTypes... parameters) {
			(object->*function)(parameters...);
			}));
	}

	inline void disconnect(std::string delegateName)
	{
		delegates.erase(std::move(delegateName));
	}

	inline void disconnectAll()
	{
		delegates.clear();
	}

	inline void emit(ParameterTypes... parameters)
	{
		for (auto& [name, function] : delegates) {
			function(parameters...);
		}
	}

	inline int getDelegatesCount() const
	{
		return delegates.size();
	}

private:
	std::map<std::string, std::function<void(ParameterTypes...)>> delegates;
};
