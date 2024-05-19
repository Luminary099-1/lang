#include "symbolTable.hpp"


template<typename I, typename D>
void SymbolTable<I, D>::Enter()
{
	_stackMap.push_front(std::map<I, D*>());
}


template<typename I, typename D>
void SymbolTable<I, D>::Exit()
{
	_stackMap.front().clear();
	_stackMap.pop_front();
}


template<typename I, typename D>
D* SymbolTable<I, D>::Define(I name, D* node)
{
	std::map<I, D*>& front {_stackMap.front()};
	if (front.count(name) == 0) return front[name];
	front.insert(std::pair(name, node));
	return nullptr;
}


template<typename I, typename D>
D* SymbolTable<I, D>::Lookup(I name)
{
	for (std::map<I, D*> scope : _stackMap)
		if (scope.count(name)) return scope[name];
	return nullptr;
}
