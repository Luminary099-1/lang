#pragma once

#include "../utilities.hpp"

#include <string>
#include <string_view>
#include <map>
#include <ostream>
#include <vector>


// Base class for nodes of the AST.
struct SyntaxTreeNode
{
	/**
	 * @brief Accumulates all symbols definitions into the specified scope stack
	 * and verifies the existence of symbols for all other occurrences. Performs
	 * a pass of only global definitions first, allowing them to be ordered
	 * arbitrarily.
	 * 
	 * @param ss The scope stack instance to use.
	 * @param first_pass True if only global definitions are to be considered.
	 * False for the general case where all symbols are to be checked.
	 * @return true on success; false if a symbol is redefined in the same scope
	 * or an undefined symbol is referenced.
	 */
	virtual bool Scope(ScopeStack& ss, TUBuffer& src, bool first_pass);

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
	static void PrintMaybe(SyntaxTreeNode* node, std::ostream& os
		, std::string_view indent, int depth);
};


// Represents a type in the program.
struct Type : public SyntaxTreeNode, public TokenInfo
{
	// Enumerates the fundamental types of the language.
	enum class Fundamentals
	{
		Void,
		Int,
		Bool,
		String
	};

	// Maps the names of fundamental types to their enumerations.
	static const std::map<std::string_view, Fundamentals> _namedFundamentals;

	std::string _name;			// The type's name.
	Fundamentals _fund_type;	// The fundamental type, if applicable.
	SyntaxTreeNode*	_type;		// The defined type, if applicable.

	// Default constructor.
	Type();

	/**
	 * @brief Construct a new Type object.
	 * 
	 * @param type_name The type's name.
	 */
	Type(std::string type_name);

	bool Scope(ScopeStack& ss, TUBuffer& src, bool first_pass) override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
