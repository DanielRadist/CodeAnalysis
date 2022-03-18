const bool boop = true;			// объявление глобальной константы

bool kek (int a, short b_) {	// объявление функции

	return boop;				// возврат из функции
}

int main()						// функция main
{
	int a = kek(1, 2);
	//kek1(10, -100);				// ИНДЕНТИФИКАТОР НЕ ОПРЕДЕЛЕН
	
	switch (a)					// объявление switch
	{
	case 1:						// case c выходом
		int s1 = true;
		break;
	//case 
	case 3:						// case без выхода
		int s1 = 14;
		int s3 = 400;	
	default:					//default c выходом
		int s5 = 0;
		int s6 = s3;			// ИНДЕНТИФИКАТОР НЕ ОПРЕДЕЛЕН
		int s7 = s5 + (s6 * a);
		break;
	}
	
	return 0;
}