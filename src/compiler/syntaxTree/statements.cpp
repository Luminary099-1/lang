#include "statements.hpp"

#include "constructs.hpp"
#include "function.hpp"

#include <iostream>

using namespace std::string_view_literals;


Type* Expression::GetYieldType(Statement* stmt)
{
	CompoundStmt* comp {dynamic_cast<CompoundStmt*>(stmt)};
	if (comp != nullptr)
	{
		if (comp->_stmts.size() == 0) return Type::Create("void");
		else return GetYieldType(comp->_stmts[comp->_stmts.size() - 1].get());
	}

	Expression* expr {dynamic_cast<Expression*>(stmt)};
	if (expr != nullptr) return expr->_type;
}


CompoundStmt::CompoundStmt(StmtList kids)
	: _stmts{kids}
{
	std::reverse(_stmts.begin(), _stmts.end());
}


bool CompoundStmt::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	ss.Enter();
	for (size_t i {0}; i < _stmts.size(); ++ i)
		success = _stmts[i]->Scope(ss, src) && success;
	ss.Exit();
	return success;
}


bool CompoundStmt::Validate(ValidateData& dat)
{
	dat._ts.push_front(this);
	bool success {true};
	Statement* last {_stmts[_stmts.size() - 1].get()};
	for (size_t i {0}; i < _stmts.size(); ++ i)
	{
		Statement* cur {_stmts[i].get()};
		if (cur != last && dynamic_cast<ExprStmt*>(cur) != nullptr)
		{
			std::cerr << '(' << _row << ", "sv << _col
				<< "): Statement yields value before the block ends.\n"sv;
			HighlightError(std::cerr, dat._src, *this);
			success = false;
		}
		success = cur->Validate(dat) && success;
	}
	dat._ts.pop_front();

	Expression* last_expr {dynamic_cast<Expression*>(last)};
	if (last_expr != nullptr) _type = last_expr->_type;

	return success;
}


void CompoundStmt::Print(
	std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "CompoundStatement:\n"sv;
	++ depth;
	for (size_t i {0}; i < _stmts.size(); ++ i)
	{
		PrintIndent(os, indent, depth);
		os << "Statement["sv << i << "] ->\n"sv;
		_stmts[i]->Print(os, indent, depth + 1);
	}
}


VariableDef::VariableDef(Type* type, std::string name, Expression* init)
	: _type{type}, _name{name}, _init{init}
{}


bool VariableDef::Scope(ScopeStack& ss, TUBuffer& src)
{
	bool success {true};
	TokenInfo* pre_def {dynamic_cast<TokenInfo*>(ss.Define(_name, this))}; // Trusting this for now.
	if (pre_def != nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Symbol collision: "sv << _name
			<< "\n# The following on line"sv << _row << ":\n";
		HighlightError(std::cerr, src, *this);
		std::cerr << "# Redefines on line "sv << pre_def->_row << ":\n";
		HighlightError(std::cerr, src, *pre_def);
		success = false;
	}
	return _init->Scope(ss, src) && success;
}


bool VariableDef::Validate(ValidateData& dat)
{
	dat._ts.push_front(this);
	bool success {_init->Validate(dat)};
	dat._ts.pop_front();

	if (*_init->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected initializer of type "sv << _type->_name
			<< ", found: "sv << _init->_type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
		success = false;
	}

	return success;
}


void VariableDef::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "VariableInitialization(_name = "sv << _name << ", "sv;
	_type->Print(os, indent, depth);
	os << "):\n"sv;
	_init->Print(os, indent, ++ depth);
}


ExprStmt::ExprStmt(Expression* expr)
	: _expr{expr}
{}


bool ExprStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool ExprStmt::Validate(ValidateData& dat)
{
	dat._ts.push_front(this);
	bool success {_expr->Validate(dat)};
	dat._ts.pop_front();
	return success;
}


void ExprStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ExpressionStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


BreakStmt::BreakStmt(Expression* expr, int levels)
	: _expr{expr}, _levels{levels}
{}


bool BreakStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool BreakStmt::Validate(ValidateData& dat)
{
	dat._ts.push_front(this);
	bool success {_expr->Validate(dat)};
	dat._ts.pop_front();

	Breakable* expr {nullptr};
	int broken {0};
	for (SyntaxTreeNode* node : dat._ts)
	{
		expr = dynamic_cast<Breakable*>(node);
		if (expr != nullptr)
		{
			broken ++;
			if (broken == _levels) break;
		}
	}

	if (broken < _levels)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Break exceeds breakable depth: "sv << _levels << '\n';
		HighlightError(std::cerr, dat._src, *this);
	}
	else if (broken == _levels)
	{
		_target = expr;
		_target->_type = _expr->_type;
	}

	return success;
}


void BreakStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "BreakStatement(Levels = "sv << _levels << "):\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}


ReturnStmt::ReturnStmt(Expression* expr)
	: _expr{expr}
{}


bool ReturnStmt::Scope(ScopeStack &ss, TUBuffer &src)
{
	return _expr->Scope(ss, src);
}


bool ReturnStmt::Validate(ValidateData& dat)
{
	dat._ts.push_front(this);
	bool success {_expr->Validate(dat)};
	dat._ts.pop_front();

	Function* parent {nullptr};
	for (SyntaxTreeNode* node : dat._ts)
	{
		parent = dynamic_cast<Function*>(node);
		if (parent != nullptr) break;
	}
	
	if (parent == nullptr)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Return statement outside of function body.\n"sv;
		HighlightError(std::cerr, dat._src, *this);
	}

	if (*parent->_type != *_type)
	{
		std::cerr << '(' << _row << ", "sv << _col
			<< "): Expected return value of type "sv << parent->_type->_name
			<< ", found: "sv << _type->_name << '\n';
		HighlightError(std::cerr, dat._src, *this);
	}

	return success;
}


void ReturnStmt::Print(std::ostream& os, std::string_view indent, int depth)
{
	PrintIndent(os, indent, depth);
	os << "ReturnStatement:\n"sv;
	PrintMaybe(_expr.get(), os, indent, ++ depth);
}
