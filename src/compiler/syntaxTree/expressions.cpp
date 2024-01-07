#include "expressions.hpp"

#include <iostream>

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
		case Ops::Neq:		return "!="sv;
		case Ops::Less:		return "<"sv;
		case Ops::LessEq:	return "<="sv;
		case Ops::More:		return ">"sv;
		case Ops::MoreEq:	return ">="sv;
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


void BinaryExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BinaryExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	PrintIndent(os, indent, depth);
	os << "Left Operand =\n"sv;
	PrintMaybe(_argl, os, indent, depth + 1);
	PrintIndent(os, indent, depth);
	os << "Right Operand =\n"sv;
	PrintMaybe(_argr, os, indent, depth + 1);
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


void PreExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PreExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg, os, indent, ++ depth);
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


void PostExpr::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "PostExpression(Op = "sv << GetOpText(_op) << ", "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	PrintIndent(os, indent, ++ depth);
	os << "Operand =\n"sv;
	PrintMaybe(_arg, os, indent, ++ depth);
}


Invocation::Invocation(std::string name, ArgList args)
	: _name{name}, _args{args}
{
	std::reverse(_args.begin(), _args.end());
}


bool Invocation::Scope(ScopeStack& ss, TUBuffer& src, bool first_pass)
{
	_def = dynamic_cast<Function*>(ss.Lookup(_name));
	if (_def == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Unkown function: " << _name << '\n';
		HighlightError(std::cerr, src, *this);
		return false;
	}
	return true;
}


void Invocation::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "InvokeExpression(Function = "sv << _name << ", "sv;
	_type.Print(os, indent, depth);
	os << "):\n"sv;
	++ depth;
	for (size_t i {0}; i < _args.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Arg["sv << i << "] =\n"sv;
		_args[i]->Print(os, indent, depth + 1);
	}
}
