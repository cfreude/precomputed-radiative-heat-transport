#include "thermal_solver.hpp"

#include <iostream>

#include "spdlog/spdlog.h"

// System headers
#include <iostream>

// Eigen headers
#include <Eigen/Core>
#include <Eigen/Sparse>

// think about (m * x^3)^-1 = (x^3)^-1 * m^-1 precompute m^-1  

Vec ThermalSolver::residual(const Vec& x, const Vec& _currentKelvin, const Mat& _transportMatrix, const Vec& _fixedVars) {
	Vec _x = x;
	_x = (_fixedVars.array() < 1.0).select(_x, _currentKelvin);
	Vec res;
	if (compute_steady_state)
		res = step_size * _transportMatrix * _x.array().pow(4.0).matrix();
	else
		res = _currentKelvin + step_size * _transportMatrix * _x.array().pow(4.0).matrix() - _x;
	res = (_fixedVars.array() < 1.0).select(res, 0.0);
	return res;
}

void ThermalSolver::jacobian(const Vec& x, Mat& jac, const Mat& _transportMatrix, const Vec& _fixedVars) {

	int size = x.size();
	Vec diag = 4.0 * x.array().pow(3.0);

	//diag = (fixedVars.array() < 1.0).select(diag, 1.0);

	jac = step_size * DiagonalMatrix<SCALAR, Eigen::Dynamic>(diag) * _transportMatrix;

	if (!compute_steady_state)
		jac.diagonal() -= Vec::Ones(size);

	for (int i = 0; i < size; i++)
		if (_fixedVars[i] > 0.0) {
			jac.row(i).setConstant(0.0);
			jac.col(i).setConstant(0.0);
			jac(i, i) = 1.0;
		}
	}

void ThermalSolver::reset()
{
	jac.setConstant(0);
}

void ThermalSolver::solve(Vec& x, const Vec& _currentKelvin, const Mat& _transportMatrix, const Vec& _fixedVars)
{
	unsigned int vsize = x.rows() * x.cols();
	jac = Mat(vsize, vsize);
	dx = Vec(vsize);
	res = Vec(vsize);
	x_alpha_res = Vec(vsize);
	prev_x = x;
	prev_res = residual(x, _currentKelvin, _transportMatrix, _fixedVars);

	if (mode == 1)
	{
		// TODO: only compute 
		x = _transportMatrix * x;
		return;
	}

	for (int i = 0; i < max_iter; i++)
	{
		res = residual(x, _currentKelvin, _transportMatrix, _fixedVars);
		jacobian(x, jac, _transportMatrix, _fixedVars);

		//spdlog::debug("jac:\n{}", jac);
		//spdlog::debug("x:\n{}", x);
		//spdlog::debug("res:\n{}", res);

		solver.compute(jac);
		dx = -solver.solve(res);

		if (solver.info() == Eigen::ComputationInfo::NoConvergence)
		{
			spdlog::error("Eigen::ComputationInfo::NoConvergence");
			//break;
		}
		//spdlog::debug("dx:\n{}", dx);
		//spdlog::debug("norm: {}", (jac * dx + residual(x)).squaredNorm());

		// line search
		SCALAR alpha = 1.0;
		x_alpha_res = residual(x + alpha * dx, _currentKelvin, _transportMatrix, _fixedVars);
		SCALAR res_norm = res.squaredNorm();
		SCALAR x_alpha_norm = x_alpha_res.squaredNorm();
		while (res_norm < x_alpha_norm) // limit to 20 iters
		{
			alpha *= 0.5;
			x_alpha_res = residual(x + alpha * dx, _currentKelvin, _transportMatrix, _fixedVars);
			x_alpha_norm = x_alpha_res.squaredNorm();
		}

		x += alpha * dx;

		res = residual(x, _currentKelvin, _transportMatrix, _fixedVars);
		res_norm = res.squaredNorm();
		SCALAR dx_norm = (x - prev_x).squaredNorm();
		SCALAR dres_norm = (res - prev_res).squaredNorm();
		
		prev_x = x;
		prev_res = residual(x, _currentKelvin, _transportMatrix, _fixedVars);

		if (isnan(res_norm))
		{
			spdlog::error("ThermalSolver - value norm is NaN");
			break;
		}

		spdlog::info("ThermalSolver - iter: {}/{}, res_norm: {}, dres_norm: {} <> {} (f_tol), dx_norm: {} <> {} (x_tol), alpha: {} <> {} (a_tol)", i, max_iter, res_norm, dres_norm, f_tol, dx_norm, x_tol, alpha, a_tol);

		if (dres_norm < f_tol && dx_norm < x_tol)
		{
			spdlog::info("ThermalSolver - TERMINATING -> dres_norm: {} < {} (f_tol) AND dx_norm: {} < {} (x_tol)", res_norm, f_tol, dx_norm, x_tol);
			//spdlog::info("Jacobian condition number ...");
			//spdlog::info("Jacobian condition number: {:.3}", condistion_number(jac));
			break;
		}
		
	}
}