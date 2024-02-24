#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <execution>
#include <algorithm>
#include <random>



void print_cores() {
	std::cout << "���������� ���������� ����: " << std::thread::hardware_concurrency() << std::endl;
}
	
//������� ��� ����������� �����������
class table {
private:
	std::string** ptr;//��������� �� ������� �����������
	int cols = 5;//�������
	int rows = 6;//������
	std::vector<std::pair<int, int>> ranges;//������ ������� ����������� ��������

	//���������� ������� �������
	int what_coloumn(int a) {
		int coloumn;
		switch (a)
		{
		case 1000:
			coloumn = 1; break;
		case 10000:
			coloumn = 2; break;
		case 100000:
			coloumn = 3; break;
		case 1000000:
			coloumn = 4; break;
		default:
			break;
		}
		return coloumn;
	}
	//���������� ������ �������
	int what_row(int a) {
		int row;
		switch (a)
		{
		case 1:
			row = 1; break;
		case 2:
			row = 2; break;
		case 4:
			row = 3; break;
		case 8:
			row = 4; break;
		case 16:
			row = 5; break;
		default:
			break;
		}
		return row;

	}

	//��������� ������ ��������� ��� ������������� �� �������
	void fill_ranges(int number_of_threads, int begin, int end) {
		//������� ��� ������ ������
		int edge = end / number_of_threads;
		int first_edge = edge;
		//������� �������, ���� �� ��������� �����

		//������ ������ (������) �������� ��������� +1 � ��������� �������, � ������ ������ � ������ ������� ��������� +1
		//����� �������������� ������ ������� ��� ������� ���-�� ��������� �� ���������� �������
		for (int i = 0; i < number_of_threads; i++) {
			ranges.push_back(std::make_pair(begin, edge));
			//std::cout << i + 1 << " ����. ����� �� " << ranges[i].first << " �� " << ranges[i].second << std::endl;
			if (i % 2 == 0) {
				begin++;
			}
			else {
				edge++;
			}
			begin += first_edge;
			//����� �������� ���������� ���������, �� ������������� �������� ���������� � ��������� ������� ������ �������
			if (i == number_of_threads - 2) {
				edge = end;
			}
			else
			{
				edge += first_edge;
			}
		}
	}
public:
	//�����������
	table() {
		ptr = new std::string* [rows];
		for (int i = 0; i < rows; i++) {
			ptr[i] = new std::string[cols];
		}
		ptr[0][0] = "";
		ptr[0][1] = "1000";
		for (int i = 2; i < cols; i++) {
			ptr[0][i] = ptr[0][i-1] + "0";
		}
		int mnozh = 1;
		for (int i = 1; i < rows; i++) {
			if (i > 1) {
				mnozh *= 2;			}
			ptr[i][0] = std::to_string(mnozh) + " �������";
			
		}
	}

	//������ �������
	void print_table() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				std::cout << ptr[i][j] << "\t";
				if (i == 0) { std::cout << "\t"; }
			}
			std::cout << std::endl;
		}
	}

private:
	//���������� ��������� � �������
	void remember(std::string str, int a, int b) {
		ptr[a][b] = str;
	}
	//��������� �������
	void summ_vec(std::vector<int>& a, std::vector<int>& b, int begin, int end) {
		using namespace std::chrono_literals;
		//std::this_thread::sleep_for(100ms);
		for (int i = begin; i < end; i++) {
			a[i] + b[i];
		}
	}

public:
	void delete_table() {
		for (int i = 0; i < rows; i++) {
			delete[] ptr[i];
		}
		delete[] ptr;
	}
	//��������� ����� ���������� �������� �������� � ���������� ������
	void calc_by_number_of_threads(std::vector<int>& a, std::vector<int>& b, int number_of_threads, int begin, int end) {
		
		//����������� ������� � ������ ������� -- ���� ����� ������
		int coloumn = what_coloumn(end);
		int row = what_row(number_of_threads);

		//���� ���� �����
		if (row == 1) {
			auto start = std::chrono::steady_clock::now();
			summ_vec(a, b, begin, end);
			auto finish = std::chrono::steady_clock::now();
			std::chrono::duration<double> dur = finish - start;
			remember(std::format("{:.7f}", dur.count()), row, coloumn);
		}
		//���� ������ ������ ������
		else {
			
			//������ �������
			std::vector<std::thread> vec_thread;

			//��������� ������� ��� ���������� �� �������
			fill_ranges(number_of_threads, begin, end);

			//��������� ������ � ������� ����� �������� �������� � ������ ������, ������� �����
			auto start = std::chrono::steady_clock::now();
			for (int i = 0; i < number_of_threads; i++) {
				vec_thread.push_back(std::thread(&table::summ_vec, *this, std::ref(a), std::ref(b), std::ref(ranges[i].first), std::ref(ranges[i].second)));
			}
			//�������� ����� �����
			auto finish = std::chrono::steady_clock::now();
			std::chrono::duration<double> dur = finish - start;
			//���������� ����� � �������
			remember(std::format("{:.7f}", dur.count()), row, coloumn);
			
			for (auto& el : vec_thread) {
				el.join();
			}
		}

	}

	//~table() {
	//	delete_table();
	//}
};



int main() {
	setlocale(LC_ALL, "rus");
	//�� 1 000
	std::vector<int> A_1000(1000), B_1000(1000);
	//�� 10 000
	std::vector<int> A_ten_s(10000), B_ten_s(10000);
	//�� 100 000
	std::vector<int> A_hundred_s(100000), B_hundred_s(100000);
	//�� 1 000 000
	std::vector<int> A_million(1000000), B_million(1000000);

	////������� ����� ����������
	//auto start = std::chrono::steady_clock::now();
	////�������
	//auto finish = std::chrono::steady_clock::now();
	//std::chrono::duration<double> dur = finish - start;
	//std::string a1 = std::format("{:.7f}", dur.count());


	//�������� ����
	std::thread th1(print_cores);
	table T;

	//����������� ����� �������� �������� � ����������� �� ������������ ������� (i = ���������� �������)
	//1000
	for (int i = 1; i < 17; i *= 2) {
		T.calc_by_number_of_threads(A_1000, B_1000, i, 0, 1000);
	}
	//10 000
	for (int i = 1; i < 17; i *= 2) {		
		T.calc_by_number_of_threads(A_ten_s, B_ten_s, i, 0, 10000);
	}
	 //100 000
	for (int i = 1; i < 17; i *= 2) {		
		T.calc_by_number_of_threads(A_hundred_s, B_hundred_s, i, 0, 100000);
	}
	//1 000 000
	for (int i = 1; i < 17; i *= 2) {
		T.calc_by_number_of_threads(A_million, B_million, i, 0, 1000000);
	}
	

	T.print_table();
	T.delete_table();

	th1.join();

	return 0;
}