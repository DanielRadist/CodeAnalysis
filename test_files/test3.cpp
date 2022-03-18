const bool boop = true;			// объявление глобальной константы

bool kek (int a, short b_) {	// объявление функции

	boop = false;				// ИЗМЕНЕНИЕ КОНСТАНТЫ
	return boop;				// возврат из функции
}

int main()						// функция main
{
	int a = kek(1, 2);
	
	return 0;
}