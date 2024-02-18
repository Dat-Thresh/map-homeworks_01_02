#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <execution>
#include <algorithm>
#include <random>



void print_cores() {
	std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl;
}
	
//таблица для запоминания результатов
class table {
private:
	std::string** ptr;
	int cols = 5;//столбцы
	int rows = 6;//строки

	//определяет колонку таблицы
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
	//определяет строку таблицы
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
public:
	//конструктор
	table() {
		ptr = new std::string * [rows];
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
			ptr[i][0] = std::to_string(mnozh) + " потоков";
			
		}
	}

	//печать таблицы
	void print_table() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				std::cout << ptr[i][j] << "\t";
				if (i == 0) { std::cout << "\t"; }
			}
			std::cout << std::endl;
		}
	}

	//запоминает результат в таблицу
	void remember(std::string str, int a, int b) {
		ptr[a][b] = str;
	}
	//суммирует вектора
	void summ_vec(std::vector<int>& a, std::vector<int>& b, int begin, int end) {
		using namespace std::chrono_literals;
		//std::this_thread::sleep_for(100ms);
		for (int i = begin; i < end; i++) {
			a[i] + b[i];
		}
	}

	//вычисляет время выполнения сложения массивов и запоолняет балицу
	void calc_by_number_of_threads(std::vector<int>& a, std::vector<int>& b, int number_of_threads, int begin, int end) {
		
		//определеяем колонку и строку таблицы -- куда будем писать
		int coloumn = what_coloumn(end);
		int row = what_row(number_of_threads);

		//если один поток
		if (row == 1) {
			auto start = std::chrono::steady_clock::now();
			summ_vec(a, b, begin, end);
			auto finish = std::chrono::steady_clock::now();
			std::chrono::duration<double> dur = finish - start;
			remember(std::format("{:.7f}", dur.count()), row, coloumn);
		}
		//если больше одного потока
		else {
			//граница для одного потока
			int edge = end / number_of_threads;

			//вектор потоков
			std::vector<std::thread> vec_thread;
			//запускаем потоки и считаем суммы элеметов векторов в рамках границ, засекая время
			auto start = std::chrono::steady_clock::now();
			for (int i = 0; i < number_of_threads; i++) {
				vec_thread.push_back(std::thread(summ_vec, std::ref(a), std::ref(b), std::ref(begin), std::ref(edge)));
				//смещаем границы, пока не достигнем конца
				if (edge < end) {
					begin += edge;
					edge += edge;
				}
			}
			//засекаем время конца
			auto finish = std::chrono::steady_clock::now();
			std::chrono::duration<double> dur = finish - start;
			//записываем время в таблицу
			remember(std::format("{:.7f}", dur.count()), row, coloumn);
			
			for (auto& el : vec_thread) {
				el.join();
			}
		}

	}

	~table() {
		for (int i = 0; i < rows; i++) {
			delete[] ptr[i];
		}
		delete[] ptr;
	}
};

//суммирует элементы указанного промежутка векторов
//void summ_vec(std::vector<int>& a, std::vector<int>& b, int begin, int end) {
//	using namespace std::chrono_literals;
//	std::this_thread::sleep_for(100ms);	
//	for (int i = begin; i < end; i++) {
//		a[i] + b[i];
//	}
//}


int main() {
	setlocale(LC_ALL, "rus");
	//на 1 000
	std::vector<int> A_1000(1000), B_1000(1000);
	//на 10 000
	std::vector<int> A_ten_s(10000), B_ten_s(10000);
	//на 100 000
	std::vector<int> A_hundred_s(100000), B_hundred_s(100000);
	//на 1 000 000
	std::vector<int> A_million(1000000), B_million(1000000);

	////считает время выполнения
	//auto start = std::chrono::steady_clock::now();
	////процесс
	//auto finish = std::chrono::steady_clock::now();
	//std::chrono::duration<double> dur = finish - start;
	//std::string a1 = std::format("{:.7f}", dur.count());


	//печатаем ядра
	std::thread th1(print_cores);
	table T;
	//высчитываем время сложения массивов в зависимости от используемых потоков (i = количеству потоков)
	for (int i = 1; i < 17; i *= 2) {
		T.calc_by_number_of_threads(A_1000, B_1000, i, 0, 1000);
		T.calc_by_number_of_threads(A_ten_s, B_ten_s, i, 0, 10000);
		T.calc_by_number_of_threads(A_hundred_s, B_hundred_s, i, 0, 100000);
		T.calc_by_number_of_threads(A_million, B_million, i, 0, 1000000);
	}

	
	

	T.print_table();
	
	/*std::thread th2(summ_vec, std::ref(A_1000), std::ref(B_1000), 0, 1000);
	std::thread th3(summ_vec, std::ref(A_ten_s), std::ref(B_ten_s), 0, 10000);

	th2.join();
	th3.join();*/
	th1.join();

	return 0;
}