#include <iostream>
#include <fstream>
#include "Scanner.h"
#include "CodeAnalysis.h"
int main()
{
	setlocale(LC_ALL, "rus");
	//std::ofstream fout("output.txt");
	SyntaxAnalyser analyser("test_files/test1.cpp");		// файл который проверяем
	try 
	{
		analyser.Program();						// вызов анализатора для проверки семантического уровня
	}
	catch (std::exception& e)					// если вызвалось исключение
	{
		std::cout << e.what();					// пишем в консоль
		return 1;
	}
	std::cout << "Analysis success";			// исключения не было

	return 0;
}
