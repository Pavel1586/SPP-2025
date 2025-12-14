#pragma once
#include <iostream>

#define MAXSIZE_ID	16
#define SCOPED_ID_MAXSIZE   MAXSIZE_ID*2
#define MAXSIZE_TI		4096
#define INT_DEFAULT	0x00000000
#define STR_DEFAULT	0x00
#define NULLIDX_TI		0xffffffff
#define STR_MAXSIZE	255
#define CHAR_MAXSIZE 1
#define INT_MAXSIZE   2147483647
#define INT_MINSIZE   -2147483647
#define MAX_PARAMS_COUNT 3
#define CONCAT_PARAMS_CNT 2
#define POW_PARAMS_CNT 2
#define RANDOM_PARAMS_CNT 2
#define LENGHT_PARAMS_CNT 1
#define ATOII_PARAMS_CNT 1
#define COMPARE_PARAMS_CNT 2
#define COPY_PARAMS_CNT 1
#define FACTORIAL_PARAMS_CNT 1
#define CONCAT_TYPE IT::IDDATATYPE::STR
#define COMPARE_TYPE IT::IDDATATYPE::INT
#define LENGHT_TYPE IT::IDDATATYPE::INT
#define POW_TYPE IT::IDDATATYPE::INT
#define RANDOM_TYPE IT::IDDATATYPE::INT
#define ATOII_TYPE IT::IDDATATYPE::INT
#define COPY_TYPE IT::IDDATATYPE::STR
#define FACTORIAL_TYPE IT::IDDATATYPE::INT


namespace IT
{
	enum IDDATATYPE { INT = 1, STR = 2, PROC = 3, CHAR = 4, UNDEF };
	enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 };
	enum STDFNC { F_POW, F_FACTORIAL, F_COMPARE, F_RANDOM, F_CONCAT, F_LENGHT, F_ATOII, F_COPY, F_NOT_STD };
	static const IDDATATYPE CONCAT_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };
	static const IDDATATYPE LENGHT_PARAMS[] = { IT::IDDATATYPE::STR };
	static const IDDATATYPE ATOII_PARAMS[] = { IT::IDDATATYPE::STR };
	static const IDDATATYPE POW_PARAMS[] = { IT::IDDATATYPE::INT, IT::IDDATATYPE::INT };
	static const IDDATATYPE RANDOM_PARAMS[] = { IT::IDDATATYPE::INT, IT::IDDATATYPE::INT };
	static const IDDATATYPE COMPARE_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };
	static const IDDATATYPE COPY_PARAMS[] = { IT::IDDATATYPE::STR};
	static const IDDATATYPE FACTORIAL_PARAMS[] = { IT::IDDATATYPE::INT };
	struct Entry
	{
		union
		{
			int	vint;            		
			struct
			{
				int len;				
				char str[STR_MAXSIZE - 1];
			} vstr;					
			struct
			{
				int count;				
				IDDATATYPE* types;			
			} params;
		} value;					
		int			idxfirstLE;						
		char		id[SCOPED_ID_MAXSIZE];	
		IDDATATYPE	iddatatype;				
		IDTYPE		idtype;					

		Entry()						
		{
			this->value.vint = INT_DEFAULT;
			this->value.vstr.len = NULL;
			this->value.params.count = NULL;
		};
		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype) 
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
		};
	};
	struct IdTable		
	{
		int maxsize;
		int size;	
		Entry* table;	
	};
	IdTable Create(int size = NULL);	
	void Add(			
		IdTable& idtable,		
		Entry entry);			 
	int isId(					
		IdTable& idtable,		
		char id[SCOPED_ID_MAXSIZE]);	
	bool SetValue(IT::Entry* entry, char* value);
	bool SetValue(IT::IdTable& idtable, int index, char* value);
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable);
};
