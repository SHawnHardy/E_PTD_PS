/**
 * @file test_tau_matrix.cpp
 * @version v0.1
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include "tau_matrix.h"

TEST(TauMatrixTest, TauMatrix) {
    const int Size = 200;
    sh::TauMatrix m(Size);
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

TEST(TauMatrixTest, AllTheSame) {
    const int Size = 200;
    sh::TauMatrix m(Size);
    m.allTheSame(10);
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_EQ(m[i][j], 10);
        }
    }
}

TEST(TauMatrixTest, Serialize) {
    const int Size = 200;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> int_dtb(0, 1000000007);
    int test_data[Size][Size];

    sh::TauMatrix source(Size);
    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            source[i][j] = test_data[i][j] = int_dtb(gen);
        }
    }

    std::ostringstream osm;
    sh::serialize_TauMatrix(source, osm);

    std::istringstream ism(osm.str());
    sh::TauMatrix *result = sh::deserialize_TauMatrix(ism);

    for (int i = 0; i < Size; ++i) {
        for (int j = 0; j < Size; ++j) {
            ASSERT_EQ(source[i][j], test_data[i][j]);
            ASSERT_EQ((*result)[i][j], source[i][j]);
        }
    }
}
