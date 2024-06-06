#include "utilities.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>


void TokenInfo::SetSymbolInfo(TokenInfo info)
{
	_row		= info._row;
	_col		= info._col;
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
	_col	= first._col;
	_off	= first._off;
	_endOff	= second._endOff;
}


TU::TU(char* src_path)
{
	_src.open(src_path, std::ios_base::in);
	if (!_src)
	{
		std::string msg {"Failed to open source file: "};
		msg.append(src_path);
		throw std::runtime_error(msg);
	}

	_buf = new char[_chunk];
	ReadNext();
}


TU::~TU()
{
	_src.close();
	delete _buf;
}


void TU::ReadNext()
{
	_src.read(_buf, _chunk);
	_size = _src.gcount();
	_end = _src.eof();
}


void TU::ReadFrom(size_t pos)
{
	_src.seekg(pos);
	ReadNext();
}


void TU::HighlightError(std::ostream& os, TokenInfo& info)
{
	const size_t line_start {info._off - info._col + 1ull};
	ReadFrom(line_start);
	os << '\t';
	const size_t line_len {std::min(_size, (size_t) 256)};
	for (size_t i {line_start}; _buf[i] != '\n' && i < line_len; ++ i)
		os << _buf[i];
	os << '\n' << '\t';
	const size_t padding {info._col - 1};
	for (int i {0}; i < padding; ++ i) os << ' ';
	const size_t len {info._endOff - info._off};
	for (size_t i {0}; i < std::min(len, 256 - padding - len); ++ i) os << '^';
	os << '\n';
};


template<typename I, typename D>
void SymbolTable<I, D>::Enter()
{
	_stackMap.push_front(std::map<I, D*>());
}


template<typename I, typename D>
void SymbolTable<I, D>::Exit()
{
	_stackMap.front().clear();
	_stackMap.pop_front();
}


template<typename I, typename D>
D* SymbolTable<I, D>::Define(I name, D* node)
{
	std::map<I, D*>& front {_stackMap.front()};
	if (front.count(name) == 0) return front[name];
	front.insert(std::pair(name, node));
	return nullptr;
}


template<typename I, typename D>
D* SymbolTable<I, D>::Lookup(I name)
{
	for (std::map<I, D*> scope : _stackMap)
		if (scope.count(name)) return scope[name];
	return nullptr;
}
