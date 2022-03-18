int main()						// функция main
{
	int a = 2;
	switch (a)					// объявление switch
	{
	case 1:						// case c выходом
		int s1 = true;
		break;
	default:					//default c выходом
		int s5 = 0;
		int s6 = false;
		int s7 = s5 + (s6 * a);
		break;
	case 3:						// case без выхода
		int s1 = 14;
		int s3 = 400;	
	}
	
	return 0;
}