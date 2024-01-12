#pragma once

#include "base.hpp"

#include <memory>
#include <vector>


struct Statement;

// Stores a list of statements.
using StmtList = std::vector<std::unique_ptr<Statement>>;

// Base class to represent statements.
struct Statement
	: public virtual SyntaxTreeNode
{
	bool _hasReturn {false};	// Indicates whether all control paths return.

	/**
	 * @brief Determines whether the passed list of statements contains a
	 * return statement in a valid place (at the end of all control paths).
	 * 
	 * @param stmts The list of statements to consider.
	 * @param dat An instance of ValidateData to store the state of the
	 * validation.
	 * @param success Reference to a Boolean that will be updated with the
	 * success of the validations performed. A false value will not be set true.
	 * @return true if the statements return in a valid way; false otherwise.
	 */
	static bool
		ValidateAndGetReturn(StmtList& stmts, ValidateData& dat, bool& success);

	// TokenInfo should refer to the entire statement in extending classes.
};


// Abstract expression.
struct Expression
	: public Statement
{
	Type* _type {Type::Create("void")};	// The expression's type.
};


// Abstract representation of expressions that can be broken.
struct Breakable
	: public Expression
{};


// Represents a variable definition (including initialization).
struct VariableDef
	: public virtual Statement, public Declaration
{
	Type* _type;						// The variable's type.
	std::unique_ptr<Expression> _init;	// The variable's initializer.

	/**
	 * @brief Construct a new VariableDef object.
	 * 
	 * @param type The variable's type.
	 * @param name The variable's identifier.
	 * @param init The variable's initialization expression.
	 */
	VariableDef(Type* type, Identifier* name, Expression* init);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the equal symbol.
};


// Represents a break statement.
struct BreakStmt
	: public Statement
{
	int _levels {1};					// The number of statements to break.
	Breakable* _target {nullptr};		// The targetted breakable expression.
	std::unique_ptr<Expression> _expr;	// An optional return expression.

	/**
	 * @brief Construct a new BreakStmt object.
	 * 
	 * @param expr The number of statements to break.
	 * @param levels The expression to be returned.
	 */
	BreakStmt(Expression* expr, int levels = 1);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the break keyword.
};


struct ReturnStmt
	: public Statement
{
	std::unique_ptr<Expression> _expr;	// An optional return expression.

	/**
	 * @brief Construct a new ReturnStmt object.
	 * 
	 * @param expr An optional return expression.
	 */
	ReturnStmt(Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the return keyword.
};


// Represents compound statements.
struct CompoundStmt
	: public Expression
{
	StmtList _stmts;					// This node's children statements.
	std::unique_ptr<Expression> _expr;	// An optional return expression.
	
	/**
	 * @brief Construct a new CompoundStmt object.
	 * 
	 * @param stmts This node's children statements. Assumed to be in reverse 
	 * order after being parsed.
	 * @param expr An optional return expression, declared as the last statement
	 * without a semicolon.
	 */
	CompoundStmt(StmtList stmts, Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the inclusive span between curly braces.
};
