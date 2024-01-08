#pragma once

#include "base.hpp"

#include <memory>
#include <vector>


// Base class to represent statements.
struct Statement
	: public SyntaxTreeNode
{};


// Stores a list of statements.
using StmtList = std::vector<Statement*>;


// Base class to represent expressions.
struct Expression
	: public Statement, public TokenInfo
{
	Type* _type {nullptr};	// The expression's type.
};


// Represents an expression statement (does not return its evaluation).
struct ExprStmt
	: public Statement
{
	std::unique_ptr<Expression> _expr;	// The contained expression.

	/**
	 * @brief Construct a new ExprStmt object.
	 * 
	 * @param expr The expression to be contained.
	 */
	ExprStmt(Expression* expr);
	
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a variable definition (including initialization).
struct VariableDef
	: public Statement, public TokenInfo
{
	Type* _type {nullptr};				// The variable's type.
	std::string _name;					// The variable's symbolic name.
	std::unique_ptr<Expression> _init;	// The variable's initializer.

	/**
	 * @brief Construct a new VariableDef object.
	 * 
	 * @param type The variable's type.
	 * @param name The variable's name.
	 * @param init The variable's initialization expression.
	 */
	VariableDef(Type* type, std::string name, Expression* init);

	bool Scope(ScopeStack& ss, TUBuffer& src, bool first_pass) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a break statement.
struct BreakStmt
	: public Expression
{
	int _levels {1};					// The number of statements to break.
	std::unique_ptr<Expression> _expr;	// An optional return expression.

	/**
	 * @brief Construct a new BreakStmt object.
	 * 
	 * @param expr The number of statements to break.
	 * @param levels The expression to be returned.
	 */
	BreakStmt(Expression* expr, int levels = 1);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


struct ReturnStmt
	: public Expression
{
	std::unique_ptr<Expression> _expr;	// An optional return expression.

	/**
	 * @brief Construct a new ReturnStmt object.
	 * 
	 * @param expr An optional return expression.
	 */
	ReturnStmt(Expression* expr);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents compound statements.
struct CompoundStmt
	: public Expression
{
	StmtList _stmts;	// This node's children statements.
	
	/**
	 * @brief Construct a new CompoundStmt object.
	 * 
	 * @param kids This node's children statements. Assumed to be in reverse 
	 * order after being parsed.
	 */
	CompoundStmt(StmtList kids);

	bool Scope(ScopeStack& ss, TUBuffer& src, bool first_pass) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
