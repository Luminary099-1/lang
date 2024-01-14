#pragma once

#include <forward_list>
#include <map>
#include <ostream>
#include <string_view>
#include <vector>


// Stores the information necessary to locate a token in the TU buffer.
struct TokenInfo
{
	int _row {1};		// Row at the start of the match.
	int _endRow {1};	// Row after the end of the match.
	int _col {1};		// Column at the start of the match.
	int _endCol {1};	// Column after the end of the match.
	size_t _off {0};	// Offset of the first character of the match.
	size_t _endOff {0};	// Offset after the last character of the match.

	/**
	 * @brief Convenience function to copy in the fields defined by this struct
	 * from other instances of TokenInfo.
	 * 
	 * @param info An instance of TokenInfo whose values are to be copied to
	 * this.
	 */
	void SetSymbolInfo(TokenInfo info);

	/**
	 * @brief Copies the fields defined by this struct from two other instance
	 * such that the interval represented spans the intervals of both inputs.
	 * 
	 * @param i1 An instance of TokenInfo.
	 * @param i2 An instance of TokenInfo.
	 */
	void SetMergedInfo(TokenInfo& i1, TokenInfo& i2);
};


// Wraps the buffer that contains a TU's source, including its length.
struct TUBuffer
{
	char* _buf;		// A pointer to a TU's source buffer.
	size_t _size;	// The TU's size.
};


/**
 * @brief Prints the line of referenced token to the specified output stream.
 * 
 * @param os The output stream to write the line and highlighting to.
 * @param src The source file buffer containing the line.
 * @param info The token to highlight from the line.
 */
void HighlightError(std::ostream& os, TUBuffer& src, TokenInfo& info);
