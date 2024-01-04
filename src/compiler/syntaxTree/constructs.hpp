#pragma once

#include "base.hpp"
#include "statements.hpp"

#include <string>


// Represents an assignment expression.
struct AssignmentExpr : public Expression
{
	std::string _name;	// The name of the variable being assigned.
	Expression* _expr;	// The expression being assigned to the variable.

	/**
	 * @brief Construct a new AssignmentExpr object.
	 * 
	 * @param name The name of the variable being assigned.
	 * @param expr The expression being assigned to the variable.
	 */
	AssignmentExpr(std::string name, Expression* expr);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an if expression (if statement that can return values).
struct IfExpr : public Expression
{
	Expression* _cond;	// The condition expression. Must be a Boolean.
	Statement* _body;	// A statement executed if the condition is true.
	Statement* _alt;		// The else case if the condition is false (optional).

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
struct ForExpr : public Expression
{
	Expression* _init;	// Initialization expression; runs before the loop.
	Expression* _cond;	// Conditional expression; runs between iterations.
	Expression* _inc;		// Increment expression; runs after the condition.
	Statement* _body;	// The statement to be executed each iteration.

	/**
	 * @brief Construct a new ForExpr object.
	 * 
	 * @param init An initialization expression.
	 * @param cond A condition expression.
	 * @param inc An increment expression.
	 * @param body The statement executed each iteration.
	 */
	ForExpr(Expression* init, Expression* cond, Expression* inc, Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents an infinite loop expression (a loop that can return values).
struct LoopExpr : public Expression
{
	Statement* _body;	// The statement to be executed each iteration.

	/**
	 * @brief Construct a new LoopExpr object;
	 * 
	 * @param body The statement executed each iteration.
	 */
	LoopExpr(Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents a while loop expression (a while loop that can return a value).
struct WhileExpr : public Expression
{
	Expression* _cond;	// Conditional expression; runs between iterations.
	Statement* _body;	// // The statement to be executed each iteration.

	/**
	 * @brief Construct a new WhileExpr object.
	 * 
	 * @param cond A condition expression.
	 * @param body The statement executed each iteration.
	 */
	WhileExpr(Expression* cond, Statement* body);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
