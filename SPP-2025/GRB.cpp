#include "Header.h"
#define GRB_ERROR_SERIES 600

typedef short GRBALPHABET;

namespace GRB
{
	Greibach greibach(NS('S'), TS('$'), 20,

		Rule(NS('S'), GRB_ERROR_SERIES, 3,	
			Rule::Chain(6, TS('t'), TS('f'), TS('i'), NS('P'), NS('T'), NS('S')),
			Rule::Chain(6, TS('p'), TS('f'), TS('i'), NS('P'), NS('G'), NS('S')),
			Rule::Chain(4, TS('m'), TS('['), NS('K'), TS(']'))
		),

		Rule(NS('T'), GRB_ERROR_SERIES + 2, 2,
			Rule::Chain(5, TS('['), TS('e'), NS('V'), TS(';'), TS(']')),
			Rule::Chain(6, TS('['), NS('K'), TS('e'), NS('V'), TS(';'), TS(']'))
		),

		Rule(NS('G'), GRB_ERROR_SERIES + 3, 2,
			Rule::Chain(4, TS('['), TS('e'), TS(';'), TS(']')),
			Rule::Chain(5, TS('['), NS('K'), TS('e'), TS(';'), TS(']'))
		),

		Rule(NS('P'), GRB_ERROR_SERIES + 1, 2,
			Rule::Chain(3, TS('('), NS('E'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('E'), GRB_ERROR_SERIES + 4, 2,					
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('E')),
			Rule::Chain(2, TS('t'), TS('i'))
		),

		Rule(NS('F'), GRB_ERROR_SERIES + 5, 2,					
			Rule::Chain(3, TS('('), NS('N'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('N'), GRB_ERROR_SERIES + 6, 4,		
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('i'), TS(','), NS('N')),
			Rule::Chain(3, TS('l'), TS(','), NS('N'))
		),


		Rule(NS('L'), GRB_ERROR_SERIES + 10, 6,		
			Rule::Chain(1, TS('<')),
			Rule::Chain(1, TS('>')),
			Rule::Chain(1, TS('=')),
			Rule::Chain(1, TS('N')),  // !=
			Rule::Chain(1, TS('G')),  // >=
			Rule::Chain(1, TS('L'))   // <= (терминал, не рекурсия)
		),

		// Q - первичные выражения (идентификаторы, литералы, скобки, функции)
		Rule(NS('Q'), GRB_ERROR_SERIES + 11, 5,
			Rule::Chain(1, TS('i')),           // идентификатор
			Rule::Chain(1, TS('l')),           // литерал
			Rule::Chain(3, TS('('), NS('Y'), TS(')')),  // арифметическое выражение в скобках
			Rule::Chain(5, TS('('), NS('Y'), NS('L'), NS('Y'), TS(')')),  // сравнение в скобках
			Rule::Chain(2, TS('i'), NS('F'))   // вызов функции
		),

		// R - битовые сдвиги (высший приоритет после первичных)
		Rule(NS('R'), GRB_ERROR_SERIES + 12, 6,
			Rule::Chain(1, NS('Q')),           // первичное выражение
			Rule::Chain(3, NS('Q'), TS('{'), NS('R')),  // сдвиг влево
			Rule::Chain(3, NS('Q'), TS('}'), NS('R')),  // сдвиг вправо
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(1, TS('i')),           // i -> Q -> R
			Rule::Chain(1, TS('l')),           // l -> Q -> R
			Rule::Chain(2, TS('i'), NS('F'))   // i F -> Q -> R
		),

		// U - факторы (умножение, деление, остаток)
		Rule(NS('U'), GRB_ERROR_SERIES + 13, 10,
			Rule::Chain(1, NS('R')),          // битовый сдвиг
			Rule::Chain(3, NS('R'), TS('*'), NS('U')),  // умножение
			Rule::Chain(3, NS('R'), TS('/'), NS('U')),  // деление
			Rule::Chain(3, NS('R'), TS('%'), NS('U')),  // остаток
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(1, TS('i')),           // i -> Q -> R -> U
			Rule::Chain(1, TS('l')),           // l -> Q -> R -> U
			Rule::Chain(2, TS('i'), NS('F')),  // i F -> Q -> R -> U
			Rule::Chain(3, TS('i'), TS('*'), NS('U')),  // i * U (развернуто для бинарных операций)
			Rule::Chain(3, TS('l'), TS('*'), NS('U')),  // l * U (развернуто для бинарных операций)
			Rule::Chain(3, TS('i'), TS('/'), NS('U')),  // i / U (развернуто для бинарных операций)
			Rule::Chain(3, TS('l'), TS('/'), NS('U'))   // l / U (развернуто для бинарных операций)
		),

		// Y - термы (сложение, вычитание)
		Rule(NS('Y'), GRB_ERROR_SERIES + 14, 11,
			Rule::Chain(1, NS('U')),           // фактор
			Rule::Chain(3, NS('U'), TS('+'), NS('Y')),  // сложение
			Rule::Chain(3, NS('U'), TS('-'), NS('Y')),  // вычитание
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(1, TS('i')),           // i -> Q -> R -> U -> Y
			Rule::Chain(1, TS('l')),           // l -> Q -> R -> U -> Y
			Rule::Chain(3, TS('i'), TS('+'), NS('Y')),  // i + Y (сложение)
			Rule::Chain(3, TS('l'), TS('+'), NS('Y')),  // l + Y (сложение)
			Rule::Chain(3, TS('i'), TS('-'), NS('Y')),  // i - Y (вычитание)
			Rule::Chain(3, TS('l'), TS('-'), NS('Y')),  // l - Y (вычитание)
			Rule::Chain(3, TS('i'), TS('*'), NS('U')),  // i * U -> Y (умножение)
			Rule::Chain(3, TS('l'), TS('*'), NS('U')),  // l * U -> Y (умножение)
			Rule::Chain(3, TS('i'), TS('/'), NS('U')),  // i / U -> Y (деление)
			Rule::Chain(3, TS('l'), TS('/'), NS('U'))   // l / U -> Y (деление)
		),

		// Z - выражения сравнения (низший приоритет)
		Rule(NS('Z'), GRB_ERROR_SERIES + 15, 6,
			Rule::Chain(1, NS('Y')),           // терм
			Rule::Chain(3, NS('Y'), NS('L'), NS('Y')),  // сравнение (бинарное)
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(1, TS('i')),           // i -> Q -> R -> U -> Y -> Z
			Rule::Chain(1, TS('l')),           // l -> Q -> R -> U -> Y -> Z
			Rule::Chain(3, TS('i'), TS('+'), NS('Y')),  // i + Y -> Z (развернуто для бинарных операций)
			Rule::Chain(3, TS('l'), TS('+'), NS('Y'))   // l + Y -> Z (развернуто для бинарных операций)
		),

		Rule(NS('V'), GRB_ERROR_SERIES + 16, 3,
			Rule::Chain(1, TS('l')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('h'))
		),

		Rule(NS('W'), GRB_ERROR_SERIES + 17, 13,
			Rule::Chain(1, NS('Z')),            // выражение
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(1, TS('i')),            // i -> Q -> R -> U -> Y -> Z -> W
			Rule::Chain(1, TS('l')),            // l -> Q -> R -> U -> Y -> Z -> W
			Rule::Chain(3, TS('i'), TS('+'), NS('Y')),  // i + Y -> Z -> W (сложение)
			Rule::Chain(3, TS('l'), TS('+'), NS('Y')),  // l + Y -> Z -> W (сложение)
			Rule::Chain(3, TS('i'), TS('-'), NS('Y')),  // i - Y -> Z -> W (вычитание)
			Rule::Chain(3, TS('l'), TS('-'), NS('Y')),  // l - Y -> Z -> W (вычитание)
			Rule::Chain(3, TS('i'), TS('*'), NS('U')),  // i * U -> Y -> Z -> W (умножение)
			Rule::Chain(3, TS('l'), TS('*'), NS('U')),  // l * U -> Y -> Z -> W (умножение)
			Rule::Chain(3, TS('i'), TS('/'), NS('U')),  // i / U -> Y -> Z -> W (деление)
			Rule::Chain(3, TS('l'), TS('/'), NS('U')),  // l / U -> Y -> Z -> W (деление)
			Rule::Chain(3, TS('('), NS('Y'), TS(')')),  // (Y) -> Q -> R -> U -> Y -> Z -> W (арифметическое выражение в скобках)
			Rule::Chain(5, TS('('), NS('Y'), NS('L'), NS('Y'), TS(')'))  // (Y L Y) -> Q -> R -> U -> Y -> Z -> W (сравнение в скобках)
		),

		Rule(NS('K'), GRB_ERROR_SERIES + 18, 13,
			Rule::Chain(7, TS('n'), TS('t'), TS('i'), TS('='), NS('V'), TS(';'), NS('K')),
			Rule::Chain(5, TS('n'), TS('t'), TS('i'), TS(';'), NS('K')),
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('K')),

			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('K')),
			Rule::Chain(4, TS('i'), NS('F'), TS(';'), NS('K')),
			Rule::Chain(4, TS('s'), TS('i'), TS('['), NS('A')),
			Rule::Chain(6, TS('n'), TS('t'), TS('i'), TS('='), NS('V'), TS(';')),
			Rule::Chain(4, TS('i'), TS('='), NS('W'), TS(';')),
			Rule::Chain(4, TS('n'), TS('t'), TS('i'), TS(';')),
			Rule::Chain(3, TS('o'), NS('V'), TS(';')),
			Rule::Chain(3, TS('i'), NS('F'), TS(';')),
			Rule::Chain()		// epsilon - пустое правило для завершения последовательности операторов
		),

		Rule(NS('X'), GRB_ERROR_SERIES + 19, 14,	
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('X')),
			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('X')),
			Rule::Chain(4, TS('i'), NS('F'), TS(';'), NS('X')),
			Rule::Chain(3, TS('B'), TS(';'), NS('X')),				// break + ; + продолжение

			Rule::Chain(4, TS('i'), TS('='), NS('W'), TS(';')),
			Rule::Chain(3, TS('o'), NS('V'), TS(';')),
			Rule::Chain(3, TS('i'), NS('F'), TS(';')),
			Rule::Chain(2, TS('B'), TS(';')),						// break + ; (конец)
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(4, TS('o'), TS('l'), TS(';'), NS('X')),		// o l ; X (output + literal + ; + продолжение)
			Rule::Chain(3, TS('o'), TS('l'), TS(';')),				// o l ; (output + literal + ; конец)
			Rule::Chain(4, TS('o'), TS('i'), TS(';'), NS('X')),		// o i ; X (output + identifier + ; + продолжение)
			Rule::Chain(3, TS('o'), TS('i'), TS(';')),				// o i ; (output + identifier + ; конец)
			Rule::Chain(3, TS('B'), TS(';'), NS('X')),				// B ; X (break + ; + продолжение) - развернутое
			Rule::Chain(2, TS('B'), TS(';'))						// B ; (break + ; конец) - развернутое
		),

		Rule(NS('A'), GRB_ERROR_SERIES + 20, 10,				// тело switch (SW -> A)
			// Правило для двух case'ов подряд с default (полностью развернутое, включая SD) - самое специфичное, должно быть первым
			Rule::Chain(19, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), TS('D'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), TS(']')),	// C l o l ; B ; C l o l ; B ; D o l ; B ; ] (два case'а + default + закрывающая скобка, полностью развернуто)
			// Правило для двух case'ов подряд с default (полностью развернутое)
			Rule::Chain(15, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), NS('I'), TS(']')),	// C l o l ; B ; C l o l ; B ; I ] (два case'а + default + закрывающая скобка)
			// Развернутые правила для последовательности case'ов, начинающиеся с C
			Rule::Chain(10, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), NS('H'), NS('I'), TS(']')),	// C l o l ; B ; H I ] (case + продолжение + default + закрывающая скобка)
			Rule::Chain(9, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), NS('H'), TS(']')),	// C l o l ; B ; H ] (case + продолжение + закрывающая скобка)
			Rule::Chain(8, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), TS(']')),	// C l o l ; B ; ] (один case + закрывающая скобка)
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(5, TS('C'), TS('l'), NS('X'), NS('I'), TS(']')),	// C l X I ] (case + литерал + тело + default + закрывающая скобка)
			Rule::Chain(4, TS('C'), TS('l'), NS('X'), TS(']')),			// C l X ] (case + литерал + тело + закрывающая скобка)
			Rule::Chain(3, NS('H'), NS('I'), TS(']')),					// case default и закрывающая скобка
			Rule::Chain(2, NS('H'), TS(']'))							// case и закрывающая скобка
		),

		Rule(NS('H'), GRB_ERROR_SERIES + 21, 6,				// последовательность case'ов (SC -> H)
			Rule::Chain(4, TS('C'), TS('l'), NS('X'), NS('H')),			// case + литерал + тело + продолжение
			Rule::Chain(3, TS('C'), TS('l'), NS('X')),					// case + литерал + тело (конец)
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(7, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';')),		// C l o l ; B ; (case + literal + output literal + ; + break + ; конец)
			Rule::Chain(8, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';'), NS('H')),	// C l o l ; B ; H (case + literal + output literal + ; + break + ; + продолжение)
			Rule::Chain(6, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'), NS('H')),	// C l o l ; H (case + literal + output literal + ; + продолжение)
			Rule::Chain(5, TS('C'), TS('l'), TS('o'), TS('l'), TS(';'))		// C l o l ; (case + literal + output literal + ; конец)
		),

		Rule(NS('I'), GRB_ERROR_SERIES + 22, 3,				// default (SD -> I)
			Rule::Chain(2, TS('D'), NS('X')),							// default + тело
			// Развернутые правила для терминалов (форма Грейбах)
			Rule::Chain(6, TS('D'), TS('o'), TS('l'), TS(';'), TS('B'), TS(';')),		// D o l ; B ; (default + output literal + ; + break + ;)
			Rule::Chain(4, TS('D'), TS('o'), TS('l'), TS(';'))				// D o l ; (default + output literal + ;)
		)
	);


	Rule::Chain::Chain(short psize, GRBALPHABET s, ...)
	{
		nt = new GRBALPHABET[size = psize];
		int* p = (int*)&s;
		for (short i = 0; i < psize; ++i)
			nt[i] = (GRBALPHABET)p[i];
	};

	Rule::Rule(GRBALPHABET pnn, int piderror, short psize, Chain c, ...)
	{
		nn = pnn;
		iderror = piderror;
		chains = new Chain[size = psize];
		Chain* p = &c;
		for (int i = 0; i < size; ++i)
			chains[i] = p[i];
	};


	Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottom, short psize, Rule r, ...)
	{
		startN = pstartN;
		stbottomT = pstbottom;
		rules = new Rule[size = psize];
		Rule* p = &r;
		for (int i = 0; i < size; ++i)
			rules[i] = p[i];
	}

	Greibach getGreibach()
	{
		return greibach;
	}

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		short rc = -1;
		short k = 0;
		while (k < size && rules[k].nn != pnn)
			++k;
		if (k < size)
			prule = rules[rc = k];
		return rc;
	}

	Rule Greibach::getRule(short n)
	{
		Rule rc;
		if (n < size)
			rc = rules[n];
		return rc;
	};

	char* Rule::getCRule(char* b, short nchain)
	{
		char bchain[200];
		b[0] = Chain::alphabet_to_char(nn);
		b[1] = '-';
		b[2] = '>';
		b[3] = 0x00;
		chains[nchain].getCChain(bchain);
		strcat_s(b, sizeof(bchain) + 5, bchain);
		return b;
	};

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		short rc = -1;
		while (j < size && chains[j].nt[0] != t)
			++j;
		rc = (j < size ? j : -1);
		if (rc >= 0)
			pchain = chains[rc];
		return rc;
	}

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < size; ++i)
			b[i] = alphabet_to_char(nt[i]);
		b[size] = 0x00;
		return b;
	}

}