#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include <vector>


// Base class for nodes of the AST.
struct SyntaxTreeNode
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
	static void
		PrintIndent(std::ostream& os, std::string_view indent, int depth);

	/**
	 * @brief Calls Print() on the referenced node. If the node is nullptr,
	 * "nullptr\n" is printed instead.
	 * 
	 * @param node A node to attempt to print.
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree (the number of levels of
	 * indentation).
	 */
	static void PrintMaybe(
		SyntaxTreeNode* node, std::ostream& os, std::string_view indent, int depth);
};


// Represents a type in the program.
struct Type : public SyntaxTreeNode
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


	Type();


	/**
	 * @brief Construct a new Type object.
	 * 
	 * @param type_name The type's name.
	 */
	Type(std::string type_name);

	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
