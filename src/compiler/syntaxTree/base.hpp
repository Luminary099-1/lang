#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include <vector>


// Base class for nodes of the AST.
struct SyntaxNode
{
	/**
	 * @brief Prints a textual representation of this AST node to the specified
	 * stream.
	 * 
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree. Need not be set.
	 */
	virtual void
		Print(std::ostream& os, std::string_view indent, int depth = 0) = 0;

	/**
	 * @brief Prints the indentation for Print().
	 * 
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree (the number of levels of
	 * indentation).
	 */
	void PrintIndent(std::ostream& os, std::string_view indent, int depth);
};


// Represents a type in the program.
struct TypeNode : public SyntaxNode
{
	// Enumerates the fundamental types of the language.
	enum class BasicTypes
	{
		Void,
		Int,
		Bool,
		String
	};

	std::string _name;	// The type's name.
	BasicTypes _type;	// Enumeration of the fundamental type, if applicable.


	TypeNode();


	/**
	 * @brief Construct a new TypeNode object.
	 * 
	 * @param type_name The type's name.
	 */
	TypeNode(std::string type_name);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
