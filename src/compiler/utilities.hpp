#pragma once

#include <forward_list>
#include <fstream>
#include <map>
#include <memory>
#include <string_view>
#include <vector>


// Stores the information necessary to locate a token in the TU buffer.
struct TokenInfo
{
	int _row {1};		// Row at the start of the match.
	int _col {1};		// Column at the start of the match.
	size_t _off {0};	// Offset of the first character of the match.
	size_t _endOff {0};	// Offset after the last character of the match.

	/**
	 * Convenience function to copy in the fields defined by this struct
	 * from other instances of TokenInfo.
	 * @param info An instance of TokenInfo whose values are to be copied to
	 * this.
	 */
	void SetSymbolInfo(TokenInfo info);

	/**
	 * Copies the fields defined by this struct from two other instance
	 * such that the interval represented spans the intervals of both inputs.
	 * @param i1 An instance of TokenInfo.
	 * @param i2 An instance of TokenInfo.
	 */
	void SetMergedInfo(TokenInfo& i1, TokenInfo& i2);
};


// Wraps a buffer to contain a TU's source.
class TU
{
	static const size_t _chunk {4096};	// Size of the buffer.
	std::ifstream _src;	// Source file.

public:
	char* _buf;			// Pointer to a TU's source buffer.
	size_t _size;		// Number of bytes stored in the buffer.
	bool _end {false};	// Indicated the input has been exhausted.

	/**
	 * Construct a new TU object for the specified file.
	 * @param src_path Path to the file to be represented by this object.
	 * @throws std::runtime_error If the specified file could not be opened.
	 */
	TU(char* src_path);

	// Deconstructor.
	~TU();

	/**
	 * Advances the buffer's position in the source file to the next available
	 * data.
	 */
	void ReadNext();

	/**
	 * Populates the buffer with the data a the specified offset in the source
	 * file.
	 * @param pos Offset to read from in the source file.
	 */
	void ReadFrom(size_t pos);

	/**
	 * Outputs an error message that contains an erroneous line of code
	 * and highlights which token causes the error.
	 * @param os Stream to write the message to.
	 * @param info Position information of the offending token.
	 */
	void HighlightError(std::ostream& os, TokenInfo& info);
};


/**
 * Stores program symbols in a scoped hierarchy.
 * @tparam I Type of identifiers in the scoped hierarchy.
 * @tparam D Type of the declarations in the scoped hierarchy.
 */
template<typename I, typename D>
struct SymbolTable
{
protected:
	// A stack of scopes that map identifiers to AST definitions.
	std::forward_list<std::map<I, D*>> _stackMap;

public:
	/** Default constructor. */
	SymbolTable()
	{}

	/** Mark the beginning of a new scope. */
	void Enter()
	{
		_stackMap.push_front(std::map<I, D*>());
	}

	/**  Mark the end of a new scope. */
	void Exit()
	{
		_stackMap.front().clear();
		_stackMap.pop_front();
	}

	/**
	 * Attempts to define a new symbol in the current scope.
	 * @param name The symbol's identifier (name).
	 * @param node The AST node referred to by the symbol.
	 * @return AST node that defines the symbol. If no such symbol is defined,
	 * nullptr is returned.
	 */
	D* Define(I name, D* node)
	{
		std::map<I, D*>& front {_stackMap.front()};
		if (front.count(name) == 0) return front[name];
		front.insert(std::pair(name, node));
		return nullptr;
	}

	/**
	 * Returns a pointer to the AST node referred to by the specfied symbol,
	 * if it exists in any scope. The most recently defined instance of that
	 * symbol is returned over others (in preservation of shadowing).
	 * @param name The symbol's identifier (name).
	 * @return AST node that defines the symbol. If no such symbol is defined,
	 * nullptr is returned. A returned pointer indicates a name collision.
	 */
	D* Lookup(I name)
	{
		for (std::map<I, D*> scope : _stackMap)
			if (scope.count(name)) return scope[name];
		return nullptr;
	}
};
