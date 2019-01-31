/**
 * @file test_ws_network.cpp
 * @version v0.2
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#include <cstring>
#include <algorithm>
#include <iostream>

#include <gtest/gtest.h>

#include "ws_network.h"

void cmpAdjacencyMatrixAndForwardStar(const sh::WsNetwork &w) {
    int v = w.Num_Vertices_;
    for (int i = 0; i < v; i++) {
        auto edge = w.get_edges(i);
        for (int j = 0; j < v; j++) {
            if (*(edge.first) == j) {
                ASSERT_TRUE(w.check(i, j));
                ++edge.first;
            } else {
                ASSERT_FALSE(w.check(i, j));
            }
        }
        ASSERT_EQ(edge.first, edge.second);
    }
}

TEST(WsNetworkTest, RegularNetwork) {
    const int Num_Vertices = 200;
    const int Regular_K = 10;
    sh::WsNetwork tmp(Num_Vertices, Regular_K, -1.0);
    ASSERT_EQ(tmp.Num_Vertices_, Num_Vertices);
    ASSERT_EQ(tmp.Num_Edges_, Num_Vertices * Regular_K * 2);
    ASSERT_EQ(tmp.Regular_K_, Regular_K);
    ASSERT_NEAR(tmp.Reconnection_Pr_, -1.0, 1E-8);
    cmpAdjacencyMatrixAndForwardStar(tmp);

    for (int i = 0; i < Num_Vertices; i++) {
        auto edges = tmp.get_edges(i);
        ASSERT_EQ(edges.second - edges.first, 2 * Regular_K);
        while (edges.first != edges.second) {
            int j = *(edges.first);
            ASSERT_NE(i, j);
            int a = std::min(i, j);
            int b = std::max(i, j);
            ASSERT_LE(std::min(b - a, a + Num_Vertices - b), Regular_K);
            ASSERT_TRUE(tmp.check(i, j));
            ++edges.first;
        }
    }
}

TEST(WsNetworkTest, ClusteringCoefficient) {
    const int Num_Vertices = 200;
    const int Regular_K = 10;
    sh::WsNetwork tmp(Num_Vertices, Regular_K, -1.0);
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
            sh::WsNetwork tmp(Num_Vertices, Regular_K, pr);
            ASSERT_EQ(tmp.Num_Vertices_, Num_Vertices);
            ASSERT_EQ(tmp.Num_Edges_, Num_Vertices * Regular_K * 2);
            ASSERT_EQ(tmp.Regular_K_, Regular_K);
            ASSERT_NEAR(tmp.Reconnection_Pr_, pr, 1E-8);
            cmpAdjacencyMatrixAndForwardStar(tmp);

            x += tmp.get_clustering_coefficient();
        }

        ASSERT_NEAR(x / 20.0, expect, 0.025);
        pr += 0.1;
    }
}

TEST(WsNetworkTest, serialize) {
    const int Num_Vertices = 200;
    const int Regular_K = 10;
    const double Reconnection_Pr = 0.2;
    sh::WsNetwork source(Num_Vertices, Regular_K, Reconnection_Pr);

    std::ostringstream osm;
    sh::serialize_WsNetwork(source, osm);

    std::istringstream ism(osm.str());
    sh::WsNetwork *result = sh::deserialize_WsNetwork(ism);

    ASSERT_EQ(source.Num_Vertices_, result->Num_Vertices_);
    ASSERT_EQ(source.Num_Edges_, result->Num_Edges_);
    ASSERT_EQ(source.Regular_K_, result->Regular_K_);
    ASSERT_NEAR(source.Reconnection_Pr_, result->Reconnection_Pr_, 1E-8);
    cmpAdjacencyMatrixAndForwardStar(*result);
    for (int i = 0; i < Num_Vertices; i++) {
        for (int j = 0; j < Num_Vertices; j++) {
            ASSERT_EQ(source.check(i, j), result->check(i, j));
        }
    }
}