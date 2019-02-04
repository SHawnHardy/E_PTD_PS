/**
 * @file solve.cpp
 * @version v0.1
 * @author SHawnHardy
 * @date 2019-02-03
 * @copyright MIT License
 */

#include <iostream>

#include "fhn_model.h"
#include "time_delay_matrix.h"
#include "ws_network.h"

int main() {
//    double time_delay_pr = 0.0;
//    double time_delay = 0.0;
    double noise_intensity = 0.001;

    sh::WsNetwork network(200, 10, 0.15);
    sh::TimeDelayMatrix tau(200);
    sh::FhnModelConfig config;
    config.noise_intensity = noise_intensity;

    sh::FhnModelSolver solver(&network, &tau, &config, &(std::cout));
    std::cout << solver.solve();

    return 0;
}