#include "utilities.hpp"

#include <fstream>


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


TU::TU(size_t size)
	: _size{size}, _buf{new char[size]}
{}


TU::~TU()
{
	delete _buf;
}


std::unique_ptr<TU> TU::LoadFromFile(char* src_path)
{
	std::ifstream src_file;
	src_file.open(src_path, std::ios_base::in);
	if (!src_file)
	{
		std::string msg {"Failed to open source file: "};
		msg.append(src_path);
		throw std::runtime_error(msg);
	}

	src_file.seekg(0, std::ios_base::end);
	size_t size = src_file.tellg();
	src_file.seekg(0, std::ios_base::beg);
	
	std::unique_ptr<TU> tu {std::make_unique<TU>(size)};
	src_file.read(tu->_buf, tu->_size);
	src_file.close();

	return tu;
}


void TU::HighlightError(std::ostream& os, TokenInfo& info)
{
	size_t line_start {info._off - info._col + 1};
	os << '\t';
	for (size_t i {line_start}; _buf[i] != '\n' && i < _size; ++ i)
		os << _buf[i];
	os << '\n' << '\t';
	for (int i {0}; i < info._col - 1; ++ i) os << ' ';
	for (int i {0}; i < info._endOff - info._off; ++ i) os << '^';
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
