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
	static const size_t _capacity {4096};	// Size of the buffer.
	std::ifstream _src;						// Source file.
	std::unique_ptr<char[]> _buf;			// TU's source buffer.
	size_t _size;							// Bytes stored in the buffer.
	bool _end {false}; // Indicates the EOF was reached in the last operation.

public:
	/**
	 * Construct a new TU object for the specified file.
	 * @param src_path Path to the file to be represented by this object.
	 * @throws std::runtime_error If the specified file could not be opened.
	 */
	TU(const char* src_path);

	/**
	 * @return Pointer to the buffer.
	 */
	char* GetBuf() const;

	/**
	 * @return Number of bytes currently available in the buffer.
	 */
	size_t GetSize() const;

	/**
	 * @return true if the end of the source was reached in the last operation;
	 * false otherwise.
	 */
	bool IsEnd() const;

	/**
	 * @return 1 if the end of the source was reached in the last operation; 0
	 * otherwise.
	 */
	int IsFinal() const;

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
