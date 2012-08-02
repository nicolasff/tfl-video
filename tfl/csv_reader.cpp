#include "csv_reader.h"

#include <iostream>
#include <vector>

#include <assert.h>

using namespace std;

CsvReader::CsvReader(string filename) {

	m_file.open(filename);

	string line;
	getline(m_file, line);

	// find keys
	split(line, m_keys);
}

vector<string>
CsvReader::get() {

	string line;
	getline(m_file, line);

	vector<string> ret;
	split(line, ret);

	return ret;
}


int
CsvReader::keyIndex(std::string key) const {
	int offset = 0;
	for (auto header_key: m_keys) {
		if (header_key == key) {
			return offset;
		}
		offset++;
	}
	return -1;
}

bool
CsvReader::eof() const {
	return m_file.eof();
}

size_t
CsvReader::dimensions() const {
	return m_keys.size();
}

void
CsvReader::add(vector<string> &out, const char *start, const char *cur) const{
	size_t sz = cur-start;
	if(*start == '"' && *(cur-1)=='"') {
		start++;
		sz -= 2;
	}
	out.push_back(string(start, sz));
}

void
CsvReader::split(string line, vector<string> &out) const {

	const char *p = line.c_str();
	const char *start = p;

	bool in_quotes = false;
	while(*p) {
		switch(*p) {
			case ',':
				if(!in_quotes) {
					add(out, start, p);
					start = p + 1;
				}
				break;

			case '"':
				in_quotes = !in_quotes;
				break;

			default:
				break;
		}
		p++;
	}

	add(out, start, p);
}
