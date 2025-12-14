#include "LexAnalysis.h"
#include "Graphs.h"
#include "Error.h"
#include "Log.h"
#include <stack>
#include <string>

#pragma warning(disable : 4996)

int DecimicalNotation(std::string input, int scaleofnot) {
	try {
		bool isNegative = !input.empty() && input[0] == '-';
		size_t startIdx = input.size() > 2 && (input[1] == 'x' || input[1] == 'X') ? 2 : 0;
		int value = std::stoi(input.substr(isNegative ? 1 + startIdx : startIdx), nullptr, scaleofnot);
		return isNegative ? -value : value;
	}
	catch (const std::out_of_range&) {
		return input[0] == '-' ? INT_MINSIZE : INT_MAXSIZE;
	}
}

namespace Lexer
{
	Graph graphs[N_GRAPHS] =
	{
		{ LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS) },
		{ LEX_LITERAL, FST::FST(GRAPH_INT_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_CHAR_LITERAL) },
		{ LEX_VAR, FST::FST(GRAPH_VAR) },
		{ LEX_MAIN, FST::FST(GRAPH_MAIN) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_INTEGER) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_STRING) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_CHAR) },
		{ LEX_FUNCTION, FST::FST(GRAPH_FUNCTION) },
		{ LEX_VOID, FST::FST(GRAPH_VOID) },
		{ LEX_RETURN, FST::FST(GRAPH_RETURN) },
		{ LEX_WRITE, FST::FST(GRAPH_OUTPUT) },
		{ LEX_CYCLE, FST::FST(GRAPH_CYCLE) },
		{ LEX_SWITCH, FST::FST(GRAPH_SWITCH) },
		{ LEX_CASE, FST::FST(GRAPH_CASE) },
		{ LEX_DEFAULT, FST::FST(GRAPH_DEFAULT) },
		{ LEX_BREAK, FST::FST(GRAPH_BREAK) },
		{ LEX_ID, FST::FST(GRAPH_ID) },
		{ LEX_LITERAL_HEX, FST::FST(GRAPH_HEX_LITERAL) },
		{ LEX_ID, FST::FST(GRAPH_FACTORIAL) },
		{ LEX_LITERAL_OCT, FST::FST(GRAPH_OCT_LITERAL) }
	};

	char* getScopeName(IT::IdTable idtable, char* prevword)
	{
		char* a = new char[5];
		a[0] = 'm';
		a[1] = 'a';
		a[2] = 'i';
		a[3] = 'n';
		a[4] = '\0';
		if (strcmp(prevword, MAIN) == 0)
			return a;
		for (int i = idtable.size - 1; i >= 0; i--)
			if (idtable.table[i].idtype == IT::IDTYPE::F)
				return idtable.table[i].id;
		return nullptr;
	}

	int getLiteralIndex(IT::IdTable ittable, char* value, IT::IDDATATYPE type, Log::LOG log, int line)
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::INT: {
					if (ittable.table[i].value.vint == atoi(value))
						return i;
					break;
				}
				case IT::IDDATATYPE::STR:
					char buf[STR_MAXSIZE];
					for (unsigned j = 1; j < strlen(value) - 1; j++)
						buf[j - 1] = value[j];
					buf[strlen(value) - 2] = IN_CODE_NULL;
					if (strcmp(ittable.table[i].value.vstr.str, buf) == 0)
						return i;
					break;
				case IT::IDDATATYPE::CHAR:
					char buk[STR_MAXSIZE];
					for (unsigned j = 1; j < strlen(value) - 1; j++)
						buk[j - 1] = value[j];
					buk[strlen(value) - 2] = IN_CODE_NULL;
					if (strcmp(ittable.table[i].value.vstr.str, buk) == 0)
						return i;
					break;
				}
			}
		}
		return NULLIDX_TI;
	}

	IT::IDDATATYPE getType(char* curword, char* idtype)
	{
		if (!strcmp(TYPE_VOID, idtype))
			return IT::IDDATATYPE::PROC;
		if (!strcmp(TYPE_STRING, idtype))
			return IT::IDDATATYPE::STR;
		if (!strcmp(TYPE_CHAR, idtype))
			return IT::IDDATATYPE::CHAR;
		if (!strcmp(TYPE_DIGIT, idtype))
			return IT::IDDATATYPE::INT;
		if (isdigit(*curword) || *curword == LEX_MINUS)
			return IT::IDDATATYPE::INT;
		else if (*curword == IN_CODE_QUOTE)
			return IT::IDDATATYPE::STR;
		else if (*curword == IN_CODE_QUOTE2)
			return IT::IDDATATYPE::CHAR;

		return IT::IDDATATYPE::UNDEF;
	}

	int getIndexInLT(LT::LexTable& lextable, int itTableIndex)
	{
		if (itTableIndex == NULLIDX_TI)
			return lextable.size;
		for (int i = 0; i < lextable.size; i++)
			if (itTableIndex == lextable.table[i].idxTI)
				return i;
		return NULLIDX_TI;
	}

	bool isLiteral(char* id)
	{
		if (isdigit(*id) || *id == IN_CODE_QUOTE || *id == IN_CODE_QUOTE2 || *id == LEX_MINUS || *id == '0')
			return true;
		return false;
	}

	IT::STDFNC getStandFunction(char* id)
	{
		if (!strcmp(POW, id))
			return IT::STDFNC::F_POW;
		if (!strcmp(FACTORIAL, id))
			return IT::STDFNC::F_FACTORIAL;
		if (!strcmp(COMPARE, id))
			return IT::STDFNC::F_COMPARE;
		if (!strcmp(RANDOM, id))
			return IT::STDFNC::F_RANDOM;
		if (!strcmp(CONCAT, id))
			return IT::STDFNC::F_CONCAT;
		if (!strcmp(LENGHT, id))
			return IT::STDFNC::F_LENGHT;
		if (!strcmp(ATOII, id))
			return IT::STDFNC::F_ATOII;
		if (!strcmp(COPY, id))
			return IT::STDFNC::F_COPY;
		return IT::STDFNC::F_NOT_STD;
	}

	char* getNextLiteralName()
	{
		static int literaldigit = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE], lich[3];
		strcpy_s(buf, MAXSIZE_ID, "LTRL");
		_itoa_s(literaldigit++, lich, 10);
		strcat(buf, lich);
		return buf;
	}

	IT::Entry* getEntry(
		Lexer::LEX& tables,
		char lex,
		char* id,
		char* idtype,
		bool isParam,
		bool isFunc,
		Log::LOG log,
		int line,
		bool& lex_ok)
	{
		IT::IDDATATYPE type = getType(id, idtype);
		int index = IT::isId(tables.idtable, id);
		if (lex == LEX_LITERAL)
			index = getLiteralIndex(tables.idtable, id, type, log, line);
		if (index != NULLIDX_TI)
			return nullptr;

		IT::Entry* itentry = new IT::Entry;
		itentry->iddatatype = type;

		itentry->idxfirstLE = getIndexInLT(tables.lextable, index);

		if (lex == LEX_LITERAL)
		{
			bool int_ok = IT::SetValue(itentry, id);
			if (!int_ok)
			{
				Log::writeError(log.stream, Error::GetError(313, line, 0));
				lex_ok = false;
			}
			if (itentry->iddatatype == IT::IDDATATYPE::STR && itentry->value.vstr.len == 0)
			{
				Log::writeError(log.stream, Error::GetError(310, line, 0));
				lex_ok = false;
			}
			if (itentry->iddatatype == IT::IDDATATYPE::CHAR && itentry->value.vstr.len > CHAR_MAXSIZE)
			{
				Log::writeError(log.stream, Error::GetError(319, line, 0));
				lex_ok = false;
			}
			if (itentry->iddatatype == IT::IDDATATYPE::CHAR && itentry->value.vstr.len == 0)
			{
				Log::writeError(log.stream, Error::GetError(310, line, 0));
				lex_ok = false;
			}
			strcpy_s(itentry->id, getNextLiteralName());
			itentry->idtype = IT::IDTYPE::L;
		}
		else
		{
			switch (type)
			{
			case IT::IDDATATYPE::STR:
				strcpy_s(itentry->value.vstr.str, "");
				itentry->value.vstr.len = STR_DEFAULT;
				break;
			case IT::IDDATATYPE::CHAR:
				strcpy_s(itentry->value.vstr.str, "");
				itentry->value.vstr.len = STR_DEFAULT;
				break;
			case IT::IDDATATYPE::INT:
				itentry->value.vint = INT_DEFAULT;
				break;
			}

			if (isFunc)
			{
				switch (getStandFunction(id))
				{
				case IT::STDFNC::F_POW:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = POW_TYPE;
					itentry->value.params.count = POW_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[POW_PARAMS_CNT];
					for (int k = 0; k < POW_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::POW_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_FACTORIAL:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = FACTORIAL_TYPE;
					itentry->value.params.count = FACTORIAL_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[FACTORIAL_PARAMS_CNT];
					for (int k = 0; k < FACTORIAL_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::FACTORIAL_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_RANDOM:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = RANDOM_TYPE;
					itentry->value.params.count = RANDOM_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[RANDOM_PARAMS_CNT];
					for (int k = 0; k < RANDOM_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::RANDOM_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_CONCAT:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = CONCAT_TYPE;
					itentry->value.params.count = CONCAT_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[CONCAT_PARAMS_CNT];
					for (int k = 0; k < CONCAT_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::CONCAT_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_COMPARE:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = COMPARE_TYPE;
					itentry->value.params.count = COMPARE_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[COMPARE_PARAMS_CNT];
					for (int k = 0; k < COMPARE_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::COMPARE_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_LENGHT:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = LENGHT_TYPE;
					itentry->value.params.count = LENGHT_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[LENGHT_PARAMS_CNT];
					for (int k = 0; k < LENGHT_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::LENGHT_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_ATOII:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = ATOII_TYPE;
					itentry->value.params.count = ATOII_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[ATOII_PARAMS_CNT];
					for (int k = 0; k < ATOII_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::ATOII_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_COPY:
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = COPY_TYPE;
					itentry->value.params.count = COPY_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[COPY_PARAMS_CNT];
					for (int k = 0; k < COPY_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::COPY_PARAMS[k];
					break;
				case IT::STDFNC::F_NOT_STD:
					itentry->idtype = IT::IDTYPE::F;
					break;
				}
			}
			else if (isParam)
				itentry->idtype = IT::IDTYPE::P;
			else
				itentry->idtype = IT::IDTYPE::V;

			strncpy_s(itentry->id, id, SCOPED_ID_MAXSIZE);
		}

		int i = tables.lextable.size;

		if (itentry->idtype == IT::IDTYPE::V)
		{
			// Проверяем, что перед переменной есть var или switch
			bool hasVar = (i >= 2 && tables.lextable.table[i - 2].lexema == LEX_VAR);
			bool hasSwitch = (i >= 1 && tables.lextable.table[i - 1].lexema == LEX_SWITCH);
			
			if (!hasVar && !hasSwitch)
			{
			Log::writeError(log.stream, Error::GetError(304, line, 0));
				lex_ok = false;
			}
		}
		if (i > 1 && itentry->idtype == IT::IDTYPE::F && tables.lextable.table[i - 1].lexema != LEX_FUNCTION)
		{
			Log::writeError(log.stream, Error::GetError(303, line, 0));
			lex_ok = false;
		}
		if (itentry->iddatatype == IT::IDDATATYPE::UNDEF)
		{
			Log::writeError(log.stream, Error::GetError(300, line, 0));
			lex_ok = false;
		}
		return itentry;
	}

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		static bool lex_ok = true;
		tables.lextable = LT::Create(MAXSIZE_LT);
		tables.idtable = IT::Create(MAXSIZE_TI);

		bool isParam = false, isFunc = false;
		int enterPoint = NULL;
		char curword[STR_MAXSIZE], nextword[STR_MAXSIZE];
		int curline;
		std::stack <char*> scopes;

		for (int i = 0; i < In::InWord::size; i++)
		{
			strcpy_s(curword, in.words[i].word);
			if (i < In::InWord::size - 1)
				strcpy_s(nextword, in.words[i + 1].word);
			curline = in.words[i].line;
			isFunc = false;
			int idxTI = NULLIDX_TI;

			// Обработка двухсимвольных операторов: <=, >=, !=
			char lexema = 0;
			if (strlen(curword) == 2)
			{
				if (strcmp(curword, "<=") == 0)
				{
					lexema = LEX_LESSEQUALS;
				}
				else if (strcmp(curword, ">=") == 0)
				{
					lexema = LEX_MOREEQUALS;
				}
				else if (strcmp(curword, "!=") == 0)
				{
					lexema = LEX_NOTEQUALS;
				}
			}

			// Если двухсимвольный оператор распознан, обработать его
			if (lexema != 0)
			{
				LT::Entry* ltentry = new LT::Entry(lexema, curline, NULLIDX_TI);
				LT::Add(tables.lextable, *ltentry);
				continue; // Перейти к следующему слову
			}

			for (int j = 0; j < N_GRAPHS; j++)
			{
				FST::FST fst(curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					char lexema = graphs[j].lexema;
					switch (lexema)
					{
					case LEX_MAIN:
						enterPoint++;
						break;
					case LEX_SEPARATORS:
					{
						switch (*curword)
						{
						case LEX_LEFTHESIS:
						{
							isParam = true;
							if (*nextword == LEX_RIGHTTHESIS || ISTYPE(nextword))
							{
								char* functionname = new char[MAXSIZE_ID];
								char* scopename = getScopeName(tables.idtable, in.words[i - 1].word);
								if (scopename == nullptr)
									break;
								strcpy_s(functionname, MAXSIZE_ID, scopename);
								scopes.push(functionname);
							}
							break;
						}
						case LEX_RIGHTTHESIS:
						{
							isParam = false;
							if (*in.words[i - 1].word == LEX_LEFTHESIS || (i > 2 && (tables.lextable.table[tables.lextable.size - 2].lexema == LEX_ID_TYPE)))
								scopes.pop();
							break;

						}
						case LEX_LEFTBRACE:
						{
							if (i > 0 && *in.words[i - 1].word == LEX_CYCLE)
								break;
							char* functionname = new char[MAXSIZE_ID];
							char* scopename = getScopeName(tables.idtable, in.words[i - 1].word);
							if (scopename == nullptr)  break;
							strcpy_s(functionname, MAXSIZE_ID, scopename);
							scopes.push(functionname);
							break;
						}
						case LEX_BRACELET:
						{
							if (*in.words[i + 1].word == LEX_ID_TYPE || *in.words[i + 1].word == LEX_VOID || *in.words[i + 1].word == LEX_MAIN)
							{
								if (!scopes.empty())
									scopes.pop();
							}
							break;
						}
						case LEX_MORE:		// >
						case LEX_LESS:		// <
						case LEX_EQUALS:	// =
						case LEX_NOTEQUALS:	// N (не !=)
						case LEX_MOREEQUALS:	// G (не >=)
						case LEX_LESSEQUALS:	// L (не <=)
							// Операторы сравнения обрабатываются как разделители
							break;
						}
						lexema = *curword;
						break;
					}

					case LEX_LITERAL_HEX:
					{
						int value;
						if (lexema == LEX_LITERAL_HEX) {
							value = DecimicalNotation(curword, 16);
						}
						else {
							value = DecimicalNotation(curword, 2);
						}
						tables.idtable.table[tables.idtable.size - 1].value.vint = value;
						bool isNegative = value < 0;
						if (isNegative) {
							value = -value;
						}
						int x = 0;
						char* str = (char*)malloc(12 * sizeof(char));
						while (value > 9) {
							str[x++] = (value % 10) + '0';
							value = value / 10;
						}
						str[x++] = value + '0';
						if (isNegative) {
							str[x++] = '-';
						}
						str[x] = '\0';
						char t;
						for (int u = 0; u < x / 2; u++) {
							t = str[u];
							str[u] = str[x - 1 - u];
							str[x - 1 - u] = t;
						}
						strcpy_s(curword, "");
						strcat(curword, str);
						free(str);
						lexema = 'l';
					}
					break;

					case LEX_LITERAL_OCT:
					{
						int value = DecimicalNotation(curword, 8);
						tables.idtable.table[tables.idtable.size - 1].value.vint = value;
						bool isNegative = value < 0;
						if (isNegative) {
							value = -value;
						}
						int x = 0;
						char* str = (char*)malloc(12 * sizeof(char));
						while (value > 9) {
							str[x++] = (value % 10) + '0';
							value = value / 10;
						}
						str[x++] = value + '0';
						if (isNegative) {
							str[x++] = '-';
						}
						str[x] = '\0';
						char t;
						for (int u = 0; u < x / 2; u++) {
							t = str[u];
							str[u] = str[x - 1 - u];
							str[x - 1 - u] = t;
						}
						strcpy_s(curword, "");
						strcat(curword, str);
						free(str);
						lexema = 'l';
					}
					break;

					case LEX_ID:
					{
						char* str = new char[256];
						str = in.words[i].word;
						if (strlen(str) > MAXSIZE_ID) {
							Log::writeError(log.stream, Error::GetError(204, curline, 0));
							lex_ok = false;
							break;
						}
					}

					case LEX_LITERAL:
					{
						char id[STR_MAXSIZE] = "";
						idxTI = NULLDX_TI;
						if (*nextword == LEX_LEFTHESIS)
							isFunc = true;
						char* idtype = (isFunc && i > 1) ?
							in.words[i - 2].word : in.words[i - 1].word;
						if (!isFunc && !scopes.empty())
							strncpy_s(id, scopes.top(), MAXSIZE_ID);
						strncat(id, curword, MAXSIZE_ID);
						if (isLiteral(curword))
							strcpy_s(id, curword);

						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, lex_ok);
						if (itentry != nullptr)
						{
							if (isFunc)
							{
								if (getStandFunction(id) == IT::STDFNC::F_NOT_STD)
								{
									itentry->value.params.count = NULL;
									itentry->value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
									for (int k = i; in.words[k].word[0] != LEX_RIGHTTHESIS; k++)
									{
										if (k == In::InWord::size || in.words[k].word[0] == LEX_SEPARATOR)
											break;
										if (ISTYPE(in.words[k].word))
										{
											if (itentry->value.params.count >= MAX_PARAMS_COUNT)
											{
												Log::writeError(log.stream, Error::GetError(306, curline, 0));
												lex_ok = false;
												break;
											}
											itentry->value.params.types[itentry->value.params.count++] = getType(NULL, in.words[k].word);
										}
									}
								}
							}
							IT::Add(tables.idtable, *itentry);
							idxTI = tables.idtable.size - 1;
						}
						else
						{
							int i = tables.lextable.size - 1;
							if (i > 0 && tables.lextable.table[i - 1].lexema == LEX_VAR || tables.lextable.table[i].lexema == LEX_VAR
								|| tables.lextable.table[i - 1].lexema == LEX_FUNCTION || tables.lextable.table[i].lexema == LEX_FUNCTION
								|| tables.lextable.table[i - 1].lexema == LEX_ID_TYPE || tables.lextable.table[i].lexema == LEX_ID_TYPE
								|| tables.lextable.table[i - 1].lexema == LEX_VOID || tables.lextable.table[i].lexema == LEX_VOID)
							{
								Log::writeError(log.stream, Error::GetError(305, curline, 0));
								lex_ok = false;
							}
							idxTI = IT::isId(tables.idtable, id);
							// Если не найден с префиксом области видимости, попробуем найти без префикса
							if (idxTI == NULLIDX_TI && !scopes.empty())
							{
								idxTI = IT::isId(tables.idtable, curword);
							}
							if (lexema == LEX_LITERAL)
								idxTI = getLiteralIndex(tables.idtable, curword, getType(id, in.words[i - 1].word), log, curline);
						}
					}
					break;
					}

					LT::Entry* ltentry = new LT::Entry(lexema, curline, idxTI);
					LT::Add(tables.lextable, *ltentry);
					break;
				}
				else if (j == N_GRAPHS - 1)
				{
					Log::writeError(log.stream, Error::GetError(201, curline, 0));
					lex_ok = false;
				}
			}
		}

		if (enterPoint == NULL)
		{
			Log::writeError(log.stream, Error::GetError(301));
			lex_ok = false;
		}
		if (enterPoint > 1)
		{
			Log::writeError(log.stream, Error::GetError(302));
			lex_ok = false;
		}
		return lex_ok;
	}
};
