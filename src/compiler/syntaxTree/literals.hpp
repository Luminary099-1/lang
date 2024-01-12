#pragma once

#include "statements.hpp"

#include <string>


// Base class to represent literals.
struct Literal
	: public Expression
{};


// Represents identifiers.
struct Variable
	: public Literal
{
	std::string _value;		// The identifier's name.
	SyntaxTreeNode* _def;	// The AST node that defines this variable.

	/**
	 * @brief Construct a new Variable object.
	 * 
	 * @param value The identifier's name.
	 */
	Variable(std::string& value);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the identifier itself.
};


// Represents integer literals.
struct IntLiteral
	: public Literal
{
	int _value;	// The literal's integer value.

	/**
	 * @brief Construct a new IntLiteral object.
	 * 
	 * @param value The literal's integer value.
	 */
	IntLiteral(int& value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the value itself.
};


// Represents Boolean literals.
struct BoolLiteral
	: public Literal
{
	bool _value;	// The literal's Boolean value.

	/**
	 * @brief Construct a new BoolLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	BoolLiteral(bool& value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the value itself.
};


// Represents string literals.
struct StrLiteral
	: public Literal
{
	std::string _value;	// The literal's string value.

	/**
	 * @brief Construct a new StrLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	StrLiteral(std::string& value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the value itself.
};
