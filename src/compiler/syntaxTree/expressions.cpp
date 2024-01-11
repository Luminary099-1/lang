#include "expressions.hpp"

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


bool BinaryExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	// Kinda ugly to avoid short-circuit evaluation.
	bool success {_argl->Scope(ss, src)};
	return _argr->Scope(ss, src) && success;
}


/**
 * @brief 
 * TODO:
 * @param src 
 * @param op 
 * @param expr 
 * @param is_left 
 */
void ExpectedBinaryType(TUBuffer& src, TokenInfo& op, Expression* expr,
	bool is_left, std::string_view expected)
{
	std::cerr << '(' << op._row << ", "sv << op._col << "): Expected "sv
		<< ((is_left) ? "left"sv : "right"sv)
		<< " operand of type "sv << expected << ", found: "sv
		<< expr->_type->_name << '\n';
	HighlightError(std::cerr, src, op);
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
					<< "): Expected operands of matching types, found: "
					<< _argl->_type->_name << ", "sv
					<< _argr->_type->_name << '\n';
				HighlightError(std::cerr, dat._src, *this);
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
	return success;
}


void BinaryExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Left Operand =\n"sv;
	PrintMaybe(_argl.get(), os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Right Operand =\n"sv;
	PrintMaybe(_argr.get(), os, indent, depth + 1);
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


bool PreExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	return _arg->Scope(ss, src);
}


bool PreExpr::Validate(ValidateData& dat)
{
	bool success {_arg->Validate(dat)};

	if (!_arg->_type->IsInt())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected operand of type int, found: "sv
			<< _arg->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	_type = _arg->_type;
	return success;
}


void PreExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg.get(), os, indent, ++ depth);
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


bool PostExpr::Scope(ScopeStack& ss, TUBuffer& src)
{
	return _arg->Scope(ss, src);
}


bool PostExpr::Validate(ValidateData& dat)
{
	bool success {_arg->Validate(dat)};

	if (!_arg->_type->IsInt())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected operand of type int, found: "sv
			<< _arg->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}
	_type = _arg->_type;
	return success;
}


void PostExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg.get(), os, indent, ++ depth);
}


Invocation::Invocation(std::string name, ArgList& args)
	: _name{name}, _args{std::move(args)}
{
	std::reverse(_args.begin(), _args.end());
}


bool Invocation::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	_def = dynamic_cast<Function*>(ss.Lookup(_name));
	if (_def == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown function: " << _name << '\n';
		HighlightError(std::cerr, src, *this);
		success = false;
	}

	for (size_t i {0}; i < _args.size(); ++ i)
		success = _args[i]->Scope(ss, src) && success;

	return success;
}


bool Invocation::Validate(ValidateData& dat)
{
	_type = _def->_type;
	size_t expected_args {_def->_params.size()};
	if (_args.size() != _def->_params.size())
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected "sv << expected_args << ", found: "sv
			<< _args.size() << '\n';
		HighlightError(std::cerr, dat._src, *this);
		return false;
	}

	bool success {true};
	for (size_t i {0}; i < expected_args; ++ i)
	{
		success = success && _args[i]->Validate(dat);
		Type* expected_type {_def->_params[i]->_type};
		Type* given_type {_args[i]->_type};
		if (*given_type != *expected_type)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Expected "sv << expected_type->_name << " for argument"sv
				<< i + 1 << " in call to "sv << _name << ", found: "sv
				<< given_type->_name << '\n';
			HighlightError(std::cerr, dat._src, *_args[i]);
		}
		success = false;
	}

	return success;
}


void Invocation::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "InvokeExpression(Function = "sv << _name << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _args.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Arg["sv << i << "] =\n"sv;
		_args[i]->Print(os, indent, depth + 1);
	}
}
