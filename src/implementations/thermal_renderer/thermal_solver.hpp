#pragma once

#include "thermal_common.hpp"

#include "../../../assets/shader/raytracing_thermal/defines.h"

#include <Eigen/Eigen>
#include <unsupported/Eigen/MatrixFunctions>
#include <unsupported/Eigen/NonLinearOptimization>

// TODO:
// DONE - find/fix RHINO small kelvin values before reload
// - add kelvin to celsius switch
// DONE - expose/fix steady state
// - fix patch lines on high res geometry?!
// - think on how to verify simulation

using namespace Eigen;

class ThermalSolver {

public:

	Vec ThermalSolver::residual(const Vec& x, const Vec& _currentKelvin, const Mat& _transportMatrix, const Vec& _fixedVars);
	void ThermalSolver::jacobian(const Vec& x, Mat& jac, const Mat& _transportMatrix, const Vec& _fixedVars);
	void ThermalSolver::solve(Vec& x, const Vec& _currentKelvin, const Mat& _transportMatrix, const Vec& _fixedVars);
	void reset();

	SCALAR step_size = 1000.0 * secondsUnitFactor;
	bool compute_steady_state = true;
	int mode = 0;

private:

	Mat jac;
	Vec dx;
	Vec res;
	Vec x_alpha_res;
	Vec prev_x;
	Vec prev_res;

	SCALAR f_tol = 1e-08;
	SCALAR x_tol = 1e-06;
	SCALAR a_tol = 1e-06;
	unsigned int max_iter = 1000;

	BiCGSTAB<Mat> solver;	
};