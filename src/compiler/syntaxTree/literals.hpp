#pragma once

#include "statements.hpp"

#include <string>


// Base class to represent literals.
struct LiteralNode : public ExprNode {};


// 
struct IDNode : public LiteralNode
{
	std::string _value;	// 

	/**
	 * @brief Construct a new IDNode object
	 * 
	 * @param value 
	 */
	IDNode(std::string value);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


// 
struct DecimalNode : public LiteralNode
{
	int _value;	// 

	/**
	 * @brief Construct a new Decimal Node object
	 * 
	 * @param value 
	 */
	DecimalNode(int value);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


//
struct BoolNode : public LiteralNode
{
	bool _value;	// 

	/**
	 * @brief Construct a new Bool Node object
	 * 
	 * @param value 
	 */
	BoolNode(bool value);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};


//
struct StrNode : public LiteralNode
{
	std::string _value;	// 

	/**
	 * @brief Construct a new Str Node object
	 * 
	 * @param value 
	 */
	StrNode(std::string value);

	void Validate() override;
	void Scope() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
