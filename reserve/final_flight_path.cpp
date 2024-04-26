#include <iostream>
#include "EphemerisRelease.hpp"
#include "RK.h"

const std::array<std::string, 12> key = {
                                        "MERCURY",
                                        "VENUS",
                                        "EARTH",
                                        "MARS",
                                        "JUPITER",
                                        "SATURN",
                                        "URANUS",
                                        "NEPTUNE",
                                        "PLUTO",
                                        "MOON",
                                        "SUN",
                                        "ROCKET"
                                         };

std::vector<glm::vec3> perevod_planets(
					    const int k,
					    const State_full full
					    )
{
	std::vector<glm::vec3> result;
	for (auto i : full.planets_solution)
	{
		glm::vec3 = temp;
		temp.x = i[k][0];
		temp.y = i[k][1];
		temp.z = i[k][2];
		result.push_back(temp);
	}
	return result;
}

std::vector<glm::vec3> perevod_rocket(
					   const State_full full
					   )
{
	std::vector<glm::vec3> result;
	for (auto i : full.rocket_solution)
	{
		glm::vec3 = temp;
                temp.x = i[k][0];
                temp.y = i[k][1];
                temp.z = i[k][2];
                result.push_back(temp);
	}
	return result;
}

std::vector<double> perevod_time(
				const State_full full
				)
{
	std::vector<double> result;
	for (auto i : full.rocket_solution)
	{
		result.push_back(i.time);
	}
	return result;

int main() {

	std::string filePath("../lnxp1600p2200.405");
	dph::EphemerisRelease de405(filePath);
        double JED, step, endTime, x_0, y_0, z_0, v_x0, v_y0, v_z0;
	//const Eigen::Vector<double, 6> cond_0 = {4e11, 0, 0, 0, 9.5e3, 0};
	//const double JED = 2460423.13825;
	
	std::cout << "Введите дату в юлианских днях (ссылка на калькулятор: https://planetcalc.ru/503/)" << std::endl;
	std::cin >> JED;
	std::cout << "Введите начальные условия (x_0, y_0, z_0, v_x0, v_y0, v_z0) в метрах" << std::endl;
	std::cin >> x_0 >> y_0 >> z_0 >> v_x0 >> v_y0 >> v_z0;
	std::cout << "Введите время шага интегрирования в секундах" << std::endl;
	std::cin >> step;
	std::cout << "Введите длительность времени расчета в секундах" << std::endl;
	std::cin >> endTime;

	const Eigen::Vector<double, 6> cond_0 = {x_0, y_0, z_0, v_x0, v_y0, v_z0};
	const double initialTime = 0;
	//const double step = 3600*24;  
   
	const State initialState = {cond_0, initialTime};

	//const double endTime = 3600*24*1500;
 
	const State_full full = rungeKutta(initialState, step, endTime, JED, de405);

	std::map<std::string, std::vector<glm::vec3>> myMap;

	for (int i = 0; i < 11; ++i)
	{
		myMap.emplace(key[i], perevod_planets(i, full));
    	}
   	myMap.emplace(key.back(), perevod_rocket(full));

	const std::vector<double> time = perevod_time(full);	
 
    	for (const auto pair : myMap)
   	{
		std::cout << pair.first << " " << pair.second[0][0] << " " << pair.second[0][1] << " " << pair.second[0][2] <<  std::endl;
    	}

    	for (int i = 0; i < 1500; ++i)
    	{
		std::cout << full.rocket_solution[i].cond[0] << " " << full.rocket_solution[i].cond[1] << " " << full.rocket_solution[i].cond[2] << std::endl;
//      	std::cout << full.planets_solution[i][4][0] << " " << full.planets_solution[i][4][1] << " " << full.planets_solution[i][4][2] << std::endl;	
    	}
}
