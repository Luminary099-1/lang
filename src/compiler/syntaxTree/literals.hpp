#pragma once

#include "statements.hpp"

#include <string>
#include <string_view>

using namespace std::string_view_literals;


// Base class to represent literals.
struct Literal
	: public Expression
{
	std::string_view _literalName;	// The name of the literal's node.
	std::string _rawValue;			// The raw value obtained from the source.

	/**
	 * @brief Construct a new Literal object.
	 * 
	 * @param literal_name
	 * @param raw_value The value described by the literal sans type descriptors.
	 */
	Literal(std::string_view literal_name, std::string& raw_value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo should refer to the entire literal.
};


// Represents identifiers.
struct Variable
	: public Literal
{
	SyntaxTreeNode* _def;	// The AST node that defines this variable.

	/**
	 * @brief Construct a new Variable object.
	 * 
	 * @param name The variable's identifier.
	 */
	Variable(std::string& name);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	bool Validate(ValidateData& dat) override;

	// TokenInfo refers to the identifier itself.
};


// Represents integer literals.
struct IntLiteral
	: public Literal
{
	int32_t _value {0};	// Concrete literal value.

	/**
	 * @brief Construct a new IntLiteral object.
	 * 
	 * @param raw_val The literal's integer value.
	 */
	IntLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;

	// TokenInfo refers to the value itself.
};


// Represents Boolean literals.
struct BoolLiteral
	: public Literal
{
	bool _value {false};	// Concrete literal value.

	/**
	 * @brief Construct a new BoolLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	BoolLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;

	// TokenInfo refers to the value itself.
};


// Represents string literals.
struct StrLiteral
	: public Literal
{
	std::string _value;	// Concrete literal value.

	/**
	 * @brief Construct a new StrLiteral object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	StrLiteral(std::string& value);

	bool Validate(ValidateData& dat) override;

	// TokenInfo refers to the value itself.
};
