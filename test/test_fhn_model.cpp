/**
 * @file test_fhn_model.cpp
 * @version v0.2
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#include "iostream"
#include <gtest/gtest.h>

#include "fhn_model.h"
#include "time_delay_matrix.h"
#include "ws_network.h"

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
}