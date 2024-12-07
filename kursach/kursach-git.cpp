#include <iostream> 
#include <fstream> 

bool get_quantity_n(std::fstream* ptr_f, int* ptr_n);
void zero_fill(double** points, const int* size);
void filling_arrays(std::fstream* ptr_f, double** points, const int* size);
int protocol_arrays(std::fstream* ptr_out, double** points, const int* size);
void transfer_arrays(double** res_points, double** points, const int* total_points, const int* size);
void output_result(std::fstream* ptr_res, double** res_points, const int* total_points);
void create_triangles(std::fstream* ptr_out, std::fstream* ptr_res, double** res_points, const int* size);
void perm(double** tem_ar, int size);
bool get_intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double& ix, double& iy);
double min(double a, double b);
double max(double a, double b);
//void check_void_points();
bool check_triangle(double x1, double y1, double x2, double y2, double x3, double y3);
double get_sq_triangle(double x1, double y1, double x2, double y2, double x3, double y3);
double get_sq_square(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
double get_sq_pent(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5);
double get_sq_hex(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5, double x6, double y6);
double get_length(double x1, double y1, double x2, double y2);
bool check_point(double x1, double y1, double x2, double y2, double x3, double y3, double x, double y, double sq);
int get_quiantity_inter_point(double a, double b, double c, double d, double e, double f, bool res1, bool res2, bool res3, bool res4, bool res5, bool res6);
double** append(double** arr_vertex, int* length, int* capacity, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5, double x6, double y6);
double get_cross(double x1, double y1, double x2, double y2, double x3, double y3);
void memory_delete(double** arr);

int main()
{
	int n, total_points = 0;
	int total_triangles = 0;
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

	// создание треугольника и нахождение площадей внутри. возвращать количество треугольников? 
	// 
	create_triangles(&out, &res, res_points, &total_points);
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
	std::cout << "Количество ожидаемых точек: " << *size << "\n\n";
	for (int i = 0; i < *size; i++)
	{
		std::cout << "Точка " << i + 1 << ": \t";
		if (points[0][i] == 1e-3 && points[1][i] != 1e-3)
		{
			std::cout << "не хватает х    " << "y: " << points[1][i] << '\n';
		}
		if (points[0][i] != 1e-3 && points[1][i] == 1e-3)
		{
			std::cout << "x: " << points[0][i] << "\t\t" << "не хватает y\n";
		}
		if (points[0][i] != 1e-3 && points[1][i] != 1e-3)
		{
			std::cout << "x: " << points[0][i] << "\t\t" << "y: " << points[1][i] << '\n';
			total_points++;
		}
	}
	std::cout << '\n';
	std::cout << "Всего точек: " << total_points << "\n\n";
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

void create_triangles(std::fstream* ptr_out, std::fstream* ptr_res, double** res_points, const int* size)
{
	double max_sq = -1;
	bool flag = 0;
	int total_triangles = 0;
	int length = 0; // количество ЭЛЕМЕНТОВ в массиве сейчас! 
	int capacity = 6; // размер массива в данный момент 
	// массив для хранения вершин с максимальной площадью
	double** arr_vertex = new double* [2];
	for (int i = 0; i < 2; i++) arr_vertex[i] = new double[6];

	// перебор вершин для первого треугольника
	for (int i = 0; i < *size - 3; i++)
	{
		for (int j = i + 1; j < *size - 2; j++)
		{
			for (int k = j + 1; k < *size - 1; k++)
			{
				// перебор вершин для второго треугольника 
				for (int i1 = 0; i1 < *size - 3; i1++)
				{
					for (int j1 = i1 + 1; j1 < *size - 2; j1++)
					{
						for (int k1 = j1 + 1; k1 < *size - 1; k1++)
						{
							std::cout << "First triangle: "
								<< '(' << res_points[0][i] << ';' << res_points[1][i] << ')' << ' '
								<< '(' << res_points[0][j] << ';' << res_points[1][j] << ')' << ' '
								<< '(' << res_points[0][k] << ';' << res_points[1][k] << ')' << ' ';
							std::cout << "Second triangle: "
								<< '(' << res_points[0][i1] << ';' << res_points[1][i1] << ')' << ' '
								<< '(' << res_points[0][j1] << ';' << res_points[1][j1] << ')' << ' '
								<< '(' << res_points[0][k1] << ';' << res_points[1][k1] << ')' << ' ';
							if (((i == i1 && j == j1 && k == k1) ||
								(i == i1 && j == k1 && k == j1) ||
								(i == j1 && j == i1 && k == k1) ||
								(i == j1 && j == k1 && k == i1) ||
								(i == k1 && j == i1 && k == j1) ||
								(i == k1 && j == j1 && k == i1)))
							{
								std::cout << "Такой набор точек не подходит для создания треугольника \n\n"; continue;
							}
							if (!((i == i1 && j == j1 && k == k1) ||
								(i == i1 && j == k1 && k == j1) ||
								(i == j1 && j == i1 && k == k1) ||
								(i == j1 && j == k1 && k == i1) ||
								(i == k1 && j == i1 && k == j1) ||
								(i == k1 && j == j1 && k == i1))) // не может быть одинакового треугольника
							{
								std::cout << '\n';
								bool check_first = check_triangle(res_points[0][i], res_points[1][i], res_points[0][j], res_points[1][j], res_points[0][k], res_points[1][k]);
								bool check_second = check_triangle(res_points[0][i1], res_points[1][i1], res_points[0][j1], res_points[1][j1], res_points[0][k1], res_points[1][k1]);
								// проверка на существование треугольников (двух) 
								if (!check_first && !check_second) { std::cout << "Такой набор точек не подходит для создания треугольника \n\n"; continue; }
								else
								{
									// переназначение точек
									int counter = 0;
									// массив для работы с точками. 
									double** finally_ar = new double* [2];
									for (int v = 0; v < 2; v++) finally_ar[v] = new double[6];

									finally_ar[0][counter] = res_points[0][i];
									finally_ar[1][counter++] = res_points[1][i];

									finally_ar[0][counter] = res_points[0][j];
									finally_ar[1][counter++] = res_points[1][j];

									finally_ar[0][counter] = res_points[0][k];
									finally_ar[1][counter++] = res_points[1][k];

									finally_ar[0][counter] = res_points[0][i1];
									finally_ar[1][counter++] = res_points[1][i1];

									finally_ar[0][counter] = res_points[0][j1];
									finally_ar[1][counter++] = res_points[1][j1];

									finally_ar[0][counter] = res_points[0][k1];
									finally_ar[1][counter++] = res_points[1][k1];

									counter = 0;
									// массив для сортировки точек
									double** temp_ar = new double* [2];
									for (int v = 0; v < 2; v++) temp_ar[v] = new double[3];
									temp_ar[0][counter] = finally_ar[0][0];
									temp_ar[1][counter++] = finally_ar[1][0];

									temp_ar[0][counter] = finally_ar[0][1];
									temp_ar[1][counter++] = finally_ar[1][1];

									temp_ar[0][counter] = finally_ar[0][2];
									temp_ar[1][counter++] = finally_ar[1][2];

									perm(temp_ar, 3);

									finally_ar[0][0] = temp_ar[0][0];
									finally_ar[1][0] = temp_ar[1][0];

									finally_ar[0][1] = temp_ar[0][1];
									finally_ar[1][1] = temp_ar[1][1];

									finally_ar[0][2] = temp_ar[0][2];
									finally_ar[1][2] = temp_ar[1][2];

									counter = 0;

									temp_ar[0][counter] = finally_ar[0][3];
									temp_ar[1][counter++] = finally_ar[1][3];

									temp_ar[0][counter] = finally_ar[0][4];
									temp_ar[1][counter++] = finally_ar[1][4];

									temp_ar[0][counter] = finally_ar[0][5];
									temp_ar[1][counter++] = finally_ar[1][5];

									perm(temp_ar, 3);

									finally_ar[0][3] = temp_ar[0][1];
									finally_ar[1][3] = temp_ar[1][1];

									finally_ar[0][4] = temp_ar[0][0];
									finally_ar[1][4] = temp_ar[1][0];

									finally_ar[0][5] = temp_ar[0][2];
									finally_ar[1][5] = temp_ar[1][2];
									counter = 0;

									memory_delete(temp_ar);
									// поиск площадей исходных треугольников (передаются три вершины) 
									double sq_triangle_f = get_sq_triangle(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2]);
									double sq_triangle_s = get_sq_triangle(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5]);

									// проверка для первого треугольника трех точек из второго. true - точка внутри, false - точка снаружи. i1, j1, k1
									bool i1_in_f = check_point(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], sq_triangle_f);
									bool j1_in_f = check_point(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], sq_triangle_f);
									bool k1_in_f = check_point(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][5], finally_ar[1][5], sq_triangle_f);

									bool i_in_s = check_point(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], sq_triangle_s);
									bool j_in_s = check_point(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], sq_triangle_s);
									bool k_in_s = check_point(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][2], finally_ar[1][2], sq_triangle_s);

									double points[2][6]; // массив пересечений 
									counter = 0;

									//	i СНАРУЖИ
									if (!i_in_s && !i1_in_f && !j1_in_f)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i_in_s && !i1_in_f && j1_in_f)
									{
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i_in_s && !j1_in_f && i1_in_f)
									{
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i_in_s && i1_in_f && j1_in_f)
									{
										// закинули i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// закинули j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
									}

									// i ВНУТРИ
									if (i_in_s && !j_in_s && !k_in_s)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (i_in_s && j_in_s && !k_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (i_in_s && k_in_s && !j_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (i_in_s && j_in_s && k_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

									}

									// j снаружи 
									if (!j_in_s && !j1_in_f && !k1_in_f)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j_in_s && !k1_in_f && j1_in_f)
									{
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j_in_s && !j1_in_f && k1_in_f)
									{
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j_in_s && j1_in_f && k1_in_f)
									{
										// закинули j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// закинули k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }
									}

									// j внутри
									if (j_in_s && !i_in_s && !k_in_s)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (j_in_s && i_in_s && !k_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (j_in_s && !i_in_s && k_in_s)
									{
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (j_in_s && i_in_s && k_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

									}

									// k снаружи
									if (!k_in_s && !i1_in_f && !k1_in_f)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!k_in_s && i1_in_f && !k1_in_f)
									{
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!k_in_s && !i1_in_f && k1_in_f)
									{
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!k_in_s && i1_in_f && k1_in_f)
									{
										// закинули i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// закинули k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }
									}

									// k внутри
									if (k_in_s && !i_in_s && !j_in_s)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (k_in_s && i_in_s && !j_in_s)
									{
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }
										// добавили i
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (k_in_s && !i_in_s && j_in_s)
									{
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (k_in_s && i_in_s && j_in_s)
									{
										// добавили i 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

									}

									// i1 снаружи 
									if (!i1_in_f && !i_in_s && !k_in_s)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i1_in_f && i_in_s && !k_in_s)
									{
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i1_in_f && !i_in_s && k_in_s)
									{
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!i1_in_f && i_in_s && k_in_s)
									{
										// закинули i
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// закинули k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }
									}

									// i1 внутри
									if (i1_in_f && !j1_in_f && !k1_in_f)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (i1_in_f && j1_in_f && !k1_in_f)
									{
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (i1_in_f && !j1_in_f && k1_in_f)
									{
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (i1_in_f && j1_in_f && k1_in_f)
									{
										// добавили i1 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

									}

									// j1 снаружи 
									if (!j1_in_f && !i_in_s && !j_in_s)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j1_in_f && i_in_s && !j_in_s)
									{
										// добавили i
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j1_in_f && !i_in_s && j_in_s)
									{
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!j1_in_f && i_in_s && j_in_s)
									{
										// закинули i
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][0] && points[1][v] == finally_ar[1][0]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][0]; points[1][counter] = finally_ar[1][0]; counter++; }
										// закинули j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
									}

									// j1 внутри 
									if (j1_in_f && !i1_in_f && !k1_in_f)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (j1_in_f && i1_in_f && !k1_in_f)
									{
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }

										}
									}
									else if (j1_in_f && !i1_in_f && k1_in_f)
									{
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (j1_in_f && i1_in_f && k1_in_f)
									{
										// добавили i1 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

									}

									// k1 снаружи
									if (!k1_in_f && !j_in_s && !k_in_s)
									{
										flag = 0;
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!k1_in_f && j_in_s && !k_in_s)
									{
										// добавили j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										/*if (get_intersection(finally_ar[0][5], finally_ar[1][5], finally_ar[0][4], finally_ar[1][4], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][5], finally_ar[1][5], finally_ar[0][4], finally_ar[1][4], finally_ar[0][3], finally_ar[1][3], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}*/
									}
									else if (!k1_in_f && !j_in_s && k_in_s)
									{
										// добавили k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }

										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][1], finally_ar[1][1], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (!k1_in_f && j_in_s && k_in_s)
									{
										// закинули j
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][1] && points[1][v] == finally_ar[1][1]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][1]; points[1][counter] = finally_ar[1][1]; counter++; }
										// закинули k
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][2] && points[1][v] == finally_ar[1][2]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][2]; points[1][counter] = finally_ar[1][2]; counter++; }
									}

									// k1 внутри 
									if (k1_in_f && !i1_in_f && !j1_in_f)
									{
										// проверили, есть ли i в массиве 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

										// находим точку пересечения для случая 1
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (k1_in_f && i1_in_f && !j1_in_f)
									{
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }
										// добавили i1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][4], finally_ar[1][4], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][1], finally_ar[1][1], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}

									}
									else if (k1_in_f && !i1_in_f && j1_in_f)
									{
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }

										// добавили пересечение 
										double ix = 0, iy = 0;
										if (get_intersection(finally_ar[0][3], finally_ar[1][3], finally_ar[0][5], finally_ar[1][5], finally_ar[0][0], finally_ar[1][0], finally_ar[0][2], finally_ar[1][2], ix, iy))
										{
											flag = 0;
											for (int v = 0; v < counter; v++)
											{
												if (points[0][v] == ix && points[1][v] == iy) { flag = 1; break; }
											}
											if (!flag) { points[0][counter] = ix; points[1][counter] = iy; counter++; }
										}
									}
									else if (j1_in_f && i1_in_f && k1_in_f)
									{
										// добавили i1 
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][3] && points[1][v] == finally_ar[1][3]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][3]; points[1][counter] = finally_ar[1][3]; counter++; }
										// добавили j1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][4] && points[1][v] == finally_ar[1][4]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][4]; points[1][counter] = finally_ar[1][4]; counter++; }
										// добавили k1
										flag = 0;
										for (int v = 0; v < counter; v++)
										{
											if (points[0][v] == finally_ar[0][5] && points[1][v] == finally_ar[1][5]) { flag = 1; break; }
										}
										if (!flag) { points[0][counter] = finally_ar[0][5]; points[1][counter] = finally_ar[1][5]; counter++; }

									}


									std::cout << "Inter points: ";
									for (int i = 0; i < counter; i++)
									{
										std::cout << '(' << points[0][i] << "; " << points[1][i] << ')' << ' ';
									}

									double sq_figure = 0.0;
									if (counter == 3) // -- треугольник
									{
										sq_figure = get_sq_triangle(points[0][0], points[1][0],
											points[0][1], points[1][1],
											points[0][2], points[1][2]);
									}

									if (counter == 4) // -- четырехугольник 
									{
										sq_figure = get_sq_square(points[0][0], points[1][0],
											points[0][1], points[1][1],
											points[0][2], points[1][2],
											points[0][3], points[1][3]);
									}
									if (counter == 5) // -- пятиугольник
									{
										sq_figure = get_sq_pent(points[0][0], points[1][0],
											points[0][1], points[1][1],
											points[0][2], points[1][2],
											points[0][3], points[1][3],
											points[0][4], points[1][4]);
									}
									if (counter == 6) // --шестиугольник
									{
										sq_figure = get_sq_hex(points[0][0], points[1][0],
											points[0][1], points[1][1],
											points[0][2], points[1][2],
											points[0][3], points[1][3],
											points[0][4], points[1][4],
											points[0][5], points[1][5]);
									}

									if (max_sq == sq_figure)
									{
										max_sq = sq_figure;
										total_triangles++;

										arr_vertex = append(arr_vertex, &length, &capacity, res_points[0][i], res_points[1][i],
											res_points[0][j], res_points[1][j],
											res_points[0][k], res_points[1][k],
											res_points[0][i1], res_points[1][i1],
											res_points[0][j1], res_points[1][j1],
											res_points[0][k1], res_points[1][k1]);

									}
									if (max_sq < sq_figure)
									{
										length = 0;
										capacity = 6;
										total_triangles = 1;
										memory_delete(arr_vertex);
										arr_vertex = new double* [2];
										for (int i = 0; i < 2; i++) arr_vertex[i] = new double[capacity];
										max_sq = sq_figure;

										arr_vertex[0][length] = res_points[0][i];
										arr_vertex[1][length++] = res_points[1][i];

										arr_vertex[0][length] = res_points[0][j];
										arr_vertex[1][length++] = res_points[1][j];

										arr_vertex[0][length] = res_points[0][k];
										arr_vertex[1][length++] = res_points[1][k];

										arr_vertex[0][length] = res_points[0][i1];
										arr_vertex[1][length++] = res_points[1][i1];

										arr_vertex[0][length] = res_points[0][j1];
										arr_vertex[1][length++] = res_points[1][j1];

										arr_vertex[0][length] = res_points[0][k1];
										arr_vertex[1][length++] = res_points[1][k1];

									}
									std::cout << '\n';
									std::cout << "SQUARE " << sq_figure << "\n";

									std::cout << "\n\n";
									memory_delete(finally_ar);
								}
							}
						}
					}
				}
			}
		}
	}
	std::cout << "MAX SQUARE: " << max_sq << '\n';
	std::cout << "TOTAL TRIANGLES: " << total_triangles << '\n';

	*ptr_res << "Количество пар: " << total_triangles << '\n';
	*ptr_res << "Максимальная площадь: " << max_sq << "\n\n";
	int counter = 1;
	int counter_pare = 1;
	*ptr_res << counter_pare << " пара\n";
	std::cout << '\n' << counter_pare << " пара\n";
	for (int i = 0; i < length; i += 3)
	{
		if (counter % 2 != 0 && i != 0)
		{
			*ptr_res << ++counter_pare << " пара\n"; std::cout << counter_pare << " пара\n";
		}
		*ptr_res << counter << " треугольник:\n";
		*ptr_res << '(' << arr_vertex[0][i] << ';' << arr_vertex[1][i] << ')' << '(' << arr_vertex[0][i + 1] << ';' << arr_vertex[1][i + 1] << ')' << '(' << arr_vertex[0][i + 2] << ';' << arr_vertex[1][i + 2] << ')';
		*ptr_res << '\n';
		std::cout << counter << " треугольник:\n";
		std::cout << '(' << arr_vertex[0][i] << ';' << arr_vertex[1][i] << ')' << '(' << arr_vertex[0][i + 1] << ';' << arr_vertex[1][i + 1] << ')' << '(' << arr_vertex[0][i + 2] << ';' << arr_vertex[1][i + 2] << ')';
		std::cout << '\n';
		//std::cout  << "Вывод против часовой стрелки: \n";

		double first_vertex_x = arr_vertex[0][i];
		double first_vertex_y = arr_vertex[1][i];

		double second_vertex_x = arr_vertex[0][i + 1];
		double second_vertex_y = arr_vertex[1][i + 1];

		double third_vertex_x = arr_vertex[0][i + 2];
		double third_vertex_y = arr_vertex[1][i + 2];

		// используем векторное произведение для вывода против часовой стрелки 
		double cross = get_cross(first_vertex_x, first_vertex_y, second_vertex_x, second_vertex_y, third_vertex_x, third_vertex_y);
		if (cross > 0) // точки идут против часовой стрелки 
		{
			std::cout << "TRIANGLE: ";
			std::cout << '(' << first_vertex_x << ';' << first_vertex_y << ')' << ' ';
			std::cout << '(' << second_vertex_x << ';' << second_vertex_y << ')' << ' ';
			std::cout << '(' << third_vertex_x << ';' << third_vertex_y << ')' << "\n";

			*ptr_res << "TRIANGLE: ";
			*ptr_res << '(' << first_vertex_x << ';' << first_vertex_y << ')' << ' ';
			*ptr_res << '(' << second_vertex_x << ';' << second_vertex_y << ')' << ' ';
			*ptr_res << '(' << third_vertex_x << ';' << third_vertex_y << ')' << "\n";
		}
		if (cross < 0) // точки идут по часовой срелке 
		{
			std::cout << "TRIANGLE: ";
			std::cout << '(' << first_vertex_x << ';' << first_vertex_y << ')' << ' ';
			std::cout << '(' << third_vertex_x << ';' << third_vertex_y << ')' << ' ';
			std::cout << '(' << second_vertex_x << ';' << second_vertex_y << ')' << "\n";

			*ptr_res << "TRIANGLE: ";
			*ptr_res << '(' << first_vertex_x << ';' << first_vertex_y << ')' << ' ';
			*ptr_res << '(' << third_vertex_x << ';' << third_vertex_y << ')' << ' ';
			*ptr_res << '(' << second_vertex_x << ';' << second_vertex_y << ')' << "\n";
		}
		counter++;
		if (counter % 2 - 1 == 0)
		{
			*ptr_res << '\n'; std::cout << '\n';
		}
	}

	memory_delete(arr_vertex);
}

void perm(double** temp_ar, int size)
{
	for (int i = 0; i < size; i++)
	{
		int pos = i;
		for (int j = i + 1; j < size; j++)
		{
			if (temp_ar[0][j] < temp_ar[0][pos])
				pos = j;
		}
		if (pos != i)
		{
			double temp_x = temp_ar[0][i];
			double temp_y = temp_ar[1][i];

			temp_ar[0][i] = temp_ar[0][pos];
			temp_ar[1][i] = temp_ar[1][pos];

			temp_ar[0][pos] = temp_x;
			temp_ar[1][pos] = temp_y;
		}
	}
}

bool get_intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double& ix, double& iy)
{
	const double eps = 1e-9;
	double A1 = y2 - y1;
	double B1 = x1 - x2;
	double C1 = A1 * x1 + B1 * y1;

	double A2 = y4 - y3;
	double B2 = x3 - x4;
	double C2 = A2 * x3 + B2 * y3;

	double det = A1 * B2 - A2 * B1;

	// Если детерминант равен 0, прямые параллельны и не пересекаются
	if (fabs(det) < eps) return false;

	ix = (C1 * B2 - C2 * B1) / det;
	iy = (A1 * C2 - A2 * C1) / det;

	// проверка на отрезок 
	if (ix < min(x1, x2) - eps || ix > max(x1, x2) + eps) return false;
	if (iy < min(y1, y2) - eps || iy > max(y1, y2) + eps) return false;

	// Проверка: лежит ли точка пересечения внутри второго отрезка
	if (ix < min(x3, x4) - eps || ix > max(x3, x4) + eps) return false;
	if (iy < min(y3, y4) - eps || iy > max(y3, y4) + eps) return false;

	return true;
}
double min(double a, double b)
{
	return (a < b) ? a : b;
}

double max(double a, double b)
{
	return (a > b) ? a : b;
}

bool check_triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double ij, ik, jk;
	ij = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	ik = sqrtf((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1));
	jk = sqrtf((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
	if ((ij + ik) > jk && (ij + jk) > ik && (ik + jk) > ij)
		return true;
	return false;
}

double get_sq_triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double side_1 = get_length(x1, y1, x2, y2);
	double side_2 = get_length(x1, y1, x3, y3);
	double side_3 = get_length(x2, y2, x3, y3);
	double p = (side_1 + side_2 + side_3) / 2;

	return sqrtf(p * (p - side_1) * (p - side_2) * (p - side_3));
}

double get_sq_square(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
	double side_1 = get_length(x1, y1, x2, y2);
	double side_2 = get_length(x2, y2, x3, y3);
	double side_3 = get_length(x3, y3, x4, y4);
	double side_4 = get_length(x4, y4, x1, y1);
	double p = (side_1 + side_2 + side_3 + side_4) / 2;

	return sqrtf((p - side_1) * (p - side_2) * (p - side_3) * (p - side_4));
}

double get_sq_pent(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5)
{
	double sq = 0.5 * abs(x1 * y2 + x2 * y3 + x3 * y4 + x4 * y5 + x5 * y1 - x2 * y1 - x3 * y2 - x4 * y3 - x5 * y4 - x1 * y5);
	return sq;
}

double get_sq_hex(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5, double x6, double y6)
{
	double sq = 0.5 * abs(x1 * y2 + x2 * y3 + x3 * y4 + x4 * y5 + x5 * y6 + x6 * y1 - y1 * x2 - y2 * x3 - y3 * x4 - y4 * x5 - y5 * x6 - y6 * x1);
	return sq;
}

double get_length(double x1, double y1, double x2, double y2)
{
	return std::hypotf(x1 - x2, y1 - y2);
}

bool check_point(double x1, double y1, double x2, double y2, double x3, double y3, double x, double y, double sq)
{
	// проверка точки с координатами x, y
	double first_tr = get_sq_triangle(x1, y1, x2, y2, x, y);
	double second_tr = get_sq_triangle(x2, y2, x3, y3, x, y);
	double third_tr = get_sq_triangle(x1, y1, x3, y3, x, y);

	double eps = 1e-2;
	double sum_sq = first_tr + second_tr + third_tr;
	if (abs(sum_sq - sq) < eps)
		return true;
	return false;
}

int get_quiantity_inter_point(double a, double b, double c, double d, double e, double f, bool res1, bool res2, bool res3, bool res4, bool res5, bool res6)
{
	int counter = 0;
	if (a != 1e-3 && res1) counter++;
	if (b != 1e-3 && res2) counter++;
	if (c != 1e-3 && res3) counter++;
	if (d != 1e-3 && res4) counter++;
	if (e != 1e-3 && res5) counter++;
	if (f != 1e-3 && res6) counter++;

	return counter;
}


double** append(double** arr_vertex, int* length, int* capacity, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double x5, double y5, double x6, double y6)
{
	if (*length >= *capacity)
	{
		(*capacity) += 6;
		double** n_arr = new double* [2];
		for (int v = 0; v < 2; v++) n_arr[v] = new double[*capacity];
		// старые значения 
		for (int v = 0; v < *length; v++)
		{
			n_arr[0][v] = arr_vertex[0][v];
			n_arr[1][v] = arr_vertex[1][v];
		}

		memory_delete(arr_vertex);
		arr_vertex = n_arr;
	}
	arr_vertex[0][*length] = x1;
	arr_vertex[1][(*length)++] = y1;

	arr_vertex[0][*length] = x2;
	arr_vertex[1][(*length)++] = y2;

	arr_vertex[0][*length] = x3;
	arr_vertex[1][(*length)++] = y3;

	arr_vertex[0][*length] = x4;
	arr_vertex[1][(*length)++] = y4;

	arr_vertex[0][*length] = x5;
	arr_vertex[1][(*length)++] = y5;

	arr_vertex[0][*length] = x6;
	arr_vertex[1][(*length)++] = y6;

	return arr_vertex;
}

double get_cross(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
}

void memory_delete(double** arr)
{

	for (int i = 0; i < 2; i++)
	{
		delete[] arr[i];
	}

	delete[] arr;
}


