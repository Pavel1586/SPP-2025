#include <iostream>
#include "Header.h"
#include <vector>
#include <sstream>
#include <locale>

#pragma warning(disable : 4996)

int wmain(int argc, wchar_t* argv[])
{
	char LEXERROR[] = "Лексический анализ не выполнен успешно";
	char SYNTERROR[] = "Синтаксический анализ не выполнен успешно";
	char SEMERROR[] = "Семантический анализ не выполнен успешно";
	char POLISHERROR[] = "Ошибка при построении обратной польской записи";
	char LEXGOOD[] = "Лексический анализ выполнен успешно";
	char SYNTGOOD[] = "Синтаксический анализ выполнен успешно";
	char SEMGOOD[] = "Семантический анализ выполнен успешно";
	char POLISHGOOD[] = "Обратная польская запись построена успешно";
	char MESSAGE[] = "--------------------Таблица лексем и идентификаторов-------------------";
	char STOP[] = "\nРабота программы завершена";
	char ALLGOOD[] = "Компиляция завершена успешно!";
	setlocale(LC_ALL, "Russian");
	Log::LOG log;
	std::vector<Error::ERROR> errors;  // Вектор для хранения ошибок

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);                            //получение параметров
		log = Log::getstream(parm.log);
		Log::writeLog(log);														//запись заголовка лога
		Log::writeParm(log, parm);												//запись в лог параметров
		In::IN in = In::getin(parm.in, log.stream);
		Log::writeIn(log.stream, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);	//получение ТС
		Lexer::LEX tables;
		bool lex_ok = false;
		// Сохраняем позицию в файле перед анализом для удаления ошибок из начала лога
		std::streampos posBeforeAnalysis = 0;
		if (log.stream != nullptr && log.stream->is_open())
		{
			log.stream->flush();
			posBeforeAnalysis = log.stream->tellp();
		}
		try
		{
			lex_ok = Lexer::analyze(tables, in, log, parm);					//выполнение лексического анализа
		}
		catch (Error::ERROR e)
		{
			errors.push_back(e);  // Сохраняем ошибку для вывода в конце
			lex_ok = false;
			// Удаляем ошибки, записанные в лог (откатываем файл к позиции до анализа)
			if (log.stream != nullptr && log.stream->is_open() && posBeforeAnalysis > 0)
			{
				log.stream->flush();
				log.stream->close();
				// Читаем содержимое файла до позиции ошибок
				std::string logFilePath;
				int len = wcslen(log.logfile);
				char* mbstr = new char[len + 1];
				wcstombs(mbstr, log.logfile, len + 1);
				logFilePath = mbstr;
				delete[] mbstr;
				std::ifstream readFile(logFilePath, std::ios::in | std::ios::binary);
				if (readFile.is_open())
				{
					readFile.seekg(0, std::ios::end);
					std::streampos fileSize = readFile.tellg();
					readFile.seekg(0, std::ios::beg);
					if (fileSize > posBeforeAnalysis)
					{
						// Читаем только часть до ошибок
						std::vector<char> buffer((size_t)posBeforeAnalysis);
						readFile.read(buffer.data(), posBeforeAnalysis);
						readFile.close();
						// Записываем обратно только эту часть
						std::ofstream writeFile(logFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
						if (writeFile.is_open())
						{
							writeFile.write(buffer.data(), posBeforeAnalysis);
							writeFile.close();
						}
					}
					else
					{
						readFile.close();
					}
				}
				// Открываем файл снова для добавления
				log.stream->open(log.logfile, std::ios::out | std::ios::app);
				// Устанавливаем локаль для правильного отображения кириллицы
				try {
					log.stream->imbue(std::locale(".1251"));
				}
				catch (...) {
					log.stream->imbue(std::locale(""));
				}
			}
		}
		// 1. Таблица лексем
		LT::writeLexTable(log.stream, tables.lextable);							//запись в лог таблицы лексем и идентификаторов 
		// 2. Таблица идентификаторов
		IT::writeIdTable(log.stream, tables.idtable);							//и таблицы идентификаторов в лог
		// 3. Лексемы, соответствующие исходному коду
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		if (!lex_ok)
		{
			Log::writeLine(&std::cout, LEXERROR, STOP, "");
			// 5. Выводим ошибки в конце
			for (const auto& e : errors)
			{
				if (e.position.col == -1 || e.position.line == -1)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
				else if (e.position.col == NULL)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line;
				else
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line
					<< " позиция в строке: " << e.position.col;
				*log.stream << std::endl;
			}
			return 0;
		}
		else
		{
			Log::writeLine(&std::cout, LEXGOOD, "");
		}
		// 4. Трассировка работы синтаксического анализатора
		MFST_TRACE_START(log);
		MFST::Mfst mfst(tables, GRB::getGreibach());							//выполнение синтаксического анализа
		bool synt_ok = mfst.start(log);
		if (!synt_ok)
		{
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			// Выводим ошибки в конце
			for (const auto& e : errors)
			{
				if (e.position.col == -1 || e.position.line == -1)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
				else if (e.position.col == NULL)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line;
				else
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line
					<< " позиция в строке: " << e.position.col;
				*log.stream << std::endl;
			}
			return 0;
		}
		else 
		{
			mfst.savededucation();
			mfst.printrules(log);													//вывод правил вывода
			Log::writeLine(&std::cout, SYNTGOOD, "");
		}
		// Сохраняем позицию перед семантическим анализом
		std::streampos posBeforeSem = 0;
		if (log.stream != nullptr && log.stream->is_open())
		{
			log.stream->flush();
			posBeforeSem = log.stream->tellp();
		}
		bool sem_ok = Semantic::semanticsCheck(tables, log);					//выполнение семантического анализа
		if (!sem_ok)
		{
			Log::writeLine(&std::cout, SEMERROR, STOP, "");
			// Удаляем ошибки, записанные в лог во время семантического анализа
			if (log.stream != nullptr && log.stream->is_open() && posBeforeSem > 0)
			{
				log.stream->flush();
				log.stream->close();
				std::string logFilePath;
				int len = wcslen(log.logfile);
				char* mbstr = new char[len + 1];
				wcstombs(mbstr, log.logfile, len + 1);
				logFilePath = mbstr;
				delete[] mbstr;
				std::ifstream readFile(logFilePath, std::ios::in | std::ios::binary);
				if (readFile.is_open())
				{
					readFile.seekg(0, std::ios::end);
					std::streampos fileSize = readFile.tellg();
					readFile.seekg(0, std::ios::beg);
					if (fileSize > posBeforeSem)
					{
						std::vector<char> buffer((size_t)posBeforeSem);
						readFile.read(buffer.data(), posBeforeSem);
						readFile.close();
						std::ofstream writeFile(logFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
						if (writeFile.is_open())
						{
							writeFile.write(buffer.data(), posBeforeSem);
							writeFile.close();
						}
					}
					else
					{
						readFile.close();
					}
				}
				log.stream->open(log.logfile, std::ios::out | std::ios::app);
			}
			// 5. Выводим ошибки в конце
			for (const auto& e : errors)
			{
				if (e.position.col == -1 || e.position.line == -1)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
				else if (e.position.col == NULL)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line;
				else
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line
					<< " позиция в строке: " << e.position.col;
				*log.stream << std::endl;
			}
			return 0;
		}
		else Log::writeLine(&std::cout, SEMGOOD, "");
		bool polish_ok = Polish::PolishNotation(tables, log);					//выполнение обратной польской записи в стек
		if (!polish_ok)
		{
			Log::writeLine(&std::cout, POLISHERROR, STOP, "");
			// Выводим ошибки в конце
			for (const auto& e : errors)
			{
				if (e.position.col == -1 || e.position.line == -1)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
				else if (e.position.col == NULL)
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line;
				else
					*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " строка: " << e.position.line
					<< " позиция в строке: " << e.position.col;
				*log.stream << std::endl;
			}
			return 0;
		}
		else Log::writeLine(&std::cout, POLISHGOOD, "");

		Gener::CodeGeneration(tables, parm, log);								//выполнение генерации кода
		
		// 5. Выводим ошибки в конце (если есть)
		for (const auto& e : errors)
		{
			if (e.position.col == -1 || e.position.line == -1)
				*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
			else if (e.position.col == NULL)
				*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " строка: " << e.position.line;
			else
				*log.stream << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " строка: " << e.position.line
				<< " позиция в строке: " << e.position.col;
			*log.stream << std::endl;
		}
	}
	catch (Error::ERROR e)
	{
		errors.push_back(e);  // Сохраняем ошибку
		// Выводим ошибки в конце
		for (const auto& err : errors)
		{
			if (err.position.col == -1 || err.position.line == -1)
				*log.stream << std::endl << "Ошибка N" << err.id << ": " << err.message;
			else if (err.position.col == NULL)
				*log.stream << std::endl << "Ошибка N" << err.id << ": " << err.message
				<< " строка: " << err.position.line;
			else
				*log.stream << std::endl << "Ошибка N" << err.id << ": " << err.message
				<< " строка: " << err.position.line
				<< " позиция в строке: " << err.position.col;
			*log.stream << std::endl;
		}
		std::cout << "Ошибка " << e.id << ": " << e.message << ", строка " << e.position.line << ", позиция " << e.position.col << std::endl;
		system("pause");
	}
	return 0;
}
