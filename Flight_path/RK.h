//#ifndef RK_RK_H
//#define RK_RK_H

#pragma once
#include "eigen/Eigen/Dense"
#include <vector>
#include <array>

using Planets_State = std::array<Eigen::Vector3d, 11>;

const std::array<double, 11> mu1 = {
	22'032e9,
	324'859e9,
	398'600.4415e9,
	42'828e9,
	126'686'534e9,
	37'931'187e9,
	5'793'939e9,
	6'836'529e9,
	871.000000e9,
	4'902.800066e9,
	132'712'440'018e9};
const std::array<double, 11> mu = {
        22'032e9,
        324'859e9,
        398'600.4415e9,
        42'828e9,
        126'686'534e9,
        37'931'187e9,
        5'793'939e9,
        6'836'529e9,
       871.000000e9,
       4'902.800066e9,
       132'712'440'018e9};

struct State {
    Eigen::Vector<double, 6> RV;
    double time;
};

struct State_full
{
	std::vector<State> rocket_solution;
	std::vector<Planets_State> planets_solution;
};


std::array<Eigen::Vector3d, 11> planets_state_update(
					        	const double JED,
							const dph::EphemerisRelease& de405
							){		
		std::array<Eigen::Vector3d, 11> result;
		const std::array<unsigned int, 11> bodies = {
			dph::Body::MERCURY,
			dph::Body::VENUS,
			dph::Body::EARTH,
			dph::Body::MARS,
			dph::Body::JUPITER,
			dph::Body::SATURN,
			dph::Body::URANUS,
			dph::Body::NEPTUNE,
			dph::Body::PLUTO,
			dph::Body::MOON,
			dph::Body::SUN
		};
		for (unsigned int i = 0; i < result.size(); ++i){
			de405.calculateBody(dph::Calculate::POSITION, bodies[i], dph::Body::SSBARY, JED, result[i].data());
			result[i] *= 1e3;
		}
		return result;	
}

[[nodiscard]] Eigen::Vector<double, 3> centralFieldAcceleration(
								const Eigen::Vector3d &relativePosition,
								const std::array<Eigen::Vector3d, 11> &planet_relativePosition
								) {

    Eigen::Vector3d acceleration = Eigen::Vector3d::Zero();	
    for (int i = 0; i < 11; ++i)
    {
	const Eigen::Vector3d delta = planet_relativePosition[i] - relativePosition;
	// std::cout << planet_relativePosition[i] << std::endl << std::endl;
	const double dist = delta.norm();
//	std::cout <<  (delta * (mu[i] / dist / dist / dist)).norm() << std::endl;
    	acceleration += delta * (mu[i] / dist / dist / dist);
    }
    //std::cout << acceleration << std::endl << std::endl;
    return acceleration;
}

[[nodiscard]] Eigen::Vector<double, 6> centralFieldRHS(
							const Eigen::Vector<double, 6> &RV,
							const double time,
							const double JED,
							const dph::EphemerisRelease& de405
							) {

    const Eigen::Vector3d position = RV.segment<3>(0);
    const Eigen::Vector3d velocity = RV.segment<3>(3);

    const Eigen::Vector3d acceleration = centralFieldAcceleration(position, planets_state_update(JED + time / 86400., de405));
    

    Eigen::Vector<double, 6> result;
    
    result.segment<3>(0) = velocity;
    result.segment<3>(3) = acceleration;
        

    return result;
}


[[nodiscard]] State rungeKuttaStep(
					const State &state,
					const double step,
					const double JED,
					const dph::EphemerisRelease& de405
					) {

    const Eigen::Vector<double, 6> k1 = centralFieldRHS(state.RV,                 state.time,             JED, de405);
    const Eigen::Vector<double, 6> k2 = centralFieldRHS(state.RV + step * k1 / 2, state.time + step / 2,  JED, de405);
    const Eigen::Vector<double, 6> k3 = centralFieldRHS(state.RV + step * k2 / 2, state.time + step / 2,  JED, de405);
    const Eigen::Vector<double, 6> k4 = centralFieldRHS(state.RV + step * k3,     state.time + step,      JED, de405);

    const Eigen::Vector<double, 6> updatedRV = state.RV + step / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
    const double updatedTime = state.time + step;

    return {updatedRV, updatedTime};
}

[[nodiscard]] State_full rungeKutta(
				const State &initialState,
				const double step,
				const double endTime,
				const double JED,
				const dph::EphemerisRelease& de405
				) {

    State_full sol;
    sol.rocket_solution.push_back(initialState);
    sol.planets_solution.push_back(planets_state_update(JED + sol.rocket_solution.back().time / 86400., de405));	
    while (sol.rocket_solution.back().time < endTime) {
        sol.rocket_solution.push_back(rungeKuttaStep(sol.rocket_solution.back(), step, JED, de405));
	sol.planets_solution.push_back(planets_state_update(JED + sol.rocket_solution.back().time / 86400., de405));
    }
    return sol;
}

//#endiff
