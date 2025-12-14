// Keep Error.h included before Windows.h to avoid macro conflicts
#include "Error.h"
#include "Parm.h"
#include "In.h"
#include "Log.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Save ERROR macro before including Windows.h
#ifdef ERROR
#pragma push_macro("ERROR")
#undef ERROR
#endif
#include <Windows.h>
// Save and undefine IN macro if it was defined by Windows.h
#ifdef IN
#pragma push_macro("IN")
#undef IN
#endif
// Ensure ERROR macro is undefined after Windows.h
#ifdef ERROR
#undef ERROR
#endif

#pragma warning(disable : 4996)

#define W(x, y)  << std::setw(x) << (y) <<
#define STR(n, line, type, id)\
	"|" W(4,n) " |  " W(5,line) "    |" W(17,type) " |  " W(SCOPED_ID_MAXSIZE, id) " |"


namespace Log
{

	// Convert UTF-8 input string to CP1251 output string (Windows)
	static std::string utf8_to_cp1251(const char* s)
	{
		if (!s || s[0] == '\0') return std::string();
		// UTF-8 -> wide
		int wlen = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
		if (wlen == 0) return std::string(s);
		std::wstring w;
		w.resize(wlen);
		MultiByteToWideChar(CP_UTF8, 0, s, -1, &w[0], wlen);
		// wide -> CP1251
		int len = WideCharToMultiByte(1251, 0, w.c_str(), -1, NULL, 0, NULL, NULL);
		if (len == 0) return std::string(s);
		std::string out;
		out.resize(len);
		WideCharToMultiByte(1251, 0, w.c_str(), -1, &out[0], len, NULL, NULL);
		if (!out.empty()) out.resize(len - 1); // drop terminating null
		return out;
	}

	LOG getstream(wchar_t  logfile[])
	{
		LOG stream;
		stream.stream = new std::ofstream;
		stream.stream->open(logfile);
		if (!stream.stream->is_open())
			throw ERROR_THROW(103); // error opening log file
		wcscpy_s(stream.logfile, logfile);
		// set locale for correct Cyrillic output (Windows CP1251)
		try {
			stream.stream->imbue(std::locale(".1251"));
		}
		catch (...) {
			stream.stream->imbue(std::locale(""));
		}
		return stream;
	}

	void writeLog(LOG& log)
	{
		char buffer[80];
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		const char* format = "%d.%m.%Y %H:%M:%S";
		strftime(buffer, 80, format, timeinfo);
		std::string s = std::string("\n----------- LOG ------------ Time: ") + buffer + " ------------ \n\n";
		*log.stream << utf8_to_cp1251(s.c_str());
	}

	void writeLine(std::ostream* stream, char* c, ...)		// write line composed from C-strings
	{
		va_list args;
		va_start(args, c);
		char* result = (char*)malloc(15);
		int size = 0;
		char* ptr = c;

		while (ptr != nullptr && strcmp(ptr, "") != 0)
		{
			size_t slen = strlen(ptr);
			result = (char*)realloc(result, size + (int)slen + 1);
			result[size] = '\0';
			size += (int)slen;
			strcat_s(result, size + 1, ptr);
			ptr = va_arg(args, char*);
		}
		va_end(args);
		std::string out = utf8_to_cp1251(result);
		*stream << out << std::endl;
		free(result);
	}

	void writeParm(LOG& log, Parm::PARM& parm)
	{
		char inTxt[PARM_MAX_SIZE],
			outTxt[PARM_MAX_SIZE],
			logTxt[PARM_MAX_SIZE];
		wcstombs(inTxt, parm.in, wcslen(parm.in) + 1);
		wcstombs(outTxt, parm.out, wcslen(parm.out) + 1);
		wcstombs(logTxt, parm.log, wcslen(parm.log) + 1);
		std::string s = std::string("\n----- PARAMETERS --------")
			+ std::string("\n-in: ") + inTxt
			+ std::string("\n-out: ") + outTxt
			+ std::string("\n-log: ") + logTxt;
		*log.stream << utf8_to_cp1251(s.c_str());
	}

	void writeIn(std::ostream* stream, In::IN& in)
	{
		std::ostringstream ss;
		ss << "\n---- INPUT INFO ------";
		ss << "\nInput size: " << std::setw(3) << in.size
			<< "\nIgnored: " << std::setw(3) << in.ignor
			<< "\nLines: " << std::setw(3) << in.lines << "\n\n";
		std::string s = ss.str();
		*stream << utf8_to_cp1251(s.c_str());
	}

	void writeError(std::ostream* stream, Error::ERROR error_param)
	{
		Error::ERROR& e = error_param;
		if (stream == NULL)
		{
			if (e.position.col == -1 || e.position.line == -1)
				std::cout << std::endl << "Error N" << e.id << ": " << e.message << std::endl;
			else if (e.position.col == NULL)
				std::cout << std::endl << "Error N" << e.id << ": " << e.message
				<< " line: " << e.position.line << std::endl;
			else
				std::cout << std::endl << "Error N" << e.id << ": " << e.message
				<< " line: " << e.position.line
				<< " col: " << e.position.col << std::endl;
			system("pause");
		}
		else
		{
			std::ostringstream ss;
			if (e.position.col == -1 || e.position.line == -1)
				ss << std::endl << "Error N" << e.id << ": " << e.message;
			else if (e.position.col == NULL)
				ss << std::endl << "Error N" << e.id << ": " << e.message
				<< " line: " << e.position.line;
			else
				ss << std::endl << "Error N" << e.id << ": " << e.message
				<< " line: " << e.position.line
				<< " col: " << e.position.col;
			std::string s = ss.str();
			*stream << utf8_to_cp1251(s.c_str());
			*stream << std::endl;
		}
		throw ERROR_THROW_IN(e.id, e.position.line, e.position.col);
	}

	void writeWords(std::ostream* stream, In::InWord* words)
	{
		std::ostringstream ss;
		ss << " ------------------ WORDS LIST: ------------------" << std::endl;
		for (int i = 0; i < words->size; i++)
			ss << std::setw(2) << i << std::setw(3) << words[i].line << " |  " << words[i].word << std::endl;
		ss << "\n-------------------------------------------------------------------------\n\n";
		std::string s = ss.str();
		*stream << utf8_to_cp1251(s.c_str());
	}

	void Close(LOG& log)
	{
		(*log.stream).close();
	}
}
// Restore ERROR and IN macros if they were defined
#ifdef ERROR
#pragma pop_macro("ERROR")
#endif
#ifdef IN
#pragma pop_macro("IN")
#endif