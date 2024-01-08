#pragma once

#include "../utilities.hpp"

#include <string>
#include <string_view>
#include <map>
#include <ostream>
#include <set>
#include <vector>


// Base class for nodes of the AST.
struct SyntaxTreeNode
{
	/**
	 * @brief Accumulates all symbols definitions into the specified scope stack
	 * and verifies the existence of symbols for all other occurrences.
	 * 
	 * @param ss The scope stack instance to use.
	 * @param src A reference to the TUBuffer that contains the source file
	 * containing this AST node.
	 * @return true on success; false if a symbol is redefined in the same scope
	 * or an undefined symbol is referenced.
	 */
	virtual bool Scope(ScopeStack& ss, TUBuffer& src);

	/**
	 * @brief 
	 * 
	 * @return true 
	 */
	virtual bool Validate(TUBuffer& src);

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
struct Type
	: public SyntaxTreeNode, public TokenInfo
{
protected:
	static const std::unique_ptr<Type> _void;	// Void singleton.
	static const std::unique_ptr<Type> _int;	// Integer singleton.
	static const std::unique_ptr<Type> _bool;	// Boolean singleton.
	static const std::unique_ptr<Type> _string;	// String singleton.

	// Maps the names of fundamental types to their singletons.
	static const std::map<std::string_view, Type*> _namedFundamentals;
	// The set of fundamental types.
	static const std::set<const Type*> _fundamentals;

	// Default constructor.
	Type();

	/**
	 * @brief Construct a new Type object.
	 * 
	 * @param type_name The type's symbolic name.
	 */
	Type(std::string& type_name);

public:
	std::string _name;					// The type's name.
	SyntaxTreeNode*	_defType {nullptr};	// The defined type, if applicable.

	/**
	 * @brief Returns a pointer to a new instance of Type with. AST nodes which
	 * define the type must take ownership of the pointer.
	 * 
	 * @param type_name The type to be represented.
	 * @return Type* A pointer to the new Type instance.
	 */
	static Type* Create(std::string& type_name);
	
	/**
	 * @return true if this is the fundamental type void; false otherwise.
	 */
	bool IsVoid();

	/**
	 * @return true if this is the fundamental type int; false otherwise.
	 */
	bool IsInt();
	
	/**
	 * @return true if this is the fundamental type bool; false otherwise.
	 */
	bool IsBool();

	/**
	 * @return true if this is the fundamental type string; false otherwise.
	 */
	bool IsString();

	// Equality operator overload.
	friend bool operator==(const Type& lhs, const Type& rhs);

	// Inequality operator overload.
	friend bool operator!=(const Type& lhs, const Type& rhs);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	// Prints inline in the format "Type = <type_name>".
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
