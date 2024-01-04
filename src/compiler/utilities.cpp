#include "utilities.hpp"


void ScopeStack::Enter()
{
	_stackMap.push(std::map<std::string_view, SyntaxTreeNode*>());
}


void ScopeStack::Exit()
{
	_stackMap.top().clear();
	_stackMap.pop();
}


bool ScopeStack::Define(std::string_view name, SyntaxTreeNode* node)
{
	if (_stackMap.top().count(name) == 0) return false;
	_stackMap.top().insert(std::pair(name, node));
	return true;
}


SyntaxTreeNode* ScopeStack::Lookup(std::string_view name)
{
	if (_stackMap.top().count(name) == 0) return nullptr;
	else return _stackMap.top()[name];
}
