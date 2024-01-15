#include "base.hpp"

#include <iostream>

using namespace std::string_literals;
using namespace std::string_view_literals;


void ScopeStack::Enter()
{
	_stackMap.push_front(std::map<std::string_view, Declaration*>());
}


void ScopeStack::Exit()
{
	_stackMap.front().clear();
	_stackMap.pop_front();
}


Declaration* ScopeStack::Define(std::string_view name, Declaration* node)
{
	std::map<std::string_view, Declaration*>& front {_stackMap.front()};
	if (front.count(name) == 0) return front[name];
	front.insert(std::pair(name, node));
	return nullptr;
}


Declaration* ScopeStack::Lookup(std::string_view name)
{
	for (std::map<std::string_view, Declaration*> scope : _stackMap)
		if (scope.count(name)) return scope[name];
	return nullptr;
}


uint32_t GenerateData::NextLabel()
{
	return _nextLabel ++;
}


void GenerateData::LabelOut(uint32_t label)
{
	*_os << "L_"sv << label << ":\n"sv;
}


uint32_t GenerateData::OutAndNextLabel()
{
	*_os << "L_"sv << _nextLabel << ":\n"sv;
	return _nextLabel ++;
}


void GenerateData::Defer()
{
	_stash.emplace();
	_os = &_stash.top();
}


void GenerateData::Resume()
{
	_dest << _stash.top().rdbuf();
	_stash.pop();
	if (_stash.size() != 0) _os = &_stash.top();
	else _os = &_dest;
}


bool SyntaxTreeNode::Scope(ScopeStack& ss, TUBuffer& src)
{
	// Take no action and assume success by default.
	return true;
}


bool SyntaxTreeNode::Validate(ValidateData& dat)
{
	// Take no action and assume success by default.
	return true;
}


void SyntaxTreeNode::Generate(GenerateData& dat)
{
	// Take no action by default.
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


Identifier::Identifier(std::string& id)
	: _id{id}
{}


void Identifier::Print(std::ostream& os, std::string_view indent, int depth)
{
	os << "ID = "sv << _id;
}


Declaration::Declaration(Identifier* name)
	: _name{name}
{}


bool Declaration::Scope(ScopeStack& ss, TUBuffer& src)
{
	Declaration* pre_def {ss.Define(_name->_id, this)};
	if (pre_def != nullptr)
	{
		std::cerr << '(' << _name->_row << ", "sv << _name->_col
			<< "): Symbol collision: "sv << _name->_id
			<< "\n# The following on line "sv << _name->_row << ":\n"sv;
		HighlightError(std::cerr, src, *_name);
		std::cerr << "# Redefines on line "sv << pre_def->_name->_row << ":\n"sv;
		HighlightError(std::cerr, src, *pre_def->_name);
		return false;
	}
	return true;
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

const bool Type::IsVoid() const
{
	return this == _void.get();
}


const bool Type::IsInt() const
{
	return this == _int.get();
}


const bool Type::IsBool() const
{
	return this == _bool.get();
}


const bool Type::IsString() const
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
