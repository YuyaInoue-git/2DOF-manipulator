// 2linkmanipulator.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include<iomanip>
#include <vector>
#include <iostream>
#include <fstream>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

void RungeKutta(MatrixXd dX, MatrixXd &X, MatrixXd u, double tt, double dt, MatrixXd A, MatrixXd B, MatrixXd C, MatrixXd D) {
	MatrixXd k1 = A * X + B * u;
	MatrixXd k2 = A * (X + 0.5*k1*dt) + B * u;
	MatrixXd k3 = A * (X + 0.5*k2*dt) + B * u;
	MatrixXd k4 = A * (X + k3 * dt) + B * u;
	MatrixXd k = (k1 + 2.0*k2 + 2.0*k3 + k4)*dt / 6.0;
	X = X + k;
}

int main(){          
	const double link2 = 0.1; 

	/*目標値*/
	const double x_target = 0.1;
	const double y_target = 0.15;

	const double weight = 1e-2;

	/*リンクの角度*/
	double theta1 = 0.0;
	double theta2 = 0.0;
	MatrixXd theta_vector(2, 1);
	theta_vector(0, 0) = theta1;
	theta_vector(1, 0) = theta2;

	/*関節2の位置*/
	double x_2 = 0.0;
	double y_2 = 0.0;

	/*先端の位置*/
	double x_h = 0.0;
	double y_h = 0.0;

	/*ヤコビ行列*/
	MatrixXd J_matrix(2, 2);

	/*ヤコビ逆行列*/
	MatrixXd Inverse_J(2, 2);

	/*角度*/
	MatrixXd Delta(2, 1);

	/*角度の微小変化*/
	MatrixXd Delta_theta(2, 1);

	/*手先位置の誤差*/
	double Delta_x = 0.0;
	double Delta_y = 0.0;

	const double Ts = 0.001;
	double t = 0.0;

	/*行列演算*/
	int i, j;

	/* system parameter and variables */
	const double simulationTime = 5;
	std::ofstream dataFile("EncryptedPneumaticCylinder.csv");

	/*Simulation*/
	while (t < simulationTime) {
		/*関節2位置*/
		x_2 = link1 * cos(theta1);
		y_2 = link1 * sin(theta2);

		/*先端位置*/
		x_h = link1 * cos(theta1) + link2 * cos(theta1 + theta2);
		y_h = link1 * sin(theta1) + link2 * sin(theta1 + theta2);

		/*手先位置の誤差*/
		Delta_x = x_target - x_h;
		Delta_y = y_target - y_h;
		Delta(0, 0) = Delta_x;
		Delta(1, 0) = Delta_y;

		J_matrix(0, 0) = -link1 * sin(theta1) - link2 * sin(theta1 + theta2);
		J_matrix(0, 1) = -link2 * sin(theta1 + theta2);
		J_matrix(1, 0) = link1 * cos(theta1) + link2 * cos(theta1 + theta2);
		J_matrix(1, 1) = link2 * cos(theta1 + theta2);

		/*逆行列*/
		Inverse_J = J_matrix.inverse();
		
		/*行列演算*/
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				Delta_theta(i, 0) = Inverse_J(i, j)*Delta(j, 0);
			}
		}

		theta_vector = theta_vector + Delta_theta * weight;
		theta1 = theta_vector(0, 0);
		theta2 = theta_vector(1, 0);
		cout << Delta_theta(0,0) << endl;

		/* update time */
		t += Ts;
	}

	dataFile.close();

    return 0;
}

