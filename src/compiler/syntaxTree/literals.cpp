#include "common.hpp"

#include "globals.hpp"

#include <iostream>

using namespace std::string_view_literals;


Literal::Literal(std::string_view literal_name, std::string& raw_value)
	: _literalName{literal_name}, _rawValue{raw_value}
{}


void Literal::Print(
	std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << _literalName << " = "sv << _rawValue << '\n';
}


Variable::Variable(std::string& name)
	: Literal{"Variable"sv, name}
{}


bool Variable::Scope(ScopeStack& ss, TUBuffer& src)
{
	_def = ss.Lookup(_rawValue);
	if (_def != nullptr && dynamic_cast<Function*>(_def) != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown symbol: " << _rawValue << '\n';
		HighlightError(std::cerr, src, *this);
		return false;
	}
	return true;
}


void Variable::Generate(GenData& dat, std::ostream& os)
{
	const RegT out_reg {dat._safeRegs.top()};
	_type->GenerateAccess(dat, dat._locations[_def], true, os);
}


IntLiteral::IntLiteral(std::string& value)
	: Literal{"IntLiteral"sv, value}
{}


bool IntLiteral::Validate(ValidateData& dat)
{
	try { _value = std::stoi(_rawValue); }
	catch (const std::out_of_range& e)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Ingeger literal is out of range of u32: "sv
			<< _rawValue << '\n';
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}
	return true;
}


void IntLiteral::Generate(GenData& dat, std::ostream& os)
{
	os << "\tmov\tw"sv << dat._safeRegs.top() << ",\t"sv << _value << '\n';
}


BoolLiteral::BoolLiteral(std::string& value)
	: Literal{"BoolLiteral"sv, value}
{}


bool BoolLiteral::Validate(ValidateData& dat)
{
	_value = (_rawValue == "true"sv) ? true : false;
	return true;
}


void BoolLiteral::Generate(GenData& dat, std::ostream& os)
{
	os << "\tmov\tw"sv << dat._safeRegs.top() << ",\t"sv << _value << '\n';
}


StrLiteral::StrLiteral(std::string& value)
	: Literal{"StrLiteral"sv, value}
{}


bool StrLiteral::Validate(ValidateData& dat)
{
	_value = _rawValue.substr(1, _rawValue.length() - 2);
	return true;
}


void StrLiteral::Generate(GenData& dat, std::ostream& os)
{
	const IDT label {dat.NextLabel()};
	dat._strings[label] = _value;
	RegT reg {dat._safeRegs.top()};
	os << "\tadrp\tx"sv << reg << ",\tL_"sv << label
		<< "\n\tadd\tx"sv << reg << ",\tx" << reg << ",\t:lo12:L_"sv << label
		<< '\n';
}
