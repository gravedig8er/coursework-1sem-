#include <iostream> 
#include <fstream> 
#include <cstdio>

bool get_quantity_n(std::fstream* ptr_f, int* ptr_n);
void zero_fill(double** points, const int* size);
void filling_arrays(std::fstream* ptr_f, double** points, const int* size);
int protocol_arrays(std::fstream* ptr_out, double** points, const int* size);
void transfer_arrays(double** res_points, double** points, const int* total_points, const int* size);
void output_result(std::fstream* ptr_res, double** res_points, const int* total_points);
void create_triangles(double** res_points, const int* size);
void get_inter_point(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double* x, double* y);
void memory_delete(double** arr);

int main()
{
	int n, total_points = 0;
	std::fstream f; // чтение из файла --- закрытие в filling_arrays
	std::fstream out; // занесение в протокол
	std::fstream res; // занесение в результат
	setlocale(LC_ALL, "Russian");
	if (!get_quantity_n(&f, &n)) // определение количества точек для объявления массива + открытие файла
		return 0;

	// создание массива. points[0][x]. points[1][y]
	double** points = new double* [2];
	for (int i = 0; i < 2; i++) points[i] = new double[n];

	// заполним массив значениями 1e-3
	zero_fill(points, &n);

	// заполнение массива из файла 
	filling_arrays(&f, points, &n);

	// запись в протокол считанных данных
	total_points = protocol_arrays(&out, points, &n);
	if (total_points < 3) return 0;
	//если total_points = 3??? что делаем 

	// изменение массивов для удобной работы
	double** res_points = new double* [2];
	for (int i = 0; i < 2; i++) res_points[i] = new double[total_points];

	// перенос данных в рабочий массив
	transfer_arrays(res_points, points, &total_points, &n);
	memory_delete(points);

	// вывод точек в result
	output_result(&res, res_points, &total_points);

	// создание треугольника и нахождение точек пересечения внутри ???
	create_triangles(res_points, &total_points);

	// очистка res_points
	memory_delete(res_points);

	return 0;
}

bool get_quantity_n(std::fstream* ptr_f, int* ptr_n)
{
	int counter_lines = 0;
	char symbol;
	ptr_f->open("D:/labs prog/kursach/points.txt", std::ios::in);
	if (!ptr_f->is_open())
	{
		return false;
	}
	if (!ptr_f->eof())
		*ptr_f >> *ptr_n;
	// проверка количества строк, т.к их может быть меньше/больше, чем "должных"
	if (*ptr_n < 3) return false;
	while (!ptr_f->eof())
	{
		double temp = -1e5;
		*ptr_f >> temp;
		if (temp != -1e5) // если число в строке есть. не важно 1 или 2 
		{
			char ch;
			ptr_f->get(ch);
			if (ch == '\n')
				counter_lines++;
		}
		else
			continue;
	}
	if (counter_lines < *ptr_n) *ptr_n = counter_lines;
	ptr_f->clear();
	ptr_f->seekg(0);
	return true;
}

void zero_fill(double** points, const int* size)
{
	for (int i = 0; i < *size; i++)
	{
		points[0][i] = 1e-3;
		points[1][i] = 1e-3;
	}
}

void filling_arrays(std::fstream* ptr_f, double** points, const int* size)
{
	double needless = 0;
	double temp;
	int i = 0;
	char symbol;
	if (!ptr_f->eof()) *ptr_f >> needless; // считываем размер, пропускаем его
	ptr_f->get(symbol); // считали переход строки после размера
	while (!ptr_f->eof() && i < needless)
	{
		ptr_f->get(symbol);
		if ((symbol == ' ' || symbol == '\t') && symbol != '\n') // считаем, что утеряно значение х и нужно считать только y 
		{
			ptr_f->seekg(-1, std::ios::cur); // вернули каретку назад 
			*ptr_f >> points[1][i]; // считали y, теперь нужно двигать каретку вперед, чтобы пропустить лишние значения
			while (true)
			{
				char symbol;
				ptr_f->get(symbol);
				if (symbol == '\n') break;
			}
			i++;
			continue;
		}
		else if (symbol != '\n')// х не потерян -> считываем
		{
			ptr_f->seekg(-1, std::ios::cur); // вернули каретку назад 
			*ptr_f >> points[0][i];
			while (true)
			{
				ptr_f->get(symbol);
				if (symbol != ' ' && symbol != '\t' && symbol != '\n') // если он цифра
				{
					ptr_f->seekg(-1, std::ios::cur);
					*ptr_f >> points[1][i]; // считываем y 
					while (true)
					{
						char ch;
						ptr_f->get(ch);
						if (ch == '\n')
						{
							ptr_f->seekg(-1, std::ios::cur);
							break;
						}
					}
					i++;
					break;
				}
				if (symbol == '\n') // утерян y, переходим дальше 
				{
					i++;
					break;
				}
			}
		}
		else // символ сразу переход строки 
			continue;
	}
	ptr_f->close();
}

int protocol_arrays(std::fstream* ptr_out, double** points, const int* size)
{
	int total_points = 0;
	ptr_out->open("D:/labs prog/kursach/protocol.txt", std::ios::out);
	if (!ptr_out->is_open())
	{
		std::cout << "Протокол не открылся \n";
		return 0;
	}
	*ptr_out << "Количество ожидаемых точек: " << *size << "\n\n";
	for (int i = 0; i < *size; i++)
	{
		*ptr_out << "Точка " << i + 1 << ": \t";
		if (points[0][i] == 1e-3 && points[1][i] != 1e-3)
		{
			*ptr_out << "не хватает х    " << "y: " << points[1][i] << '\n';
		}
		if (points[0][i] != 1e-3 && points[1][i] == 1e-3)
		{
			*ptr_out << "x: " << points[0][i] << "\t\t" << "не хватает y\n";
		}
		if (points[0][i] != 1e-3 && points[1][i] != 1e-3)
		{
			*ptr_out << "x: " << points[0][i] << "\t\t" << "y: " << points[1][i] << '\n';
			total_points++;
		}
	}
	*ptr_out << '\n';
	*ptr_out << "Всего точек: " << total_points << '\n';
	return total_points;
}

void transfer_arrays(double** res_points, double** points, const int* total_points, const int* size)
{
	int point = 0;
	for (int i = 0; i < *size; i++)
	{
		if (points[0][i] != 1e-3 && points[1][i] != 1e-3)
		{
			if (point == *total_points) break;
			res_points[0][point] = points[0][i];
			res_points[1][point] = points[1][i];
			point++;
		}
	}
	if (point != *total_points) std::cout << "ERROR\n";
}

void output_result(std::fstream* ptr_res, double** res_points, const int* total_points)
{
	ptr_res->open("D:/labs prog/kursach/result.txt", std::ios::out);
	if (!ptr_res->is_open())
	{
		std::cout << "Файл для результата не открылся \n";
	}
	*ptr_res << "Считано " << *total_points << " точек:\n\n";
	for (int i = 0; i < *total_points; i++)
	{
		*ptr_res << res_points[0][i] << "\t" << res_points[1][i] << '\n';
	}
	*ptr_res << '\n';
}

void create_triangles(double** res_points, const int* size)
{
	// перебор вершин для первого треугольника
	for (int i = 0; i < *size; i++)
	{
		for (int j = i + 1; j < *size; j++)
		{
			for (int k = j + 1; j < *size; j++)
			{
				// перебор вершин для второго треугольника 
				for (int i1 = 0; i1 < *size; i1++)
				{
					for (int j1 = i1 + 1; j1 < *size; j1++)
					{
						for (int k1 = j1 + 1; k1 < *size; k1++)
						{
							if (i != i1 && j != j1 && k != k1) // не может быть одинакового треугольника
							{
								double x_ij_j1k1, y_ij_j1k1, x_ij_j1i1, y_ij_j1i1, x_jk_j1k1, y_jk_j1k1, x_jk_i1k1, y_jk_i1k1, x_ik_j1i1, y_ik_j1i1, x_ik_i1k1, y_ik_i1k1; // координаты пересечения 
								get_inter_point(res_points[0][i], res_points[1][i], res_points[0][j], res_points[1][j], res_points[0][j1], res_points[1][j1], res_points[0][k1], res_points[1][k1], &x_ij_j1k1, &y_ij_j1k1);
								get_inter_point(res_points[0][i], res_points[1][i], res_points[0][j], res_points[1][j], res_points[0][j1], res_points[1][j1], res_points[0][i1], res_points[1][i1], &x_ij_j1i1, &y_ij_j1i1);
								get_inter_point(res_points[0][j], res_points[1][j], res_points[0][k], res_points[1][k], res_points[0][j1], res_points[1][j1], res_points[0][k1], res_points[1][k1], &x_jk_j1k1, &y_jk_j1k1);
								get_inter_point(res_points[0][j], res_points[1][j], res_points[0][k], res_points[1][k], res_points[0][i1], res_points[1][i1], res_points[0][k1], res_points[1][k1], &x_jk_i1k1, &y_jk_i1k1);
								get_inter_point(res_points[0][i], res_points[1][i], res_points[0][k], res_points[1][k], res_points[0][j1], res_points[1][j1], res_points[0][i1], res_points[1][i1], &x_ik_j1i1, &y_ik_j1i1);
								get_inter_point(res_points[0][i], res_points[1][i], res_points[0][k], res_points[1][k], res_points[0][i1], res_points[1][i1], res_points[0][k1], res_points[1][k1], &x_ik_i1k1, &y_ik_i1k1);
								std::cout << "вершина 1.1 треуг: " << res_points[0][i] << ' ' << res_points[1][i] << '\n';
								std::cout << "вершина 2.1 треуг: " << res_points[0][j] << ' ' << res_points[1][j] << '\n';
								std::cout << "вершина 1.2 треуг: " << res_points[0][j1] << ' ' << res_points[1][j1] << '\n';
								std::cout << "вершина 2.2 треуг: " << res_points[0][k1] << ' ' << res_points[1][k1] << '\n';
								std::cout << "Точка пересечения: " << x_ij_j1k1 << ' ' << y_ij_j1k1;
							}
						}
					}
				}
			}
		}
	}
}

void get_inter_point(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double* x, double* y)
{
	// уравнение для прямой 1-го треугольника
	double a1 = y2 - y1;
	double b1 = x1 - x2;
	double c1 = x2 * y1 - x1 * y2;

	// уравнениие для прямой 2-го треугольника
	double a2 = y4 - y3;
	double b2 = x3 - x4;
	double c2 = x4 * y3 - x3 * y4;

	// найдем точку пересечения 
	double det = a1 * b2 - a2 * b1;
	double det_x = -c1 * b2 + c2 * b1;
	double det_y = -a1 * c2 + a2 * c1;

	*x = det_x / det; // координата по x  
	*y = det_y / det; // координата по y 
}

void memory_delete(double** arr)
{

	for (int i = 0; i < 2; i++)
	{
		delete[] arr[i];
	}

	delete[] arr;
}

