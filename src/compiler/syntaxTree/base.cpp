#include "base.hpp"

#include <iostream>
#include <numeric>

using namespace std::string_literals;
using namespace std::string_view_literals;


ValidateData::ValidateData(TU* src)
	: _src{src}
{}


Location::Location()
{}


Location::Location(Place place, Type* type)
	: _place{place}, _type{type}
{}


Location Location::CreateGlobal(Type* type, IDT label)
{
	Location loc (Place::Global, type);
	loc._val._label = label;
	return loc;
}


Location Location::CreateRegister(Type* type, RegT reg)
{
	Location loc (Place::Register, type);
	loc._val._reg = reg;
	return loc;
}


Location Location::CreateLocal(Type* type, BytesT offset)
{
	Location loc (Place::Local, type);
	loc._val._offset = offset;
	return loc;
}


Location::Place Location::GetPlace()
{
	return _place;
}


void Location::ReinterpretStack(BytesT stack_args_size)
{
	if (_place != Place::Local) return;
	_val._offset = _val._offset - stack_args_size - 16;
}


IDT GenData::NextLabel()
{
	return _nextLabel ++;
}


void GenData::GeneratePop(Type* type, RegT reg, std::ostream& os)
{
	const BytesT size {type->GetSize()};
	if (_isSubFrame) _frameSize -= size;

	switch (size)
	{
		case 1:	os << "\tldrb\tw"sv << reg << ",\t[sp]\t1\n"sv;	break;
		case 2:	os << "\tldrh\tw"sv << reg << ",\t[sp]\t2\n"sv;	break;
		case 4:	os << "\tldr\tw"sv << reg << ",\t[sp]\t4\n"sv;	break;
		case 8:	os << "\tldr\tx"sv << reg << ",\t[sp]\t8\n"sv;	break;
	}
}


void GenData::GeneratePush(Type* type, RegT reg, std::ostream& os)
{
	const BytesT size {type->GetSize()};
	if (_isSubFrame) _frameSize += size;

	switch (size)
	{
		case 1:	os << "\tstrb\tw"sv << reg << ",\t[sp, -1]!\n"sv;	break;
		case 2:	os << "\tstrh\tw"sv << reg << ",\t[sp, -2]!\n"sv;	break;
		case 4:	os << "\tstr\tw"sv << reg << ",\t[sp, -4]!\n"sv;	break;
		case 8:	os << "\tstr\tx"sv << reg << ",\t[sp, -8]!\n"sv;	break;
	}
}


bool SyntaxTreeNode::Validate(ValidateData& dat)
{
	// Take no action and assume success by default.
	return true;
}


void SyntaxTreeNode::Generate(GenData& dat, std::ostream& os)
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


const std::unique_ptr<Type> Type::_void(	new Type("void"s,	0));
const std::unique_ptr<Type> Type::_int(		new Type("int"s,	4));
const std::unique_ptr<Type> Type::_bool(	new Type("bool"s,	1));
const std::unique_ptr<Type> Type::_string(	new Type("string"s,	8));


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


Type::Type(std::string& type_name, BytesT size)
	: _name{type_name}, _size{size}
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


const bool Type::IsIntegral() const
{
	return this == _int.get() || this == _bool.get();
}


const bool Type::IsPointer() const
{
	return this == _string.get();
}


const BytesT Type::GetSize() const
{
	return _size;
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


void
Type::GenerateAccess(GenData& dat, Location loc, bool load, std::ostream& os)
{
	RegT ior {dat._safeRegs.top()}; // Input/output register.
	if (loc._place == Location::Place::Register)
	{
		const char s {(_size <= 4) ? 'w' : 'x'};
		const RegT vr {loc._val._reg};
		if (load) os << "\tmov\t"sv << s << ior << ",\t"sv << s << vr << '\n';
		else os << "\tmov\t"sv << s << vr << ",\t"sv << s << ior << '\n';
	}
	else
	{
		RegT ar; // Address register.
		if (loc._place == Location::Place::Global)
		{
			dat._safeRegs.pop();
			ar = dat._safeRegs.top();
			dat._safeRegs.push(ar);
			os << "\tadrp\tx"sv << ar << "\t,L_"sv << loc._val._label
				<< "\n\tadd\tx"sv << ar << ",\tx"sv << ar << ",\t:lo12:L_"sv
				<< loc._val._label << '\n';
		}
		
		if (load && IsInt()) switch (_size)
		{
			case 1:	os << "\tldrwb\tw"sv;	break;
			case 2:	os << "\tldrwh\tw"sv;	break;
			case 4:	os << "\tldrsw\tw"sv;	break;
			case 8:	os << "\tldr\tx"sv;		break;
		}
		else
		{
			os << '\t' << ((load) ? 'l' : 's');
			switch (_size)
			{
				case 1:	os << "drb\tw"sv;	break;
				case 2:	os << "drh\tw"sv;	break;
				case 4:	os << "dr\tw"sv;	break;
				case 8:	os << "dr\tx"sv;	break;
			}
		}
		os << ior;
		if (loc._place == Location::Place::Global)
			os << ",\t[x"sv << ar << "]\n"sv;
		else os << ",\t[fp, "sv << loc._val._offset << "]\n"sv;
	}
}


void Type::Print(std::ostream& os, std::string_view indent, int depth)
{
	os << "Type = "sv << _name;
}
