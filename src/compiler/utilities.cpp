#include "utilities.hpp"


void HighlightError(std::ostream& os, TUBuffer& src, TokenInfo& info)
{
	size_t line_start {info._off - info._col + 1};
	os << '\t';
	for (size_t i {line_start}; src._buf[i] != '\n' && i < src._size; ++ i)
		os << src._buf[i];
	os << '\n' << '\t';
	for (int i {0}; i < info._col - 1; ++ i) os << ' ';
	for (int i {0}; i < info._endOff - info._off; ++ i) os << '^';
	os << '\n';
};


void ScopeStack::Enter()
{
	_stackMap.push_front(std::map<std::string_view, Symbol*>());
}


void ScopeStack::Exit()
{
	_stackMap.front().clear();
	_stackMap.pop_front();
}


Symbol* ScopeStack::Define(std::string_view name, Symbol* node)
{
	std::map<std::string_view, Symbol*>& front {_stackMap.front()};
	if (front.count(name) == 0) return front[name];
	front.insert(std::pair(name, node));
	return nullptr;
}


Symbol* ScopeStack::Lookup(std::string_view name)
{
	for (std::map<std::string_view, Symbol*> scope : _stackMap)
		if (scope.count(name)) return scope[name];
	return nullptr;
}
