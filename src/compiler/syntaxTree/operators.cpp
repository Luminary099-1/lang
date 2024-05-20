#include "common.hpp"

#include "globals.hpp"

#include <algorithm>
#include <iostream>
#include <utility>

using namespace std::string_view_literals;


BinaryExpr::BinaryExpr(Expression* argl, Expression* argr, Ops op)
	: _argl{argl}, _argr{argr}, _op{op}
{}


std::string_view BinaryExpr::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::LAND:		return "&&"sv;
		case Ops::LOR:		return "||"sv;
		case Ops::AND:		return "&"sv;
		case Ops::XOR:		return "^"sv;
		case Ops::OR:		return "|"sv;
		case Ops::Eq:		return "=="sv;
		case Ops::NE:		return "!="sv;
		case Ops::LT:		return "<"sv;
		case Ops::LE:		return "<="sv;
		case Ops::GT:		return ">"sv;
		case Ops::GE:		return ">="sv;
		case Ops::LShift:	return "<<"sv;
		case Ops::RShift:	return ">>"sv;
		case Ops::Add:		return "+"sv;
		case Ops::Sub:		return "-"sv;
		case Ops::Mul:		return "*"sv;
		case Ops::Div:		return "/"sv;
		case Ops::Mod:		return "%"sv;
	}

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


/**
 * @brief Prints the error message associated with incorrectly typed operand
 * expressions for binary operators.
 * 
 * @param src The buffer of the source file expressing this AST.
 * @param op The TokenInfo of the operator in question.
 * @param expr The incorrectly typed operand expression.
 * @param is_left true if the passed operand is the left operand; false
 * otherwise.
 * @param expected The name of the expected type.
 */
void ExpectedBinaryType(TU* src, TokenInfo& op, Expression* expr,
	bool is_left, std::string_view expected)
{
	std::cerr << '(' << op._row << ", "sv << op._col << "): Expected "sv
		<< ((is_left) ? "left"sv : "right"sv)
		<< " operand of type "sv << expected << ", found: "sv
		<< expr->_type->_name << '\n';
	src->HighlightError(std::cerr, op);
}


bool BinaryExpr::Validate(ValidateData& dat)
{
	bool success {_argl->Validate(dat)};
	success = _argr->Validate(dat) && success;

	switch (_op)
	{
		// Operands must both be Booleans.
		case Ops::LAND:	case Ops::LOR:
			if (!_argl->_type->IsBool())
			{
				ExpectedBinaryType(
					dat._src, *this, _argl.get(), true, "bool"sv);
				success = false;
			}
			if (!_argr->_type->IsBool())
			{
				ExpectedBinaryType(
					dat._src, *this, _argr.get(), false, "bool"sv);
				success = false;
			}
			_type = _argl->_type;
			break;

		// Operands must both be integers.
		case Ops::AND:		case Ops::XOR:		case Ops::OR:
		case Ops::LShift:	case Ops::RShift:	case Ops::Mod:
			if (!_argl->_type->IsInt())
			{
				ExpectedBinaryType(dat._src, *this, _argl.get(), true, "int"sv);
				success = false;
			}
			if (!_argr->_type->IsInt())
			{
				ExpectedBinaryType(
					dat._src, *this, _argr.get(), false, "bool"sv);
				success = false;
			}
			_type = _argl->_type;
			break;

		// Operands must be the same type.
		case Ops::Eq:		case Ops::NE:		case Ops::GT:
		case Ops::LT:		case Ops::LE:		case Ops::GE:
			if (_argl->_type != _argr->_type)
			{
				std::cerr << '(' << _row << ", "sv << _col
					<< "): Expected operands of matching types, found: "sv
					<< _argl->_type->_name << " and "sv
					<< _argr->_type->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			}
			_type = Type::Create("bool");
			break;

		// Operands must be numbers.
		case Ops::Add:	case Ops::Sub:
		case Ops::Mul:	case Ops::Div:
			if (!_argl->_type->IsInt())
			{
				ExpectedBinaryType(dat._src, *this, _argl.get(), true, "int"sv);
				success = false;
			}
			if (!_argr->_type->IsInt())
			{
				ExpectedBinaryType(
					dat._src, *this, _argr.get(), false, "bool"sv);
				success = false;
			}
			_type = _argl->_type;
			break;
	}

	_hasCall = _argl->_hasCall || _argr->_hasCall;
	_evalWeight = std::max(
		static_cast<RegT>(_argl->_evalWeight + 1), _argr->_evalWeight);
	if (_op == Ops::Mod) ++ _evalWeight;
	return success;
}


void BinaryExpr::Generate(GenData& dat, std::ostream& os)
{
	static const std::map<Ops, std::string_view> mnems
	{
		{Ops::LAND, "and"sv},	{Ops::AND, "and"sv},
		{Ops::LOR, "orr"sv},	{Ops::OR, "orr"sv},
		{Ops::XOR, "eor"sv},	{Ops::Eq, "eq"sv},
		{Ops::NE, "ne"sv},		{Ops::LT, "lt"sv},
		{Ops::LE, "le"sv},		{Ops::GT, "gt"sv},
		{Ops::GE, "ge"sv},		{Ops::LShift, "lsl"sv},
		{Ops::RShift, "lsr"sv},	{Ops::Add, "add"sv},
		{Ops::Sub, "sub"sv},	{Ops::Mul, "mul"sv},
		{Ops::Div, "sdiv"sv}
	};

	_argl->Generate(dat, os);
	const RegT out_r {dat._safeRegs.top()};
	RegT lr {out_r};
	RegT rr {out_r};
	if (_argr->_hasCall || _argr->_evalWeight > dat._safeRegs.size())
	{
		dat.GeneratePush(_argl->_type, lr, os);
		_argr->Generate(dat, os);
		dat._safeRegs.pop();
		lr = dat._safeRegs.top();
		dat.GeneratePop(_argl->_type, lr, os);
	}
	else
	{
		dat._safeRegs.pop();
		rr = dat._safeRegs.top();
		_argr->Generate(dat, os);
	}

	switch (_op)
	{
		case Ops::LAND:	case Ops::AND:	case Ops::LOR:	case Ops::OR:
		case Ops::XOR:	case Ops::Add:	case Ops::Sub:	case Ops::Mul:
		case Ops::Div:
			os << '\t' << mnems.at(_op)
				<< "\tw"sv << out_r << ",\tw"sv << lr << ",\tw"sv << rr << '\n';
			break;
		
		case Ops::LShift:	case Ops::RShift:
			os << '\t' << mnems.at(_op)
				<< "\tw"sv << lr << ",\tw"sv << rr << '\n';
			break;

		case Ops::Eq:	case Ops::NE:	case Ops::LT:
		case Ops::LE:	case Ops::GT:	case Ops::GE:
		{
			char s {(_type->GetSize() <= 4) ? 'w' : 'x'};
			os << "\tcmp\t"sv << s << lr << ",\t" << s << rr << "\n\t"sv
				<< "cset\t"sv << s << out_r << ",\t"sv << mnems.at(_op) << '\n';
			break;
		}

		case Ops::Mod:
		{
			dat._safeRegs.pop();
			RegT tr {dat._safeRegs.top()};
			os << "\tudiv\tw"sv << tr << ",\tw"sv << lr << ",\tw"sv << rr
				<< "\n\tmsub\tw"sv << out_r << ",\tw"sv << tr << ",\tw"sv << rr
				<< ",\tw"sv << lr << '\n';
			dat._safeRegs.push(tr);
			break;
		}
	}

	dat._safeRegs.push(lr);
	dat._safeRegs.push(rr);
}


void BinaryExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExpression(Op = "sv << GetOpText(_op) << ", Type = "sv
		<< _type->_name << "):\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Left Operand =\n"sv;
	_argl->Print(os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Right Operand =\n"sv;
	_argr->Print(os, indent, depth + 1);
}


PreExpr::PreExpr(Expression* arg, Ops op)
	: _arg{arg}, _op{op}
{}


std::string_view PreExpr::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::Pos: 		return "+"sv;
		case Ops::Neg: 		return "-"sv;
		case Ops::Inc: 		return "++"sv;
		case Ops::Dec: 		return "--"sv;
		case Ops::Deny: 	return "!"sv;
		case Ops::Comp: 	return "~"sv;
	}

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


bool PreExpr::Validate(ValidateData& dat)
{
	bool success {_arg->Validate(dat)};

	if (!_arg->_type->IsInt())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected operand of type int, found: "sv
			<< _arg->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}

	_type = _arg->_type;
	_hasCall = _arg->_hasCall;
	_evalWeight = _arg->_evalWeight;
	return success;
}


void PreExpr::Generate(GenData& dat, std::ostream& os)
{
	// TODO: Implement this.
	// FIXME: These operations can incorrectly be applied to rvalues.
}


void PreExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExpression(Op = "sv << GetOpText(_op)
		<< ", Type = "sv << _type->_name << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	_arg->Print(os, indent, ++ depth);
}


PostExpr::PostExpr(Expression* arg, Ops op)
	: _arg{arg}, _op{op}
{}


std::string_view PostExpr::GetOpText(Ops op)
{
	switch (op)
	{
		case Ops::Inc: return "++"sv;
		case Ops::Dec: return "--"sv;
	}

	return ""sv; // Shouldn't ever reach this but the compiler complains.
}


bool PostExpr::Validate(ValidateData& dat)
{
	bool success {_arg->Validate(dat)};

	if (!_arg->_type->IsInt())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected operand of type int, found: "sv
			<< _arg->_type->_name << '\n';
		dat._src->HighlightError(std::cerr, *this);
		success = false;
	}

	_type = _arg->_type;
	_hasCall = _arg->_hasCall;
	_evalWeight = std::max(static_cast<RegT>(2), _arg->_evalWeight);
	return success;
}


void PostExpr::Generate(GenData& dat, std::ostream& os)
{
	// TODO: Implement this.
	// FIXME: These operations can incorrectly be applied to rvalues.
}


void PostExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExpression(Op = "sv << GetOpText(_op)
		<< ", Type = "sv << _type->_name << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	_arg->Print(os, indent, ++ depth);
}


Invocation::Invocation(Identifier* name, ArgList& args)
	: _name{name}, _args{std::move(args)}
{
	std::reverse(_args.begin(), _args.end());
	_hasCall = true;
}


bool Invocation::Validate(ValidateData& dat)
{
	_type = _def->_type;
	const size_t expected_args {_def->_params.size()}; // Bigger than _evalIndex.
	if (_args.size() != expected_args)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Incorrect number of arguments in call to "sv
			<< _name->_id << ". Expected "sv << expected_args
			<< ", found "sv << _args.size() << ".\n"sv;
		dat._src->HighlightError(std::cerr, *this);
		return false;
	}
	_evalWeight = static_cast<RegT>(expected_args - 1);

	bool success {true};
	for (size_t i {0}; i < expected_args; ++ i)
	{
		Expression* arg {_args[i].get()};
		success = success && arg->Validate(dat);
		Type* expected_type {_def->_params[i]->_type};
		Type* given_type {arg->_type};
		if (*given_type != *expected_type)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected "sv << expected_type->_name << " for argument"sv
				<< i + 1 << " in call to "sv << _name->_id << ", found: "sv
				<< given_type->_name << '\n';
			dat._src->HighlightError(std::cerr, *this);
			success = false;
		}
		_hasCall = _hasCall || arg->_hasCall;
	}

	return success;
}


void Invocation::Generate(GenData& dat, std::ostream& os)
{
	const BytesT stack_args_size {dat._stackParamSizes[_def]};
	if (stack_args_size != 0)
		os << "\tsub\tsp,\tsp,\t"sv << stack_args_size << '\n';

	for (size_t i {0}; i < _args.size(); ++ i)
	{
		Parameter* param {_def->_params[i].get()};
		Location loc {dat._argLocations[param]};
		_args[i]->Generate(dat, os);
		loc.ReinterpretStack(stack_args_size);
		param->_type->GenerateAccess(dat, loc, false, os);
	}

	os << "\tbl\tF_"sv << _def->_name->_id << '\n';
	if (stack_args_size != 0)
		os << "\tadd\tsp,\tsp,\t"sv << stack_args_size << '\n';

	if (_type->IsVoid()) return;
	const char s {(_type->GetSize() <= 4) ? 'w' : 'x'};
	os << "\tmov\t"sv << s << dat._safeRegs.top() << ",\t" << s << "0\n"sv;
}


void Invocation::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "InvokeExpression(Function = "sv << _name->_id
		<< ", Type = "sv << _type->_name << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _args.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Arg["sv << i << "] =\n"sv;
		_args[i]->Print(os, indent, depth + 1);
	}
}
