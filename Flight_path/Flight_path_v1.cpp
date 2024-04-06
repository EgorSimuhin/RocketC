#include <iostream>
#include "EphemerisRelease.hpp" //подключаем библиотеку для расчета эфемерид
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>

double perevod_to_jd(const std::vector<int>& initialTime)
{
	year = initialTime[0];
        month = initialTime[1];
        day = initialTime[2];
        hour = initialTime[3];
        minute = initialTime[4];
        second = initialTime[5];

	int a = (14 - month)/12;
	int y = year + 4800 - a;
	int m = month + 12*a - 3;

	int JDN = day + (153*m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;

	double JD = JDN + (hour - 12)/24 + month/1440 + second/86400;

	return JD;
}

void planets_cond_update(std::vector<double*>& planets_cond, int T)
{
        std::tm* initialTime_  = std::localtime(&T);

	JED = perevod_to_jd(initialTime);
	//double **planets_cond = new double* [planet_count];
	//std::vector<double*> planets_cond;

	for (int i = 0; i < planet_count; i++)
       {
                double* arr = new double[3];
		planets_cond.push_back(arr);
       }

        dph::EphemerisRelease de405(filePath);
        std::cout << de405.isReady() << std::endl;

        {
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

}

double linalg_norm(std::vector<double>& cond, double* planet_cond)
{
	double dx = cond[0] - planet_cond[0];
	double dy = cond[1] - planet_cond[1];
	double dz = cond[2] - planet_cond[2];

	return sqrt(dx*dx + dy*dy + dz*dz); 
}

std::vector<double> balilistics_right_part(double t, std::vector<double>& cond, double** planets_cond, const std::vector<double>& mu)
{
	std::vector<double> result;
	std::vector<double> position;

	for (int i = 3; i < 6; i++)
	{
		result.push_back(cond[i]);
	}

	for (int i = 0; i < 3; i++)
        {
                position.push_back(cond[i]);
        }

	std::vector<double> distance;

	for (int i = 0; i < 11; i++)
	{
		distance.push_back(linalg_norm(cond, planets_cond[i]));
	}

	double k = 0;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 11; j++)
		{
			k += mu[j]*(planets_cond[j][i] - position[i])/(distance[j]*distance[j]*distance[j]);
		}
		result[3 + i] = k;
		k = 0;
	}

	return result;
}

std::vector<double> lin_komb(const std::vector<double>& cond, double delta_t, const std::vector<double>& k, const int deter)
{
	std::vector<double> result;
	for (int i = 0; i < 6; i++)
	{
		result.push_back(cond[i] + (delta_t / deter) * k[i]);
	}
	return result;
}

std::vector<double> calc_step(double t, std::vector<double>& cond, std::vector<double*>& planets_cond, double delta_t,  const std::vector<double>& mu)
{
	k1 = balistic_right_part(t, cond, planets_cond, delta_t, func);
	k2 = balistic_right_part(t + delta_t/2, lin_komb(cond, delta_t, k1, 2), planets_cond);
	k3 = balistic_right_part(t + delta_t/2, lin_komb(cond, delta_t, k2, 2), planets_cond);
	k4 = balistic_right_part(t + delta_t, lin_komb(cond, delta_t, k3, 1), planets_cond);
	std::vector<double> result;
	for (int i = 0; i < 6; i++)
        {
                result.push_back(cond[i] + (delta / 6) * (k1[i] + 2 * (k2[i] + k3[i]) + k4[i]);
        }
        return result;
}

void solve_ballistics(std::vector<std::vector<double>>& solutions, std::vector<int>& times, std::vector<double>& cond_0, std::vector<double*>&  planets_cond, const  int delta_t, const int final_time, time_t currentTimeT, const std::vector<double>& mu)
{
	//std::vector<std::vector<double>> solutions;
	//std::vector<int> times;
	std::vector<double> current_cond;
	std::vector<double> current_cond_next;
	std::vector<double> current_planets_cond_next;


	current_cond.assign(cond_0.begin(), cond_0.end());
	solutions.push_back(cond_0);
	times.push_back(0);
	int current_t = 0;
	int T = current_t + currentTimeT;

	while(times.back() < final_time)
	{
		current_cond_next = calc_step(current_t, current_cond, planets_cond, delta_t, mu));
		current_cond.assign(current_cond_next.begin(), current_cond_next.end());
		current_cond_next.clear();
		solutions.push_back(current_cond);
		T += delta_t;
		current_t += delta_t;
		times.push_back(current_t);
		planets_cond_update(planets_cond, T);
	}
	return solutions, times;

}

int main()
{
	int year, month, day, hour, minute, second, increment;
    	std::cout << "Enter date (YYYY MM DD): ";
    	std::cin >> year >> month >> day;
    	std::cout << "Enter time (HH MM SS): ";
    	std::cin >> hour >> minute >> second;
    	std::cout << "Enter time increment (in seconds): ";
    	std::cin >> increment;

    	// Создание начальной даты и времени
   	std::tm* initialTime;
        initialTime->tm_year = year;
        initialTime->tm_mon = month;
        initialTime->tm_mday = day;
        initialTime->tm_hour = hour;
        initialTime->tm_min = minute;
        initialTime->tm_sec = second;


	std::time_t currentTimeT = std::mktime(initialTime);
    	
	double x_0, y_0, z_0, vx_0, vy_0, vz_0;
	std::cin >> x_0 >> y_0 >> z_0 >> vx_0 >> vy_0 >> vz_0;
	std::vector<double> cond_0 = {x_0, y_0, z_0, vx_0, vy_0, vz_0}; //начальное состояние
	double JED = perevod_to_jd(initialTime);
	
	std::string filePath("lnxp1600p2200.405");

	const int parametres = 3;
	const int planet_count = 11;
	const double G = 6.67e-11;
	std::vector<double*> planets_cond_0;
       	planets_cond_update(planets_cond_0, currentTimeT);
	std::vector<double> mu = {G*3.302e23, G*4.8685e24, G*5.9736e24,
                            G*6.419e23, G*1.8986e27, G* 5.6846e26,
                            G*8.6832e25, G*1.0243e26, G*1.31e22, G*7.35e22,
                            G*1.9885e30};

	double delta_t, final_time;
	std::cin >> delta_t;
	std::cin >> final_time;

	std::vector<std::vector<double>> solutions;
        std::vector<int> times;

	solve_ballistics(solutions, times, cond_0, planets_cond_0, delta_t, final_time, currentTimeT, mu);
	

	for(int i = 0; i < planet_count; i++)
	{
		delete [] planets_cond[i];
	}	

	return 0;
}
