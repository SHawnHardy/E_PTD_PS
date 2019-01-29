/**
 * @file test_ws_network.cpp
 * @version v0.1
 * @author SHawnHardy
 * @date 2019-01-28
 * @copyright MIT License
 */

#include <cmath>
#include <algorithm>
#include <iostream>

#include <gtest/gtest.h>

#include "ws_network.h"

TEST(WsNetworkTest, RegularNetwork) {
    const int Num_Vertices = 200;
    const int Regular_K = 10;
    WsNetwork tmp(Num_Vertices, Regular_K, -1.0);

    for (int i = 0; i < Num_Vertices; i++) {
        auto edges = tmp.get_edges(i);
        ASSERT_EQ(edges.second - edges.first, 2 * Regular_K);
        while (edges.first != edges.second) {
            int j = *(edges.first);
            ASSERT_NE(i, j);
            int a = std::min(i, j);
            int b = std::max(i, j);
            ASSERT_LE(std::min(b - a, a + Num_Vertices - b), Regular_K);
            ++edges.first;
        }
    }
}

TEST(WsNetworkTest, ClusteringCoefficient) {
    const int Num_Vertices = 200;
    const int Regular_K = 10;
    WsNetwork tmp(Num_Vertices, Regular_K, -1.0);
    ASSERT_LT(tmp.get_clustering_coefficient() -
              (3.0 * Regular_K - 3.0) / (4.0 * Regular_K - 2.0), 1E-8);
}

TEST(WsNetworkTest, WsNetwork) {
    const int Num_Vertices = 2000;
    const int Regular_K = 20;
    double pr = 0.05;
    while (pr < 1.0) {
        const double expect = (3.0 * Regular_K - 3.0) / (4.0 * Regular_K - 2.0) *
                              (1.0 - pr) * (1.0 - pr) * (1.0 - pr);
        double x = 0.0;
        for (int i = 0; i < 20; ++i) {
            WsNetwork tmp(Num_Vertices, Regular_K, pr);
            x += tmp.get_clustering_coefficient();
        }

        ASSERT_LT(fabs(x / 20.0 - expect), 0.025);
        pr += 0.1;
    }
}
