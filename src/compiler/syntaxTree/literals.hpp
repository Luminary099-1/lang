#pragma once

#include "statements.hpp"

#include <string>


// Base class to represent literals.
struct LiteralNode : public ExprNode {};


// Represents identifiers.
struct IDNode : public LiteralNode
{
	std::string _value;	// The identifier's name.

	/**
	 * @brief Construct a new IDNode object.
	 * 
	 * @param value The identifier's name.
	 */
	IDNode(std::string value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents integer literals.
struct DecimalNode : public LiteralNode
{
	int _value;	// The literal's integer value.

	/**
	 * @brief Construct a new DecimalNode object.
	 * 
	 * @param value The literal's integer value.
	 */
	DecimalNode(int value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents Boolean literals.
struct BoolNode : public LiteralNode
{
	bool _value;	// The literal's Boolean value.

	/**
	 * @brief Construct a new BoolNode object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	BoolNode(bool value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// Represents string literals.
struct StrNode : public LiteralNode
{
	std::string _value;	// The literal's string value.

	/**
	 * @brief Construct a new StrNode object.
	 * 
	 * @param value The literal's Boolean value.
	 */
	StrNode(std::string value);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
