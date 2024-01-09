#include "base.hpp"

#include <iostream>

using namespace std::string_literals;
using namespace std::string_view_literals;


bool SyntaxTreeNode::Scope(ScopeStack& ss, TUBuffer& src)
{
	return true;
}


bool SyntaxTreeNode::Validate(ValidateData& dat)
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


const std::unique_ptr<Type> Type::_void(new Type("void"s));
const std::unique_ptr<Type> Type::_int(new Type("int"s));
const std::unique_ptr<Type> Type::_bool(new Type("bool"s));
const std::unique_ptr<Type> Type::_string(new Type("string"s));


const std::map<std::string_view, Type*> Type::_namedFundamentals
{
	{"void"sv,		Type::_void.get()	},
	{"int"sv,		Type::_int.get()	},
	{"bool"sv,		Type::_bool.get()	},
	{"string"sv,	Type::_string.get()	}
};


const std::set<const Type*> Type::_fundamentals
{
	{Type::_void.get()},
	{Type::_int.get()},
	{Type::_bool.get()},
	{Type::_string.get()}
};


Type::Type()
{}


Type::Type(std::string& type_name)
	: _name{type_name}
{}


Type* Type::Create(std::string type_name)
{
	if (_namedFundamentals.count(type_name))
		return _namedFundamentals.at(type_name);
	else return new Type(type_name);
}

bool Type::IsVoid()
{
	return this == _void.get();
}


bool Type::IsInt()
{
	return this == _int.get();
}


bool Type::IsBool()
{
	return this == _bool.get();
}


bool Type::IsString()
{
	return this == _string.get();
}


bool operator==(const Type& lhs, const Type& rhs)
{
	if (Type::_fundamentals.count(&lhs)) return &lhs == &rhs;
	else return lhs._defType == rhs._defType;
}


bool operator!=(const Type& lhs, const Type& rhs)
{
	return !(lhs == rhs);
}


bool Type::Scope(ScopeStack& ss, TUBuffer& src)
{
	if (!_fundamentals.count(this))
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
	os << "Type = "sv << _name;
}
