int a = 5;
bool b = true;
int c;
bool d;

bool kek(int ww, bool boop)
{
	int b0 = true;				// приведение типов
	int b1 = false;				//
	bool b2 = 4;				//
	bool b3 = 0;				//
	
	c = 40;						// изменение значений...
	d = true;					// ...со значений по умолчанию
	return d;
}

int main()
{
	d = false;
	int tmp = a;				// присвоение переменным...	
	b = d;						// значений других переменных
	return 0;
}