#include <iostream>
#include "EphemerisRelease.hpp"
#include "RK.h"

std::vector<Eigen::Vector3d> perevod_planets(const int k, const State_full full)
{
	std::vector<Eigen::Vector3d> result;
	for (auto i : full.planets_solution)
	{
		result.push_back(i[k]);
	}
	return result;
}

std::vector<Eigen::Vector3d> perevod_rocket(const State_full full)
{
	std::vector<Eigen::Vector3d> result;
	for (auto i : full.rocket_solution)
	{
		result.push_back(i.RV.segment<3>(0));
	}
	return result;
}

int main() {

    std::string filePath("lnxp1600p2200.405");
    dph::EphemerisRelease de405(filePath);
        
    const Eigen::Vector<double, 6> RV = {1.46e+11, 1.49e+11, 6.45e+10, 2.5e3, 9.5e3, 0};
    const double JED = 2460423.13825;
    const double initialTime = 0;
    const double step = 3600;
    
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
    
    const State initialState = {RV, initialTime};

    //const double T = 2 * M_PI * rho * std::sqrt(rho / GRAV_PARAMETER);
    const double endTime = 3600*24*5;
 
    const State_full full = rungeKutta(initialState, step, endTime, JED, de405);

    std::map<std::string, std::vector<Eigen::Vector3d>> myMap;
    for (int i = 0; i < 11; ++i)
    {
	myMap.emplace(key[i], perevod_planets(i, full));
    }
    myMap.emplace(key.back(), perevod_rocket(full));		
 //   std::cout << full.rocket_solution.back().RV << std::endl;
    for (const auto pair : myMap)
    {
	    std::cout << pair.first << " " << pair.second[0][0] << " " << pair.second[0][1] << " " << pair.second[0][2] <<  std::endl;
    }
    for (int i = 0; i < 24*5; ++i)
    {
	    std::cout << full.rocket_solution[i].RV[0] << " " << full.rocket_solution[i].RV[1] << " " << full.rocket_solution[i].RV[2] << std::endl;
	      std::cout << full.planets_solution[i][3][0] << " " << full.planets_solution[i][3][1] << " " << full.planets_solution[i][3][2] << std::endl;	
    }
}
