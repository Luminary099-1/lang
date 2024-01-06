#include "base.hpp"

#include <iostream>

using namespace std::string_view_literals;


bool SyntaxTreeNode::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	return true;
}


void SyntaxTreeNode::PrintIndent(
	std::ostream& os, std::string_view indent, int depth)
{
	for (int i {0}; i < depth; ++ i) os << indent;
}


void SyntaxTreeNode::PrintMaybe(
	SyntaxTreeNode* node, std::ostream& os, std::string_view indent, int depth)
{
	if (node != nullptr) node->Print(os, indent, depth);
	else
	{
		PrintIndent(os, indent, depth);
		os << "nullptr\n"sv;
	}
}


const std::map<std::string_view, Type::Fundamentals> Type::_namedFundamentals {
	{"void"sv,		Type::Fundamentals::Void},
	{"int"sv,		Type::Fundamentals::Int},
	{"bool"sv,		Type::Fundamentals::Bool},
	{"string"sv,	Type::Fundamentals::String}
};


Type::Type()
{}


Type::Type(std::string type_name)
	: _name{type_name}
{}


bool Type::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	try { _fund_type = _namedFundamentals.at(_name); }
	catch(const std::out_of_range& e)
	{
		_type = ss.Lookup(_name);
		if (_type == nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Unknown type: "sv << _name << '\n';
			HighlightError(std::cerr, src, *this);
			return false;
		}
	}
	return true;
}


void Type::Print(std::ostream& os, std::string_view indent, int depth)
{
	os << _name;
}
