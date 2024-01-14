#include "utilities.hpp"


void HighlightError(std::ostream& os, TUBuffer& src, TokenInfo& info)
{
	size_t line_start {info._off - info._col + 1};
	os << '\t';
	for (size_t i {line_start}; src._buf[i] != '\n' && i < src._size; ++ i)
		os << src._buf[i];
	os << '\n' << '\t';
	for (int i {0}; i < info._col - 1; ++ i) os << ' ';
	for (int i {0}; i < info._endOff - info._off; ++ i) os << '^';
	os << '\n';
};


void TokenInfo::SetSymbolInfo(TokenInfo info)
{
	_row		= info._row;
	_endRow		= info._endRow;
	_col		= info._col;
	_endCol		= info._endCol;
	_off		= info._off;
	_endOff		= info._endOff;
}


void TokenInfo::SetMergedInfo(TokenInfo& i1, TokenInfo& i2)
{
	TokenInfo& first {i1};
	TokenInfo& second {i2};
	if (i2._off < i1._off)
	{
		first = i2;
		second = i1;
	}
	_row	= first._row;
	_endRow	= second._endRow;
	_col	= first._col;
	_endCol	= second._endCol;
	_off	= first._off;
	_endOff	= second._endOff;
}
