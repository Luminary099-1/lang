#pragma once

#include "../utilities.hpp"

#include <sstream>
#include <string>
#include <string_view>
#include <list>
#include <map>
#include <ostream>
#include <queue>
#include <set>
#include <stack>
#include <vector>


// The type of data sizes and offsets.
using BytesT = int64_t;
// Integer type to represent label IDs for the assembly output.
using IDT = uint32_t;
// Integer type to encode register designations.
using RegT = uint8_t;


// Forward declarations.
struct Breakable;
struct Function;

// Stores the data necessary to perform validation.
struct ValidateData
{
	// TU for the source file expressing this AST.
	TU* _src;
	// Sack containing the chain of breakable predecessor AST nodes.
	std::vector<Breakable*> _bs;
	// Function definition currently being validated.
	Function* _curFunc {nullptr};

	/**
	 * Construct a new structure to store validation data.
	 * @param src A translation unit source file buffer.
	 */
	ValidateData(TU* src);
};


// Forward declarations.
struct Type;
struct Invocation;
struct Function;


// Represents the storage location of variables or other data.
struct Location
{
	// TODO: Address this nonsense by extracting subclasses?
	friend struct Type;
	friend struct Invocation;
	friend struct Function;

	// Indicated the type of storage location represented.
	enum class Place
	{
		Global,		// Global variable.
		Register,	// Register-backed variable.
		Local		// Local relative to the current sub-frame.
	};

protected:
	Type* _type {nullptr};	// Type of the data stored in the location.
	Place _place;			// The type of location represented.

	union
	{
		IDT _label;		// Assembly label ID of this location's definition.
		RegT _reg;		// ID of the register represented by this location.
		BytesT _offset;	// FP-relative offset represented by this location.
	}
	_val; // Stores information necessary for each variant.

	/**
	 * Construct a new location.
	 * @param place Storage type designated by this Location.
	 * @param type Type of data stored in this Location.
	 */
	Location(Place place, Type* type);

public:
	// Default constructor.
	Location();

	/**
	 * Factory function to create a new Location object representing a global
	 * variable.
	 * @param type The stored data's type.
	 * @param label ID of the storage definitions of this location.
	 * @return A new instance of Location.
	 */
	static Location CreateGlobal(Type* type, IDT label);

	/**
	 * Factory function to create a new Location object representing a register-
	 * backed variable.
	 * @param type Stored data's type.
	 * @param reg Designation of the register represented by this location.
	 * @return A new instance of Location.
	 */
	static Location CreateRegister(Type* type, RegT reg);

	/**
	 * Factory function to create a new Location object representing a memory-
	 * backed variable relative to FP.
	 * @param type The stored data's type.
	 * @param offset FP-relative offset represented by this location.
	 * @return A new instance of Location.
	 */
	static Location CreateLocal(Type* type, BytesT offset);

	/**
	 * @return Returns the storage type designated by this Location.
	 */
	Place GetPlace();

	/**
	 * Converts this stack argument Location for access as a callee to a the
	 * corresponding location for access as a caller. Has no effect if this
	 * location is not a local.
	 */
	void ReinterpretStack(BytesT stack_args_size);
};


// Forward declaration:
struct Declaration;


// Stores program symbols in a scoped hierarchy.
struct SymbolTable
{
protected:
	// A stack of scopes that map identifiers to AST declarations.
	std::forward_list<std::map<std::string_view, Declaration*>> _stackMap;

public:
	/** Default constructor. */
	SymbolTable();

	/** Mark the beginning of a new scope. */
	void Enter();

	/**  Mark the end of a new scope. */
	void Exit();

	/**
	 * Attempts to define a new symbol in the current scope.
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return AST node that defines the symbol. If no such symbol is defined,
	 * nullptr is returned.
	 */
	Declaration* Define(std::string_view name, Declaration* node);

	/**
	 * Returns a pointer to the AST node referred to by the specfied symbol,
	 * if it exists in any scope. The most recently defined instance of that
	 * symbol is returned over others (in preservation of shadowing).
	 * @param name The symbol's identifier (name).
	 * @return AST node that defines the symbol. If no such symbol is defined,
	 * nullptr is returned. A returned pointer indicates a name collision.
	 */
	Declaration* Lookup(std::string_view name);
};


// Storess the data necessary to generate code and provides some utilities.
struct GenData
{};


// Base class for nodes of the AST.
struct SyntaxTreeNode
	: public virtual TokenInfo
{
	// Necessary to enable polymorphic deletion of derived AST nodes.
	virtual ~SyntaxTreeNode();

	/**
	 * Performs scope resolution by populating references to declarations.
	 * @param symbols Symbol table to be used.
	 * @param tu Translation unit corresponding to this node.
	 * @return true if scope resolution is successful; false otherwise.
	 */
	virtual bool Scope(SymbolTable& symbols, TU& tu);

	/**
	 * Traverses the AST structure and validates the semantics of the
	 * represented program.
	 * @param dat An instance of ValidateData to store the state of the
	 * validation.
	 * @return true if the validation discovered no errors; false otherwise.
	 */
	virtual bool Validate(ValidateData& dat);

	/**
	 * Generates the output assembly described by the AST.
	 * @param dat An instance of GenData to store the state of the generation.
	 * @param os The output stream to write the program output to.
	 */
	virtual void Generate(GenData& dat, std::ostream& os);

	/**
	 * Prints a textual representation of this AST node to the specified stream.
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree. Need not be set.
	 */
	virtual void
		Print(std::ostream& os, std::string_view indent, int depth = 0) = 0;

	/**
	 * Prints the indentation for Print().
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree (the number of levels of
	 * indentation).
	 */
	static void
		PrintIndent(std::ostream& os, std::string_view indent, int depth);

	/**
	 * Calls Print() on the referenced node. If the node is nullptr,
	 * "nullptr\n" is printed instead.
	 * @param node A node to attempt to print.
	 * @param os The output stream to print to.
	 * @param indent A view of the unit of indentation to be used.
	 * @param depth The depth of the node in the tree (the number of levels of
	 * indentation).
	 */
	static void PrintMaybe(SyntaxTreeNode* node, std::ostream& os
		, std::string_view indent, int depth);
};


// Stores the AST of a complete program.
using AST = std::vector<std::unique_ptr<SyntaxTreeNode>>;


struct Identifier
	: public SyntaxTreeNode
{
	std::string _id; // Name associated with this node.

	/**
	 * Construct a new identifier.
	 * @param id Name to be associated with this node.
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
	std::unique_ptr<Identifier> _name; // Declared identifier.

	/**
	 * Construct a new declaration.
	 * @param name Identifier associated with the declaration.
	 */
	Declaration(Identifier* name);
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
	// Set of fundamental types.
	static const std::set<const Type*> _fundamentals;
	// Type's instance size in bytes.
	BytesT _size {0};

	// Default constructor.
	Type();

	/**
	 * Construct a new type.
	 * @param type_name Type's symbolic name.
	 * @param size Type's instance size in bytes.
	 */
	Type(std::string type_name, BytesT size = 0);


public:
	// TODO: Reevaluate whether the name should be stored as an Identifier node.
	std::string _name;					// The type's name.
	SyntaxTreeNode*	_defType {nullptr};	// The defined type, if applicable.

	/**
	 * Returns a pointer to a new instance of Type with. AST nodes which
	 * define the type must take ownership of the pointer.
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

	/**
	 * @return true if this is an integral type; false otherwise.
	 */
	const bool IsIntegral() const;

	/**
	 * @return true if this is a pointer type; false otherwise.
	 */
	const bool IsPointer() const;

	/**
	 * @return The type's instance size in bytes.
	 */
	const BytesT GetSize() const;

	// Equality operator overload.
	friend bool operator==(const Type& lhs, const Type& rhs);

	// Inequality operator overload.
	friend bool operator!=(const Type& lhs, const Type& rhs);

	/**
	 * Generates the output assembly necessary to load or store data to
	 * the specified location. The source/destination register for the operation
	 * will be the first available register in dat.
	 * @param dat Instance of GenData with the appropriate context for this
	 * opperation.
	 * @param loc The data's location.
	 * @param load true if the access is to be a load operation; false for a
	 * store operation.
	 * @param os Output stream to write the program output to.
	 */
	void
	GenerateAccess(GenData& dat, Location loc, bool load, std::ostream& os);

	// Prints inline in the format "Type = <type_name>".
	void Print(std::ostream& os, std::string_view indent, int depth) override;
};
