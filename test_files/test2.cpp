const bool boop = true;			// объявление глобальной константы

bool kek (int a, bool b) {		// объявление функции
	return b + a;				// возврат из функции
}

int rer = 1;

int main()						// функция main
{
	int a = kek(1, boop);
	
	const int dva = 2;
	
	switch (a)					// объявление switch
	{
	case 1:
		a = rer + a;
		break;
	case 2:
		rer = dva;
	}
	
	return 0;
}