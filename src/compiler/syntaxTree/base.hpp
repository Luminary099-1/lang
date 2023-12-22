#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include <vector>


// Base class for nodes of the AST.
struct SyntaxNode
{
	/**
	 * @brief Traverses the AST and checks that all identifiers are defined in
	 * their scope at most once. Decorates the tree with references to
	 * definitions of types defined elsewhere in the tree.
	 */
	virtual void Scope(/* Something goes here that refers to the scope stack. */) = 0;

	/**
	 * @brief Traverses the AST and checks for semantic and typing correctness.
	 */
	virtual void Validate() = 0;
	
	/**
	 * @brief Prints a textual representation of this AST node to the specified
	 * stream.
	 * 
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree. Need not be set.
	 */
	virtual void
		Print(std::ostream& os, std::string_view indent, int depth = 0);

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

	/**
	 * @brief Construct a new TypeNode object.
	 * 
	 * @param type_name The type's name.
	 */
	TypeNode(std::string type_name);

	/**
	 * @brief Sets the type reference to the applicable node of the syntax tree.
	 */
	void Resolve(/* Something goes here that refers to the scope stack. */);

	void Scope() override;
	void Validate() override;
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
