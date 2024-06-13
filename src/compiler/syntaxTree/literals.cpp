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


void Variable::Generate(GenData& dat, std::ostream& os)
{}


IntLiteral::IntLiteral(std::string& value)
	: Literal{"IntLiteral"sv, value}
{}


bool IntLiteral::Validate(ValidateData& dat)
{
	try { _value = std::stoi(_rawValue); }
	catch ([[maybe_unused]] const std::out_of_range& e)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Ingeger literal is out of range of u32: "sv
			<< _rawValue << '\n';
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	return true;
}


void IntLiteral::Generate(GenData& dat, std::ostream& os)
{}


BoolLiteral::BoolLiteral(std::string& value)
	: Literal{"BoolLiteral"sv, value}
{}


bool BoolLiteral::Validate(ValidateData& dat)
{
	_value = (_rawValue == "true"sv) ? true : false;
	return true;
}


void BoolLiteral::Generate(GenData& dat, std::ostream& os)
{}


StrLiteral::StrLiteral(std::string& value)
	: Literal{"StrLiteral"sv, value}
{}


bool StrLiteral::Validate(ValidateData& dat)
{
	_value = _rawValue.substr(1, _rawValue.length() - 2);
	return true;
}


void StrLiteral::Generate(GenData& dat, std::ostream& os)
{}
