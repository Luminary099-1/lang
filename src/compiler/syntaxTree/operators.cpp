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
		<< (is_left ? "left"sv : "right"sv)
		<< " operand of type "sv << expected << ", found: "sv
		<< expr->_type->_name << '\n';
	src->HighlightError(std::cerr, op);
}


bool BinaryExpr::Scope(SymTab& symbols, TU& tu)
{
	const bool success {_argl->Scope(symbols, tu)};
	return _argr->Scope(symbols, tu) && success;
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
	return success;
}


void BinaryExpr::Generate(GenData& dat, std::ostream& os)
{}


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


bool PreExpr::Scope(SymTab& symbols, TU& tu)
{
	return _arg->Scope(symbols, tu);
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
	return success;
}


void PreExpr::Generate(GenData& dat, std::ostream& os)
{
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


bool PostExpr::Scope(SymTab& symbols, TU& tu)
{
	return _arg->Scope(symbols, tu);
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
	return success;
}


void PostExpr::Generate(GenData& dat, std::ostream& os)
{
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


bool Invocation::Scope(SymTab& symbols, TU& tu)
{
	bool success {true};
	_def = dynamic_cast<Function*>(symbols.Lookup(_name->_id));
	if (_def == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unknown function: "sv << _name->_id << '\n';
		tu.HighlightError(std::cerr, *_name);
		success = false;
	}

	for (size_t i {0}; i < _args.size(); ++ i)
		success = _args[i]->Scope(symbols, tu) && success;	

	return success;
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
{}


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
