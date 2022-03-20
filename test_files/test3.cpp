const bool boop = true;			// объявление глобальной константы

bool kek (int a, bool b) {		// объявление функции
	int yem = -15;
	return a + boop * b;		// возврат из функции
}

int main()						// функция main
{
	int a = kek(1, 2);
	
	return 0;
}