#include <iostream>
#include "EphemerisRelease.hpp" //подключаем библиотеку для расчета эфемерид
#include <vector>
#include <array>
#include <cmath>

struct Condition
{
	std::array<double, 6> cond;
	double time;
};

template<size_t N>
std::array<double, N> operator+(const std::array<double, N> &arr1, const std::array<double, N> &arr2)
{
	std::array<double, N> result;
	for (size_t i = 0; i < N; ++i)
	{
		result[i] = arr1[i] + arr2[i];
	}
	return result;
}

template<size_t N>
std::array<double, N> operator*(const std::array<double, N> &arr, const double k)
{
        std::array<double, N> result;
        for (size_t i = 0; i < N; ++i)
        {
                result[i] = arr[i]*k;
        }
        return result;
}

double position_Norm(const std::array<double, 3>& cond)
{
	return sqrt(cond[0]*cond[0] + cond[1]*cond[1] + cond[2]*cond[2]);
}

std::array<double, 3> Acceleration(const std::array<double, 3>& cond_pos, const double mu)
{
	std::array<double, 3> acceleration;
	const std::array<double, 3> position = {cond_pos[0], cond_pos[1], cond_pos[2]};
	const double positionNorm = position_Norm(position);
	const double positionNormCube = positionNorm * positionNorm * positionNorm;
	acceleration = position*(-mu*(1.0/positionNormCube));
	
	return acceleration;
}

std::array<double, 6> proizv_cond(const std::array<double, 6>& cond, const double time, const double mu)
{
	std::array<double, 3> position, velocity, acceleration;
	for (int i = 0; i < 3; ++i)
	{
		position[i] = cond[i];
	}
	for (int i = 0; i < 3; ++i)
	{
		velocity[i] = cond[i+3];
	}
	acceleration = Acceleration(position, mu);

	std::array<double, 6> result;
	for(int i = 0; i < 3; ++i)
	{
		result[i] = velocity[i];
	}
	for(int i = 0; i < 3; ++i)
	{
		result[i+3] = acceleration[i];
	}

	return result;
}

Condition RungeKuttaStep(const Condition& condition, const double step, const double mu)
{
	const std::array<double, 6> k1 = proizv_cond(condition.cond, condition.time, mu);
	const std::array<double, 6> lin_komb1 = condition.cond + k1*(step/2);
	const std::array<double, 6> k2 = proizv_cond(lin_komb1, condition.time + step/2, mu);
	const std::array<double, 6> lin_komb2 = condition.cond + k2*(step/2);
	const std::array<double, 6> k3 = proizv_cond(lin_komb2, condition.time + step/2, mu);
	const std::array<double, 6> lin_komb3 = condition.cond + k3*step;
	const std::array<double, 6> k4 = proizv_cond(lin_komb3, condition.time + step, mu);
	const std::array<double, 6> updated_cond = condition.cond + (k1 + k2*2 + k3*2 + k4)*(step/6);
	const double updated_time = condition.time + step;

	return {updated_cond, updated_time};
}

std::vector<Condition> RungeKutta(const Condition& initialCondition, const double step, const double final_time, const double mu)
{
	const auto solutionSize = static_cast<std::size_t>(((final_time - initialCondition.time) / step)) + 2;
	std::vector<Condition> solution;
	solution.reserve(solutionSize);
	solution.push_back(initialCondition);
	
	for(std::size_t i = 1; i < solutionSize - 1; ++i)
	{
		solution.push_back(RungeKuttaStep(solution[i - 1], step, mu));
	}

	const double final_step = final_time - solution[solutionSize - 2].time;
	solution.push_back(RungeKuttaStep(solution[solutionSize - 2], final_step, mu));
	return solution;

}

int main()
{
	const double mu = 3.986e14;
	const double rho = 7e6;
	const double velocity = sqrt(mu / rho);
	const std::array<double, 6> cond = {rho, 0, 0, 0, velocity, 0};
        const double initialTime = 0;
	const double step = 10;

	const Condition initialCondition = {cond, initialTime};

	const double final_time = 2*3600;
	const std::vector<Condition> solution = RungeKutta(initialCondition, step, final_time, mu);

	for (int i = 0; i < 720; ++i)
	{
		std::cout << solution[i].cond[0] << " " << solution[i].cond[1] << " " << solution[i].cond[2] << std::endl;
	}

		

	return 0;
}
