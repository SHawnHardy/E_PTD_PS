/**
 * @file test_fhn_model.cpp
 * @version v0.5
 * @author SHawnHardy
 * @date 2019-02-04
 * @copyright MIT License
 */

#include "iostream"
#include <gtest/gtest.h>

#include "fhn_model.h"
#include "time_delay_matrix.h"
#include "ws_network.h"

TEST(FhnModelConfig , Initial) {
    sh::FhnModelConfig config_1;
    ASSERT_DOUBLE_EQ(config_1.noise_intensity, 0.001);

    sh::FhnModelConfig config_2(0.002);
    ASSERT_DOUBLE_EQ(config_2.noise_intensity, 0.002);
}

TEST(FhnModelConfigTest, Serialize) {
    sh::FhnModelConfig source;
    std::ostringstream osm;
    sh::serialize_FhnModelConfig(source, osm);

    std::istringstream ism(osm.str());
    sh::FhnModelConfig *result = sh::deserialize_FhnModelConfit(ism);

    ASSERT_NEAR(source.epsilon, result->epsilon, 1E-8);
    ASSERT_NEAR(source.coupling_strength, result->coupling_strength, 1E-8);
    ASSERT_NEAR(source.systematic_param_a, result->systematic_param_a, 1E-8);
    ASSERT_NEAR(source.subthreshold_signal_amplitude, result->subthreshold_signal_amplitude, 1E-8);
    ASSERT_NEAR(source.subthreshold_signal_period, result->subthreshold_signal_period, 1E-8);
    ASSERT_NEAR(source.pulse_threshold, result->pulse_threshold, 1E-8);
    ASSERT_NEAR(source.noise_intensity, result->noise_intensity, 1E-8);
}

TEST(FhnModelSolverTest, Initial) {
    sh::WsNetwork network(200, 10, 0.15);
    sh::TimeDelayMatrix tau(200);
    sh::FhnModelConfig config;
    sh::FhnModelSolver solver(&network, &tau, &config, &(std::cout));
    solver.solve();

    std::ostringstream result_osm;
    solver.set_osm(&result_osm);
    solver.solve();
}

TEST(FhnModelSolverTest, LogX) {
    sh::WsNetwork network(200, 10, 0.15);
    sh::TimeDelayMatrix tau(200);
    sh::FhnModelConfig config;
    sh::FhnModelSolver solver(&network, &tau, &config, &(std::cout));
    solver.set_time(0, 0, 10);

    std::ostringstream log_x_osm;
    solver.enableLogX(&log_x_osm, [](int now){return true;});
    solver.solve();

    solver.disableLogX();
    solver.solve();
}

TEST(FhnModelSolverTest, LogPulse) {
    sh::WsNetwork network(200, 10, 0.15);
    sh::TimeDelayMatrix tau(200);
    sh::FhnModelConfig config(0.01);
    sh::FhnModelSolver solver(&network, &tau, &config, &(std::cout));

    std::ostringstream log_pulse_osm;
    solver.enableLogPulse(&log_pulse_osm);
    solver.solve();

    solver.disableLogPulse();
    solver.solve();
}
