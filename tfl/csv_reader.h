#ifndef TFL_CSV_READER_H
#define TFL_CSV_READER_H

#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <stack>

class CsvReader {
public:

	CsvReader(std::string filename);

	std::vector<std::string> get();
	bool eof() const;
	size_t dimensions() const;

private:
	void split(std::string line, std::vector<std::string> &out) const;
	void add(std::vector<std::string> &out, const char *start, const char *cur) const;

private:
	std::ifstream m_file;
	std::vector<std::string> m_keys;

};

#endif // TFL_CSV_READER_H
