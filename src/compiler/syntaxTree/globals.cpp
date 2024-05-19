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


Function::Function(
	Identifier* name, ParamList& params, Type* type, StmtList body)
	: Declaration{name}, _params{std::move(params)}
		, _type{type}, _body{std::move(body)}
{
	std::reverse(_params.begin(), _params.end());
	std::reverse(_body.begin(), _body.end());
}


BytesT Function::MarshalParams(GenData& dat)
{
	// Stage A:
	RegT ngrn {0};		// Next General-purpose Register Number.
	// RegT nsrn {0};	// Next SIMD and floating-point Register Number.
	// RegT nprn {0};	// Next Scalable Predicate Register Number.
	BytesT nsaa {0};	// Next Stacked Argument Address (relative to SP).

	// Stage B - Unimplemented, appears irrelevant for now.

	// Stage C:
	for (size_t i {0}; i < _params.size(); ++ i)
	{
		Parameter* param {_params[i].get()};
		Type* p_type {param->_type};
		Location loc;

		// C1 - Unimplemented as there is no floating-point support yet.
		// C2 - Unimplemented as there is no floating-point support yet.
		// C3 - Unimplemented as there is no floating-point support yet.
		// C4 - Unimplemented as there is no floating-point support yet.
		// C5 - Unimplemented as there is no floating-point support yet.
		// C6 - Unimplemented as there is no floating-point support yet.
		// C7 - Unimplemented as there are no pure scalable types yet.
		// C8 - Unimplemented as there are no pure scalable types yet.

		// C9 - Allocate Integral or Pointer types of dword size or less:
		if ((p_type->IsIntegral() || p_type->IsPointer())
			&& p_type->GetSize() <= 8 && ngrn < 8)
		{
			loc = Location::CreateRegister(p_type, ngrn);
			++ ngrn;
			goto store_locations; // The parameter is allocated.
		}

		// C10 - Unimplemented as there are no types with qword alignment yet.
		// C11 - Unimplemented as there are no types with qword size yet.
		// C12 - Unimplemented as there are no composite types yet.

		ngrn = 8; // C13.
		nsaa &= -8; // C14 - Partial: No types have qword natural alignment yet.

		// C15 - Unimplemented as there are no composite types yet.

		// C16 + 17: Add 16 bytes to accommodate FP and LR saving.
		loc = Location::CreateLocal(p_type, nsaa + 16);
		nsaa += std::max(p_type->GetSize(), static_cast<BytesT>(8));

	store_locations:
		dat._argLocations[param] = loc;
		dat._locations[param] = loc;
	}

	return nsaa;
}


bool Function::Validate(ValidateData& dat)
{
	bool success {true};
	dat._curFunc = this;
	const bool returns {Statement::ValidateAndGetReturn(
		_body, dat, success, nullptr, &_hasCall)};
	dat._curFunc = nullptr;

	if (!_type->IsVoid() && !returns)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Non-void function does not return in all control paths: \n"sv
			<< _name->_id << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}

	dat._curFunc = nullptr;
	return success;
}


void Function::Generate(GenData& dat, std::ostream& os)
{
	dat._strings.clear();
	dat._stackParamSizes[this] = MarshalParams(dat);

	// Deferred output for register argument remapping.
	std::stringstream remap_os;
	// Accumulates the size of remapped arguments.
	BytesT remap_off {0};
	// Remap register parameters to be locals if non-terminal:
	if (_hasCall) for (size_t i {0}; i < _params.size(); ++ i)
	{
		Parameter* param {_params[i].get()};
		Location old_loc {dat._locations[param]};
		if (old_loc.GetPlace() != Location::Place::Register) continue;
		dat._locations.erase(param);
		remap_off += param->_type->GetSize();
		dat.GeneratePush(param->_type, old_loc._val._reg, remap_os);
		dat._locations[param] = Location::CreateLocal(param->_type, -remap_off);
	}

	// Generate defferred output for the body:
	dat._isGlobal = false;
	std::stringstream dos; // Deferred output for the body.
	for (size_t i {0}; i < _body.size(); ++ i) _body[i]->Generate(dat, dos);
	dat._isGlobal = true;

	// Allocate space for callee-saved registers and round to 16 bytes.
	const BytesT reg_save_off {dat._frameSize + 16}; // Locals + FP and LR.
	const BytesT frame_size {(reg_save_off + 80) & -16}; // Callee-saved & round.

	// Output string constants:
	for (std::pair<IDT, std::string> c : dat._strings)
		os << "S_"sv << c.first << ":\t.string\t\""sv << c.second << "\"\n"sv;

	// Output procedure header:
	os << ".balign\t4"
		<< "F_"sv << _name << ":\n"	// Procedure call label.
		"\tstp\tfp,\tlr,\t[sp, "sv << -frame_size << "]!\n"
		"\tmov\tfp,\tsp\n"
		"\tstp\tx19,\tx20,\t[fp, "sv << (reg_save_off + 16) << "]\n"
		"\tstp\tx21,\tx22,\t[fp, "sv << (reg_save_off + 32) << "]\n"
		"\tstp\tx23,\tx24,\t[fp, "sv << (reg_save_off + 48) << "]\n"
		"\tstp\tx25,\tx26,\t[fp, "sv << (reg_save_off + 64) << "]\n"
		"\tstp\tx27,\tx28,\t[fp, "sv << (reg_save_off + 80) << "]\n"sv;

	// Output any remapped register arguments and the body's code:
	os << remap_os.rdbuf() << dos.rdbuf();

	// Reclaim stack space occupied by remapped arguments:
	if (remap_off > 0) os << "\tadd\tsp,\tsp,\t"sv << remap_off << '\n';

	// Output procedure footer:
	os << "R_"sv << _name << ":\n"	// Procedure return label.
		"\tldp\tx19,\tx20,\t[fp, "sv << (reg_save_off + 16) << "]\n"
		"\tldp\tx21,\tx22,\t[fp, "sv << (reg_save_off + 32) << "]\n"
		"\tldp\tx23,\tx24,\t[fp, "sv << (reg_save_off + 48) << "]\n"
		"\tldp\tx25,\tx26,\t[fp, "sv << (reg_save_off + 64) << "]\n"
		"\tldp\tx27,\tx28,\t[fp, "sv << (reg_save_off + 80) << "]\n"
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
