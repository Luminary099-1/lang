#include "globals.hpp"

#include "../utilities.hpp"

#include <algorithm>
#include <iostream>

using namespace std::string_view_literals;


Parameter::Parameter(Type* type, Identifier* name)
	: Declaration{name}, _type{type}
{}


void Parameter::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Parameter(ID = "sv << _name->_id
		<< ", Type = "sv << _type->_name << ")\n"sv;
}


void Parameter::Generate(GenData& dat, std::ostream& os)
{
	dat._lastSize = _type->GetSize();
}


Function::Function(
	Identifier* name, ParamList& params, Type* type, StmtList body)
	: Declaration{name}, _params{std::move(params)}
		, _type{type}, _body{std::move(body)}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


void Function::AllocateParams(GenData& dat)
{
	constexpr size_t reg_count {8}; // The number of parameter registers.
	
	// Map the first 7 arguments to x0-x7.
	for (size_t i {0}; i < std::min(_params.size(), reg_count); ++ i)
		dat._locations.emplace(
			std::pair(_params[i].get(), GenData::VarLocation(false, i)) );

	BytesT stack_off {0}; // The offset before the FP.
	// Maps the arguments after the first 7 to the stack before the FP.
	for (size_t i {_params.size() - 1}; i >= reg_count; -- i)
	{
		Parameter* param {_params[i].get()};
		stack_off += dat._lastSize;
		dat._locations.emplace(
			std::pair(param, GenData::VarLocation(true, stack_off)) );
	}
}


bool Function::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {Declaration::Scope(ss, src)};

	ss.Enter();
	for (size_t i {0}; i < _params.size(); ++ i)
		success = _params[i]->Scope(ss, src) && success;
	for (size_t i {0}; i < _body.size(); ++ i)
		success = _body[i]->Scope(ss, src) && success;
	ss.Exit();
	return success;
}


bool Function::Validate(ValidateData& dat)
{
	bool success {true};
	dat._curFunc = this;
	const bool returns {Statement::ValidateAndGetReturn(_body, dat, success)};
	dat._curFunc = nullptr;

	if (!_type->IsVoid() && !returns)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Non-void function does not return in all control paths: \n"sv
			<< _name->_id << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}

	return success;
}


void Function::Generate(GenData& dat, std::ostream& os)
{
	AllocateParams(dat);

	// Determine the frame size and capture procedure body output:
	BytesT frame_size {0};
	BytesT max_compound_size {0};
	std::stringstream bos; // Temporary output for the body.
	for (size_t i {0}; i < _body.size(); ++ i)
	{
		Statement* child {_body[i].get()};
		child->Generate(dat, bos);
		if (dynamic_cast<CompoundStmt*>(child) != nullptr
			&& dat._lastSize > max_compound_size)
			max_compound_size = dat._lastSize;
		else frame_size += dat._lastSize;
	}
	// Add the largest compound sub-frame size and space for saved registers.
	frame_size += max_compound_size + (12 * 8);
	frame_size &= 16;	// Round to the nearest 16 bytes.

	// Procedure header:
	os << _name << ":\n"	// Procedure call label.
		"\tstp\tfp,\tlr,\t[sp, "sv << -frame_size << "]!\n"
		"\tmov\tfp,\tsp\n"
		"\tstp\tx19,\tx20,\t[fp, 16]\n"
		"\tstp\tx21,\tx22,\t[fp, 32]\n"
		"\tstp\tx23,\tx24,\t[fp, 48]\n"
		"\tstp\tx25,\tx26,\t[fp, 64]\n"
		"\tstp\tx27,\tx28,\t[fp, 80]\n"sv;

	// Output the body's code:
	os << bos.rdbuf();

	// Procedure footer:
	os << "r_"sv << _name << ":\n"	// Procedure return label.
		"\tldp\tx19,\tx20,\t[fp, 16]\n"
		"\tldp\tx21,\tx22,\t[fp, 32]\n"
		"\tldp\tx23,\tx24,\t[fp, 48]\n"
		"\tldp\tx25,\tx26,\t[fp, 64]\n"
		"\tldp\tx27,\tx28,\t[fp, 80]\n"
		"\tldp\tfp,\tlr\t[sp],\t"sv << frame_size << '\n'
		<< "\tret\n"sv;
}


void Function::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "Function(ID = "sv << _name->_id
		<< ", Type = "sv << _type->_name << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _params.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Parameters["sv << i << "](ID = "sv << _params[i]->_name->_id
			<< ", Type = "sv << _params[i]->_type->_name << ")\n"sv;
	}
	for (size_t i {0}; i < _body.size(); ++ i)
		_body[i]->Print(os, indent, depth);
}
