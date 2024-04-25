#pragma once
#include "eigen/Eigen/Dense"
#include <vector>
#include <array>

using Planets_State = std::array<Eigen::Vector3d, 11>;

const std::array<double, 11> mu =
{
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
				132'712'440'018e9
};

struct State
{
    Eigen::Vector<double, 6> cond;
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
						    )
{		
		std::array<Eigen::Vector3d, 11> result;
		const std::array<unsigned int, 11> bodies =
	       	{
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

		for (unsigned int i = 0; i < result.size(); ++i)
		{
			de405.calculateBody(dph::Calculate::POSITION, bodies[i], dph::Body::SSBARY, JED, result[i].data());
			result[i] *= 1e3;
		}

		return result;	
}

Eigen::Vector<double, 3> Acceleration(
			             const Eigen::Vector3d &relativePosition,
				     const std::array<Eigen::Vector3d, 11> &planet_relativePosition
				     )
{

	Eigen::Vector3d acceleration = Eigen::Vector3d::Zero();	
	for (int i = 0; i < 11; ++i)
	{
		const Eigen::Vector3d delta = planet_relativePosition[i] - relativePosition;
		const double dist = delta.norm();
		acceleration += delta * (mu[i] / dist / dist / dist);
    	}

	return acceleration;
}

Eigen::Vector<double, 6> derivative_cond(
				        const Eigen::Vector<double, 6> &cond,
				        const double time,
				        const double JED,
				        const dph::EphemerisRelease& de405
				        )
{

	const Eigen::Vector3d position = cond.segment<3>(0);
	const Eigen::Vector3d velocity = cond.segment<3>(3);

	const Eigen::Vector3d acceleration = Acceleration(position, planets_state_update(JED + time / 86400., de405));
    

	Eigen::Vector<double, 6> result;
    
	result.segment<3>(0) = velocity;
	result.segment<3>(3) = acceleration;
        

	return result;
}


State rungeKuttaStep(
              	    const State &state,
		    const double step,
		    const double JED,
		    const dph::EphemerisRelease& de405
		    )
{

	const Eigen::Vector<double, 6> k1 = derivative_cond(state.cond,                 state.time,             JED, de405);
	const Eigen::Vector<double, 6> k2 = derivative_cond(state.cond + step * k1 / 2, state.time + step / 2,  JED, de405);
	const Eigen::Vector<double, 6> k3 = derivative_cond(state.cond + step * k2 / 2, state.time + step / 2,  JED, de405);
	const Eigen::Vector<double, 6> k4 = derivative_cond(state.cond + step * k3,     state.time + step,      JED, de405);

	const Eigen::Vector<double, 6> updated_cond = state.cond + step / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
	const double updatedTime = state.time + step;

	return {updated_cond, updatedTime};
}

State_full rungeKutta(
		     const State &initialState,
		     const double step,
		     const double endTime,
		     const double JED,
		     const dph::EphemerisRelease& de405
		     )
{
	State_full sol;
	sol.rocket_solution.push_back(initialState);
	sol.planets_solution.push_back(planets_state_update(JED + sol.rocket_solution.back().time / 86400., de405));	

	while (sol.rocket_solution.back().time < endTime)
       	{
		sol.rocket_solution.push_back(rungeKuttaStep(sol.rocket_solution.back(), step, JED, de405));
		sol.planets_solution.push_back(planets_state_update(JED + sol.rocket_solution.back().time / 86400., de405));
    	}

	return sol;
}
