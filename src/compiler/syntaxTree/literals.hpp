#pragma once

#include "statements.hpp"

#include <string>


// Base class to represent literals.
struct Literal : public Expression, public TokenInfo
{};


// Represents identifiers.
struct Identifier : public Literal
{
	std::string _value;		// The identifier's name.
	SyntaxTreeNode* _def;	// The AST node that defines this variable.

	/**
	 * @brief Construct a new Identifier object.
	 * 
	 * @param value The identifier's name.
	 */
	Identifier(std::string value);

	bool Scope(ScopeStack& ss, TUBuffer& src, bool first_pass) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents integer literals.
struct IntLiteral : public Literal
{
	int _value;	// The literal's integer value.

	/**
	 * @brief Construct a new IntLiteral object.
	 * 
	 * @param value The literal's integer value.
	 */
	IntLiteral(int value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents Boolean literals.
struct BoolLiteral : public Literal
{
	bool _value;	// The literal's Boolean value.

	/**
	 * @brief Construct a new BoolLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	BoolLiteral(bool value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents string literals.
struct StrLiteral : public Literal
{
	std::string _value;	// The literal's string value.

	/**
	 * @brief Construct a new StrLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	StrLiteral(std::string value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
