/**
 * @file test_time_delay_matrix.cpp
 * @version v0.6
 * @author SHawnHardy
 * @date 2019-02-07
 * @copyright MIT License
 */

#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include "time_delay_matrix.h"

TEST(TimeDelayMatrixTest, TauMatrix) {
    const int Size = 200;
    sh::TimeDelayMatrix m(Size);
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_EQ(m[i][j], 0);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> int_dtb(0, 1000000007);
    int test_data[200][200];
    for (int i = 0; i < 200; ++i) {
        for (int j = 0; j < 200; ++j) {
            m[i][j] = test_data[i][j] = int_dtb(gen);
        }
    }

    for (int i = 0; i < 200; ++i) {
        for (int j = 0; j < 200; ++j) {
            ASSERT_EQ(test_data[i][j], m[i][j]);
        }
    }
}

TEST(TimeDelayMatrixTest, AllTheSame) {
    const int Size = 200;
    sh::TimeDelayMatrix m(Size);
    m.allTheSame(10);
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_EQ(m[i][j], 10);
        }
    }
}

TEST(TimeDelayMatrixTest, PartialTimeDelay) {
    const int Size = 20;
    sh::TimeDelayMatrix m(Size);
    int sum[Size][Size];
    m.partialTimeDelay(0.5, 1);

    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_DOUBLE_EQ(m[i][j], m[j][i]);
            ASSERT_TRUE(m[i][j] == 0 || m[i][j] == 1);
            sum[i][j] = m[i][j];
        }
    }

    for (int cas = 0; cas < 9999; ++cas) {
        m.partialTimeDelay(0.5, 1);
        for (int i = 0; i < Size; ++i) {
            for (int j = 0; j < Size; ++j) {
                sum[i][j] += m[i][j];
            }
        }
    }

    double expect = 10000 * 0.5;

    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_NEAR(expect, sum[i][j], expect * 0.05);
        }
    }
}

TEST(TimeDelayMatrixTest, Serialize) {
    const int Size = 200;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> int_dtb(0, 1000000007);
    int test_data[Size][Size];

    sh::TimeDelayMatrix source(Size);
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            source[i][j] = test_data[i][j] = int_dtb(gen);
        }
    }

    std::ostringstream osm;
    sh::serialize_TauMatrix(source, osm);

    std::istringstream ism(osm.str());
    sh::TimeDelayMatrix *result = sh::deserialize_TauMatrix(ism);

    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_EQ(source[i][j], test_data[i][j]);
            ASSERT_EQ((*result)[i][j], source[i][j]);
        }
    }
}
