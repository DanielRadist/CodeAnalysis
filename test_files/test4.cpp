int main()						// функция main
{
	int a = 2;
	int s1;
	int s3;
	switch (a)					// объявление switch
	{
	case 1:						// case c выходом
		a++;
		break;
	case 3:						// case без выхода
		s1 = 14;
		s3 = 400;	
	default:					//default
		a = a * 2;
	}
	
	return 0;
}