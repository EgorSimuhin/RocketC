#include <iostream>
#include "EphemerisRelease.hpp" //подключаем библиотеку для расчета эфемерид
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>
double perevod_to_jd(tm* initialTime)
{
        int year = initialTime->tm_year + 1900;
        int month = initialTime->tm_mon + 1;
        int day = initialTime->tm_mday;
        int hour = initialTime->tm_hour;
        int minute = initialTime->tm_min;
        int second = initialTime->tm_sec;

        int a = (14 - month)/12;
        int y = year + 4800 - a;
        int m = month + 12*a - 3;

        int JDN = day + (153*m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;

        double JD = JDN + (hour - 12)/24 + month/1440 + second/86400;

        return JD;
}

void planets_cond_update(std::vector<double*>& planets_cond, tm* initialTime_, time_t T, std::string filePath)
{
	initialTime_ = std::localtime(&T);//получение нового времени в григорианком календаре
	double JED = perevod_to_jd(initialTime_);   

        dph::EphemerisRelease de405(filePath);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::MERCURY, dph::Body::SSBARY, JED, planets_cond[0]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::VENUS, dph::Body::SSBARY, JED, planets_cond[1]);

	de405.calculateBody(dph::Calculate::POSITION, dph::Body::EARTH, dph::Body::SSBARY, JED, planets_cond[2]);

	de405.calculateBody(dph::Calculate::POSITION, dph::Body::MARS, dph::Body::SSBARY, JED, planets_cond[3]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::JUPITER, dph::Body::SSBARY, JED, planets_cond[4]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::SATURN, dph::Body::SSBARY, JED, planets_cond[5]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::URANUS, dph::Body::SSBARY, JED, planets_cond[6]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::NEPTUNE, dph::Body::SSBARY, JED, planets_cond[7]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::PLUTO, dph::Body::SSBARY, JED, planets_cond[8]);

	de405.calculateBody(dph::Calculate::POSITION, dph::Body::MOON, dph::Body::SSBARY, JED, planets_cond[9]);

        de405.calculateBody(dph::Calculate::POSITION, dph::Body::SUN, dph::Body::SSBARY, JED, planets_cond[10]);

}
int main()
{
        int year, month, day, hour, minute, second, increment;
	//Вводим дату и время в начальный момент
	std::cout << "Enter date (YYYY MM DD): ";
        std::cin >> year >> month >> day;
        std::cout << "Enter time (HH MM SS): ";
        std::cin >> hour >> minute >> second;
       
        // Создание начальной даты и времени
        std::tm* initialTime = new std::tm; //структура для работы с временем
        initialTime->tm_year = year - 1900; 
        initialTime->tm_mon = month - 1;
        initialTime->tm_mday = day;
        initialTime->tm_hour = hour;
        initialTime->tm_min = minute;
        initialTime->tm_sec = second;

        std::time_t currentTimeT = std::mktime(initialTime);//перевод даты в номе дня в григорианском календаре

        double JED = perevod_to_jd(initialTime);//функция перевода в номер юлианского дня

        std::string filePath("lnxp1600p2200.405");//считывание файла, хранящего эфемериды планет

        const int parametres = 3;
        const int planet_count = 11;
        const double G = 6.67e-11;
        std::vector<double*> planets_cond_0;//вектор, хранящий состояние 11 небесных тел
	for (int i = 0; i < 11; i++)
       {
                double* arr = new double[3];//по три координаты
                planets_cond_0.push_back(arr);
       }

        planets_cond_update(planets_cond_0, initialTime, currentTimeT, filePath);//заполнение вектора первым значением

        unsigned long long int delta_t, final_time;//каждые сколько секунд производить расчет и сколько всего времени рассчитвыать в секундах
        delta_t = 3600*5*75;
	final_time = 31536000*50;

        std::vector<std::vector<double*>> solutions;//вектор, хранящий положения всех планет в каждый момент времени
	std::vector<int> times;//вектор, содержащий значения времени, прошедшего с начала отсчета к рассматориваемому моменту
	
        times.push_back(0);
	solutions.push_back(planets_cond_0);
	int current_t = 0;
	std::time_t T = current_t + currentTimeT;
	while(times.back() < final_time)
	{
		T += delta_t;
		current_t += delta_t;
		planets_cond_0.clear();
		for (int i = 0; i < 11; i++)
      		{
                	double* arr = new double[3];
                	planets_cond_0.push_back(arr);
	        }
		planets_cond_update(planets_cond_0, initialTime, T, filePath);
		times.push_back(current_t);
		solutions.push_back(planets_cond_0);
	}
	std::cout << times.back() << std::endl;
	for(int i = 0; i < solutions.size(); i++)//вывод трех координат 10-ой планеты каждые delta_t секунд с начала отсчета(нужно менять чтобы получить, что хочешь)
	{
		std::cout << i << " " << solutions[i][10][0] << " " << solutions[i][10][1] << " " << solutions[i][10][2] << std::endl;
	}

	//очистка памяти
        for(int i = 0; i < planet_count; i++)
        {
                delete [] planets_cond_0[i];
        }
	delete  initialTime;
        return 0;
}

