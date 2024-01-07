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


std::map<std::string_view, Type::Fundamentals> Type::_namedFundamentals {
	{"void"sv,		Fundamentals::Void		},
	{"int"sv,		Fundamentals::Int		},
	{"bool"sv,		Fundamentals::Bool		},
	{"string"sv,	Fundamentals::String	}
};


std::map<Type::Fundamentals, std::string_view> Type::_fundamentalNames {
	{Fundamentals::Void,	"void"sv	},
	{Fundamentals::Int,		"int"sv		},
	{Fundamentals::Bool,	"bool"sv	},
	{Fundamentals::String,	"string"sv	}
};


Type::Type()
{}


Type::Type(std::string type_name)
{
	if (_namedFundamentals.count(type_name))
		_fundType = _namedFundamentals[type_name];
	else _name = type_name;
}


bool Type::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	if (_fundType == Fundamentals::EMPTY)
	{
		_defType = ss.Lookup(_name);
		if (_defType == nullptr)
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
	os << "Type = "sv;
	if (_fundType == Fundamentals::EMPTY) os << _name;
	else os << _fundamentalNames[_fundType];
}
