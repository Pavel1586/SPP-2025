#pragma once
#include "Parm.h"
#include "Error.h"
#include "In.h"
#include <iomanip>
#include <iostream>

// Protect against Windows ERROR and IN macros if they're defined
#ifdef ERROR
#pragma push_macro("ERROR")
#undef ERROR
#define ERROR_WAS_DEFINED
#endif
#ifdef IN
#pragma push_macro("IN")
#undef IN
#define IN_WAS_DEFINED
#endif

namespace Log
{
	struct LOG
	{
		wchar_t logfile[PARM_MAX_SIZE];
		std::ofstream* stream;
		LOG() : stream(nullptr)
		{
			wcscpy_s(logfile, L"");
		}
	};

	LOG getstream(wchar_t logfile[]);
	void writeLog(LOG& log);									
	void writeLine(std::ostream* stream, char* c, ...);			  
	void writeParm(LOG& log, Parm::PARM& parm);					
	void writeIn(std::ostream* stream, In::IN& in);				
	void writeWords(std::ostream* stream, In::InWord* words);	  
	void writeError(std::ostream* stream, Error::ERROR error);	  
	void Close(LOG& log);										
}

// Restore ERROR and IN macros if they were defined
#ifdef ERROR_WAS_DEFINED
#pragma pop_macro("ERROR")
#undef ERROR_WAS_DEFINED
#endif
#ifdef IN_WAS_DEFINED
#pragma pop_macro("IN")
#undef IN_WAS_DEFINED
#endif
