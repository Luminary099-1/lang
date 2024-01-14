#pragma once

#include "../utilities.hpp"

#include <string>
#include <string_view>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <vector>


// Forward declarations.
struct Breakable;
struct Declaration;
struct Function;


// A data structure to store program symbols in a scope hierarchy.
struct ScopeStack
{
protected:
	// A stack of scopes that map identifiers to AST definitions.
	std::forward_list<std::map<std::string_view, Declaration*>> _stackMap;

public:
	/**
	 * @brief Begin a new scope in the scope stack.
	 */
	void Enter();

	/**
	 * @brief Exit the current scope in the scope stack.
	 */
	void Exit();

	/**
	 * @brief Attempts to define a new symbol in the scope stack in the current
	 * scope.
	 * 
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return The AST node that defines the symbol. If no such symbol is
	 * defined, nullptr is returned.
	 */
	Declaration* Define(std::string_view name, Declaration* node);

	/**
	 * @brief Returns a pointer to the AST node referred to by the specfied
	 * symbol, if it exists in any scope. The most recently defined instance of
	 * the symbol will be returned.
	 * 
	 * @param name The symbol's identifier (name).
	 * @return The AST node that defines the symbol. If no such symbol is
	 * defined, nullptr is returned.
	 */
	Declaration* Lookup(std::string_view name);
};


struct ValidateData
{
	// The TUBuffer for the source file expressing this AST.
	TUBuffer& _src;
	// Tree stack containing the chain of breakable predecessor AST nodes.
	std::vector<Breakable*>& _bs;
	// The current function definition.
	Function* _curFunc;
};


// Base class for nodes of the AST.
struct SyntaxTreeNode
	: public virtual TokenInfo
{
	/**
	 * @brief Accumulates all symbols definitions into the specified scope stack
	 * and verifies the existence of symbols for all other occurrences.
	 * 
	 * @param ss The scope stack instance to use.
	 * @param src The TUBuffer that contains the source file containing this AST
	 * node.
	 * @return true on success; false if a symbol is redefined in the same scope
	 * or an undefined symbol is referenced.
	 */
	virtual bool Scope(ScopeStack& ss, TUBuffer& src);

	/**
	 * @brief Traverses the AST structure and validates the semantics of the
	 * represented program.
	 * 
	 * @param dat An instance of ValidateData to store the state of the
	 * validation.
	 * @return true if the validation discovered no errors; false otherwise.
	 */
	virtual bool Validate(ValidateData& dat);

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


struct Identifier
	: public SyntaxTreeNode
{
	std::string _id;	// The name associated with this node.

	/**
	 * @brief Construct a new Identifier object.
	 * 
	 * @param id The name to be associated with this node.
	 */
	Identifier(std::string& id);

	// Prints inline in the format "ID = <type_name>".
	void Print(std::ostream& os, std::string_view indent, int depth) override;

	// TokenInfo refers to the ID itself, assigned by the parser.
};


// Represents any node which defines a symbol.
struct Declaration
	: public virtual SyntaxTreeNode
{
	std::unique_ptr<Identifier> _name;	// The declared identifier.

	/**
	 * @brief Construct a new Declaration object.
	 * 
	 * @param name The identifier associated with the declaration.
	 */
	Declaration(Identifier* name);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
};


// Represents a type in the program.
struct Type
	: public SyntaxTreeNode
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
	static Type* Create(std::string type_name);
	
	/**
	 * @return true if this is the fundamental type void; false otherwise.
	 */
	const bool IsVoid() const;

	/**
	 * @return true if this is the fundamental type int; false otherwise.
	 */
	const bool IsInt() const;
	
	/**
	 * @return true if this is the fundamental type bool; false otherwise.
	 */
	const bool IsBool() const;

	/**
	 * @return true if this is the fundamental type string; false otherwise.
	 */
	const bool IsString() const;

	// Equality operator overload.
	friend bool operator==(const Type& lhs, const Type& rhs);

	// Inequality operator overload.
	friend bool operator!=(const Type& lhs, const Type& rhs);

	bool Scope(ScopeStack& ss, TUBuffer& src) override;
	// Prints inline in the format "Type = <type_name>".
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
