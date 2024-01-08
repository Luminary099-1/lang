#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <memory>
#include <string>


// Represents an assignment expression.
struct AssignmentExpr
	: public Expression
{
	std::string _name;					// The variable being assigned to.
	std::unique_ptr<Expression> _expr;	// The expression being assigned.

	/**
	 * @brief Construct a new AssignmentExpr object.
	 * 
	 * @param name The name of the variable being assigned.
	 * @param expr The expression being assigned to the variable.
	 */
	AssignmentExpr(std::string name, Expression* expr);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an if expression (if statement that can return values).
struct IfExpr
	: public Expression
{
	std::unique_ptr<Expression> _cond;	// A conditional expression.
	std::unique_ptr<Statement> _body;	// The statement executed on true.
	std::unique_ptr<Statement> _alt;	// The optional else case.

	/**
	 * @brief Construct a new IfExpr object;
	 * 
	 * @param cond A condition expression.
	 * @param body A statement executed on true.
	 * @param alt The else case if the condition is false.
	 */
	IfExpr(Expression* cond, Statement* body, Statement* alt);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a counting for expression (a for loop that can return values).
struct ForExpr
	: public Expression
{
	std::unique_ptr<Expression> _init;	// Runs before the loop.
	std::unique_ptr<Expression> _cond;	// Tests before iterations.
	std::unique_ptr<Expression> _inc;	// Runs after the condition.
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new ForExpr object.
	 * 
	 * @param init An initialization expression.
	 * @param cond A conditional expression.
	 * @param inc An increment expression.
	 * @param body The statement executed each iteration.
	 */
	ForExpr(Expression* init, Expression* cond, Expression* inc, Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an infinite loop expression (a loop that can return values).
struct LoopExpr
	: public Expression
{
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new LoopExpr object;
	 * 
	 * @param body The statement executed each iteration.
	 */
	LoopExpr(Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a while loop expression (a while loop that can return a value).
struct WhileExpr
	: public Expression
{
	std::unique_ptr<Expression> _cond;	// Tests before iterations.
	std::unique_ptr<Statement> _body;	// Executed each iteration.

	/**
	 * @brief Construct a new WhileExpr object.
	 * 
	 * @param cond A conditional expression.
	 * @param body The statement executed each iteration.
	 */
	WhileExpr(Expression* cond, Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
