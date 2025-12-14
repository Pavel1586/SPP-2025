#include "Generator.h"
#include "Parm.h"
#include "LexAnalysis.h"
#include "IT.h"
#include "LT.h"
#include <sstream>
#include <cstring>
#include <iosfwd>
#include <stack>
#include <vector>

using namespace std;


namespace Gener
{
	static int conditionnum = 0;
	string numberOfPoints = "k";
	string itoS(int x) { stringstream r;  r << x;  return r.str(); }

	string genCallFuncCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;
		IT::Entry e = ITENTRY(i);
		stack <IT::Entry> temp;
		bool stnd = (e.idtype == IT::IDTYPE::S);

		for (int j = i + 1; LEXEMA(j) != LEX_RIGHTTHESIS; j++)
		{
			if (LEXEMA(j) == LEX_ID || LEXEMA(j) == LEX_LITERAL)
				temp.push(ITENTRY(j));	
		}
		str += "\n";

		while (!temp.empty())
		{
			if (temp.top().idtype == IT::IDTYPE::L && temp.top().iddatatype == IT::IDDATATYPE::STR || temp.top().iddatatype == IT::IDDATATYPE::CHAR)
				str = str + "push offset " + temp.top().id + "\n";
			else   str = str + "push " + temp.top().id + "\n";
			temp.pop();
		}
		if (stnd)
			str += "push offset buffer\n";
		str = str + "call " + string(e.id) + IN_CODE_ENDL;

		return str;
	}

	string genEqualCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;
		IT::Entry e1 = ITENTRY(i - 1);
		switch (e1.iddatatype)
		{
		case IT::IDDATATYPE::INT:
		{
			bool first = true;
			for (int j = i + 1; LEXEMA(j) != LEX_SEPARATOR; j++)
			{
				switch (LEXEMA(j))
				{
				case LEX_LITERAL:
				case LEX_ID:
				{
					if (ITENTRY(j).idtype == IT::IDTYPE::F || ITENTRY(j).idtype == IT::IDTYPE::S)
					{
						str = str + genCallFuncCode(tables, log, j);
						str = str + "push eax\n";
						while (LEXEMA(j) != LEX_RIGHTTHESIS) j++;
						break;
					}
					else  str = str + "push " + ITENTRY(j).id + "\n";
					break;
				}
				case LEX_PLUS:
					str = str + "pop ebx\npop eax\nadd eax, ebx\npush eax\n"; break;
				case LEX_MINUS:
					str = str + "pop ebx\npop eax\nsub eax, ebx\njnc b" + numberOfPoints + "\n" + "neg eax\n" + "b" + numberOfPoints + ": \n" + "push eax\n"; numberOfPoints = numberOfPoints + "m"; break;
				case LEX_STAR:
					str = str + "pop ebx\npop eax\nimul eax, ebx\npush eax\n"; break;
				case LEX_DIRSLASH:
					str = str + "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n"; break;
				case LEX_PERSENT:
					str = str + "pop ebx\npop eax\ncdq\nmov edx,0\nidiv ebx\npush edx\n"; break;
				case LEX_RIGHT:
					str = str + "pop ebx \npop eax \nmov cl, bl \nshr eax, cl\npush eax\n"; break;
				case LEX_LEFT:
					str = str + "pop ebx \npop eax \nmov cl, bl \nshl eax, cl\npush eax\n"; break;
				case LEX_MORE:  // >
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetg al\nmovzx eax, al\npush eax\n"; break;
				case LEX_LESS:  // <
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetl al\nmovzx eax, al\npush eax\n"; break;
				case LEX_EQUALS:  // =
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetz al\nmovzx eax, al\npush eax\n"; break;
				case LEX_NOTEQUALS:  // != (N)
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetnz al\nmovzx eax, al\npush eax\n"; break;
				case LEX_MOREEQUALS:  // >= (G)
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetge al\nmovzx eax, al\npush eax\n"; break;
				case LEX_LESSEQUALS:  // <= (L)
					str = str + "pop ebx\npop eax\ncmp eax, ebx\nsetle al\nmovzx eax, al\npush eax\n"; break;
				}
			}

			str = str + "\npop ebx\nmov " + e1.id + ", ebx\n";
			break;
		}
		case IT::IDDATATYPE::STR:
		{
			char lex = LEXEMA(i + 1);
			IT::Entry e2 = ITENTRY(i + 1);
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				str += genCallFuncCode(tables, log, i + 1);
				str = str + "mov " + e1.id + ", eax";
			}
			else if (lex == LEX_LITERAL)
			{
				str = str + "mov " + e1.id + ", offset " + e2.id;
			}
			else
			{
				str = str + "mov ecx, " + e2.id + "\nmov " + e1.id + ", ecx";
			}
			break;
		}
		case IT::IDDATATYPE::CHAR:
		{
			char lex = LEXEMA(i + 1);
			IT::Entry e2 = ITENTRY(i + 1);
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				str += genCallFuncCode(tables, log, i + 1);
				str = str + "mov " + e1.id + ", eax";
			}
			else if (lex == LEX_LITERAL)
			{
				str = str + "mov " + e1.id + ", offset " + e2.id;
			}
			else
			{
				str = str + "mov ecx, " + e2.id + "\nmov " + e1.id + ", ecx";
			}
			break;
		}
		}
		return str;
	}

	string genFunctionCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string str;
		IT::Entry e = ITENTRY(i + 1);
		IT::IDDATATYPE type = e.iddatatype;
		str = SEPSTR(funcname) + string(e.id) + string(" PROC,\n\t");

		int j = i + 3;
		while (LEXEMA(j) != LEX_RIGHTTHESIS)
		{
			if (LEXEMA(j) == LEX_ID)
				str = str + string(ITENTRY(j).id) + (ITENTRY(j).iddatatype == IT::IDDATATYPE::INT ? " : sdword, " : " : dword, ");
			j++;
		}
		int f = str.rfind(',');
		if (f > 0)
			str[f] = IN_CODE_SPACE;
		str += "\n; --- save registers ---\npush ebx\npush edx\n; ----------------------";
		return str;
	}

	string genExitCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string str = "; --- restore registers ---\npop edx\npop ebx\n; -------------------------\n";
		if (LEXEMA(i + 1) != LEX_SEPARATOR)
		{
			str = str + "mov eax, " + string(ITENTRY(i + 1).id) + "\n";
		}
		str += "ret\n";
		str += funcname + " ENDP" + SEPSTREMP;
		return str;
	}

	string genForCode(Lexer::LEX& tables, int i, string& forcode)
	{
		string str;
		conditionnum++;
		forcode.clear();
		
		IT::Entry initVar = ITENTRY(i + 2);
		IT::Entry initVal = ITENTRY(i + 4);
		
		str = "mov " + string(initVar.id) + ", " + (initVal.idtype == IT::IDTYPE::L ? itoS(initVal.value.vint) : string(initVal.id)) + "\n";
		
		str = str + "for_loop" + itoS(conditionnum) + ":\n";
		
		IT::Entry leftCond = ITENTRY(i + 6);
		IT::Entry rightCond = ITENTRY(i + 8);
		char op = LEXEMA(i + 7);
		
		str = str + "mov edx, " + string(leftCond.id) + "\ncmp edx, " + (rightCond.idtype == IT::IDTYPE::L ? itoS(rightCond.value.vint) : string(rightCond.id)) + "\n";
		
		string jmpStr;
		switch (op)
		{
		case LEX_MORE: jmpStr = "jg"; break;
		case LEX_LESS: jmpStr = "jl"; break;
		case LEX_MOREEQUALS: jmpStr = "jge"; break;
		case LEX_LESSEQUALS: jmpStr = "jle"; break;
		case LEX_EQUALS: jmpStr = "je"; break;
		case LEX_NOTEQUALS: jmpStr = "jne"; break;
		default: jmpStr = "jg"; break;
		}
		
		str = str + jmpStr + " for_end" + itoS(conditionnum) + "\n";
		forcode = str;
		
		return str;
	}

	string genConditionCode(Lexer::LEX& tables, int i, string& cyclecode)
	{
		string str;
		conditionnum++;
		cyclecode.clear();
		IT::Entry lft = ITENTRY(i + 1);
		IT::Entry rgt = ITENTRY(i + 3);
		bool w = false, r = false, c = false;
		string wstr, rstr, rstr2;

		// Поиск LEX_CYCLE в теле конструкции (до закрывающей скобки)
		for (int j = i + 5; j < tables.lextable.size && LEXEMA(j) != LEX_BRACELET; j++)
		{
			if (LEXEMA(j) == LEX_CYCLE) c = true;
		}
		str = str + "mov edx, " + lft.id + "\ncmp edx, " + rgt.id + "\n";
		switch (LEXEMA(i + 2))
		{
		case LEX_MORE:  rstr = "jg";  wstr = "jl";  break;
		case LEX_LESS:   rstr = "jl";  wstr = "jg";  break;
		case LEX_EQUALS:    rstr = "jz";  wstr = "jnz";  break;
		case LEX_NOTEQUALS:   rstr = "jnz";  wstr = "jz";  break;
		case LEX_MOREEQUALS:   rstr = "jz"; rstr2 = "jg";  wstr = "jnz";  break;
		case LEX_LESSEQUALS:   rstr = "jz"; rstr2 = "jl";  wstr = "jnz";  break;
		}

		if (LEXEMA(i + 2) == LEX_MORE || LEXEMA(i + 2) == LEX_LESS || LEXEMA(i + 2) == LEX_EQUALS || LEXEMA(i + 2) == LEX_NOTEQUALS) {
			if (!c && r) str = str + "\n" + rstr + " right" + itoS(conditionnum);
			if (!c && w) str = str + "\n" + wstr + " wrong" + itoS(conditionnum);
		}
		if (LEXEMA(i + 2) == LEX_MOREEQUALS || LEXEMA(i + 2) == LEX_LESSEQUALS) {
			if (!c && r) str = str + "\n" + rstr + " right" + itoS(conditionnum) + "\n" + rstr2 + " right" + itoS(conditionnum);
			if (!c && w) str = str + "\n" + wstr + " wrong" + itoS(conditionnum);
		}
		if (c)
		{
			str = str + "\n" + rstr + " cycle" + itoS(conditionnum);
			cyclecode = str;
			str = str + "\njmp cyclenext" + itoS(conditionnum);
		}
		else if (!r || !w)  str = str + "\njmp next" + itoS(conditionnum);
		return str;
	}

	vector <string> startFillVector(Lexer::LEX& tables)
	{
		vector <string> v;
		v.push_back(BEGIN);
		v.push_back(EXTERN);
		vector <string> vlt;  vlt.push_back(CONST);
		vector <string> vid;  vid.push_back(DATA);
		for (int i = 0; i < tables.idtable.size; i++)// const, data
		{
			IT::Entry e = tables.idtable.table[i];
			string str = "\t\t" + string(e.id);

			if (tables.idtable.table[i].idtype == IT::IDTYPE::L)
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::INT:  str = str + " sdword " + itoS(e.value.vint);  break;
				case IT::IDDATATYPE::STR:  str = str + " byte '" + string(e.value.vstr.str) + "', 0";  break;
				case IT::IDDATATYPE::CHAR:  str = str + " byte '" + string(e.value.vstr.str) + "', 0";  break;
				}
				vlt.push_back(str);
			}
			else if (tables.idtable.table[i].idtype == IT::IDTYPE::V)
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::INT: str = str + " dword 0";  break;
				case IT::IDDATATYPE::STR: str = str + " dword ?";  break;
				case IT::IDDATATYPE::CHAR: str = str + " dword ?";  break;
				}
				vid.push_back(str);
			}
		}
		v.insert(v.end(), vlt.begin(), vlt.end());
		v.insert(v.end(), vid.begin(), vid.end());
		v.push_back(CODE);
		return v;
	}

	void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log)
	{
		vector <string> v = startFillVector(tables);
		ofstream ofile(parm.out);
		string funcname;
		string cyclecode;
		int pcount;
		string str;
		bool inSwitch = false; // Флаг для отслеживания состояния switch

		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (LEXEMA(i))
			{
			case LEX_MAIN:
			{
				str = str + SEPSTR("MAIN") + "main PROC";
				break;
			}
			case LEX_FUNCTION:
			{
				funcname = ITENTRY(i + 1).id;
				pcount = ITENTRY(i + 1).value.params.count;
				str = genFunctionCode(tables, i, funcname, pcount);
				break;
			}
			case LEX_RETURN:
			{
				str = genExitCode(tables, i, funcname, pcount);
				break;
			}
			case LEX_ID:
			{
				if (LEXEMA(i + 1) == LEX_LEFTHESIS && LEXEMA(i - 1) != LEX_FUNCTION)
					str = genCallFuncCode(tables, log, i);
				break;
			}
			case LEX_BRACELET:
			{
				if (inSwitch)
				{
					// Генерируем метку конца switch
					str = str + "switch_end" + itoS(conditionnum) + ":\n";
					inSwitch = false; // Выходим из switch
				}
				else
				{
					// Генерация меток после закрывающей скобки для циклов
					bool c = false;
					for (int j = i; j > 0; j--)
						if (LEXEMA(j) == LEX_CYCLE)
						{
							c = true;
							break;
						}
					if (c && i + 1 < tables.lextable.size && LEXEMA(i + 1) != LEX_SEPARATOR)
						str = cyclecode + "\ncyclenext" + itoS(conditionnum) + ":";
				}
				break;
			}
			case LEX_CYCLE:
			{
				str = str + "cycle" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_EQUAL:
			{
				str = genEqualCode(tables, log, i);
				while (LEXEMA(++i) != LEX_SEPARATOR);
				break;
			}
			case LEX_WRITE:
			{
				IT::Entry e = ITENTRY(i + 1);
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::INT:
					str = str + "\npush " + e.id + "\ncall outlich\n";
					break;
				case IT::IDDATATYPE::STR:
					if (e.idtype == IT::IDTYPE::L)  str = str + "\npush offset " + e.id + "\ncall outrad\n";
					else  str = str + "\npush " + e.id + "\ncall outrad\n";
					break;
				case IT::IDDATATYPE::CHAR:
					if (e.idtype == IT::IDTYPE::L)  str = str + "\npush offset " + e.id + "\ncall outrad\n";
					else  str = str + "\npush " + e.id + "\ncall outrad\n";
					break;
				}
				break;
			}
			case LEX_SWITCH: // switch
			{
				conditionnum++;
				inSwitch = true; // Входим в switch
				IT::Entry e = ITENTRY(i + 1); // переменная для switch
				str = str + "mov eax, " + string(e.id) + "\n";
				break;
			}
			case LEX_CASE: // case
			{
				// Генерируем case_next перед новым case, если это не первый case в switch
				bool needCaseNext = false;
				if (i > 0)
				{
					// Ищем ближайший break или case перед текущим case
					// Игнорируем промежуточные элементы (output, литералы и т.д.)
					for (int j = i - 1; j >= 0; j--)
					{
						if (LEXEMA(j) == LEX_BREAK)
						{
							needCaseNext = true;
							break;
						}
						if (LEXEMA(j) == LEX_CASE)
						{
							needCaseNext = true;
							break;
						}
						if (LEXEMA(j) == LEX_SWITCH || LEXEMA(j) == LEX_BRACELET)
							break;
					}
				}
				if (needCaseNext)
					str = str + "case_next" + itoS(conditionnum) + ":\n";
				
				IT::Entry e = ITENTRY(i + 1); // значение для case
				str = str + "cmp eax, " + itoS(e.value.vint) + "\n";
				str = str + "jne case_next" + itoS(conditionnum) + "\n";
				break;
			}
			case LEX_DEFAULT: // default
			{
				// case_next не нужен перед default, так как default выполняется автоматически
				// если ни один case не сработал (последний jne case_next приведет к default)
				str = str + "default_case" + itoS(conditionnum) + ":\n";
				break;
			}
			case LEX_BREAK: // break
			{
				str = str + "jmp switch_end" + itoS(conditionnum) + "\n";
				// case_next будет сгенерирован перед следующим case или default
				break;
			}
			}
			if (!str.empty())
				v.push_back(str);
			str.clear();
		}
		v.push_back(END);

		for (auto x : v)
			ofile << x << endl;
		ofile.close();
	}
};