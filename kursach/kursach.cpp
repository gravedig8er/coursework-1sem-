#include <iostream> 
#include <fstream> 

bool get_quantity_n(std::fstream* ptr_f, int* ptr_n);
void zero_fill(double** points, const int* size);
void filling_arrays(std::fstream* ptr_f, double** points, const int* size);
int protocol_arrays(std::fstream* ptr_out, double** points, const int* size);
void transfer_arrays(double** res_points, double** points, const int* total_points, const int* size);
void output_result(std::fstream* ptr_res, double** res_points, const int* total_points);
void memory_delete(double** arr);

int main()
{
	int n, total_points = 0; 
	std::fstream f; // ������ �� ����� --- �������� � filling_arrays
	std::fstream out; // ��������� � ��������
	std::fstream res; // ��������� � ���������
	setlocale(LC_ALL, "Russian");
	if (!get_quantity_n(&f, &n)) // ����������� ���������� ����� ��� ���������� ������� + �������� �����
		return 0;

	// �������� �������. points[0][x]. points[1][y]
	double** points = new double* [2];
	for (int i = 0; i < 2; i++) points[i] = new double[n];

	// �������� ������� ���������� 1e-3
	zero_fill(points, &n);
	// ���������� �������� �� ����� 
	filling_arrays(&f, points, &n); 

	// ������ � �������� ��������� ������
	total_points = protocol_arrays(&out, points, &n);
	if (total_points == 0) return 0;

	// ��������� �������� ��� ������� ������
	double** res_points = new double* [2];
	for (int i = 0; i < 2; i++) res_points[i] = new double[total_points];

	// ������� ������ � ������� ������
	transfer_arrays(res_points, points, &total_points, &n);
	memory_delete(points);

	// ����� ����� � result
	output_result(&res, res_points, &total_points);
	
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
	// �������� ���������� �����, �.� �� ����� ���� ������/������, ��� "�������"
	if (*ptr_n <= 0) return false; 
	while (!ptr_f->eof())
	{
		double temp = -1e5;
		*ptr_f >> temp;
		if (temp != -1e5) // ���� ����� � ������ ����. �� ����� 1 ��� 2 
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
	double needless; 
	double temp; 
	int i = 0; 
	char symbol;
	if (!ptr_f->eof()) *ptr_f >> needless; // ��������� ������, ���������� ���
	ptr_f->get(symbol); // ������� ������� ������ ����� ������� 
	while (!ptr_f->eof())
	{
		ptr_f->get(symbol);
		if ((symbol == ' ' || symbol == '\t') && symbol != '\n') // �������, ��� ������� �������� � � ����� ������� ������ y 
		{
			ptr_f->seekg(-1, std::ios::cur); // ������� ������� ����� 
			*ptr_f >> points[1][i]; // ������� y, ������ ����� ������� ������� ������, ����� ���������� ������ ��������
			while (true)
			{
				char symbol;
				ptr_f->get(symbol);
				if (symbol == '\n') break;
			}
			i++;
			continue;
		}
		else if (symbol != '\n')// � �� ������� -> ���������
		{
			ptr_f->seekg(-1, std::ios::cur); // ������� ������� ����� 
			*ptr_f >> points[0][i];
			while (true)
			{
				ptr_f->get(symbol);
				if (symbol != ' ' && symbol != '\t' && symbol != '\n') // ���� �� �����
				{
					ptr_f->seekg(-1, std::ios::cur);
					*ptr_f >> points[1][i]; // ��������� y 
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
				if (symbol == '\n') // ������ y, ��������� ������ 
				{
					i++;
					break;
				}
			}
		}
		else // ������ ����� ������� ������ 
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
		std::cout << "�������� �� �������� \n";
		return 0;
	}
	*ptr_out << "���������� ��������� �����: " << *size << "\n\n";
	for (int i = 0; i < *size; i++)
	{
		*ptr_out << "����� " << i + 1 << ": \t";
		if (points[0][i] == 1e-3 && points[1][i] != 1e-3)
		{
			*ptr_out << "�� ������� �    " << "y: " << points[1][i] << '\n';
		}
		if (points[0][i] != 1e-3 && points[1][i] == 1e-3)
		{
			*ptr_out << "x: " << points[0][i] << "\t\t" << "�� ������� y\n";
		}
		if (points[0][i] != 1e-3 && points[1][i] != 1e-3)
		{
			*ptr_out << "x: " << points[0][i] << "\t\t" << "y: " << points[1][i] << '\n';
			total_points++;
		}
	}
	*ptr_out << '\n';
	*ptr_out << "����� �����: " << total_points << '\n';
	return total_points;
}

void transfer_arrays(double** res_points, double** points, const int* total_points, const int* size)
{
	int point = 0;
	for (int i = 0; i < *size; i++)
	{
		if (points[0][i] != 1e-3 && points[1][i] != 1e-3)
		{
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
		std::cout << "���� ��� ���������� �� �������� \n";
	}
	*ptr_res << "������� " << *total_points << " �����:\n\n";
	for (int i = 0; i < *total_points; i++)
	{
		*ptr_res << res_points[0][i] << "\t" << res_points[1][i] << '\n';
	}
	*ptr_res << '\n';
}

void memory_delete(double** arr)
{
	for (int i = 0; i < 2; i++)
	{
		delete[] arr[i];
	}
	delete[] arr;
}