/**
 * @file tau_matrix.h
 * @version v0.1
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#ifndef E_PTD_PS_TAU_MATRIX_H
#define E_PTD_PS_TAU_MATRIX_H

#include <cstring>
#include <algorithm>
#include <iostream>

namespace sh {
    class TauMatrix {
    public:
        const int Size_;

        TauMatrix(int size) : Size_(size) {
            tau_ = new int *[Size_];
            for (int i = 0; i < Size_; ++i) {
                tau_[i] = new int[Size_]();
            }
        }

        void allTheSame(int x) {
            std::fill(tau_[0], tau_[0] + Size_, x);
            for (int i = 1; i < Size_; ++i) {
                memcpy(tau_[i], tau_[0], sizeof(int) * Size_);
            }
        }

        int *operator[](const int &x) const {
            return tau_[x];
        }

    private:
        int **tau_;
    };

    void serialize_TauMatrix(const TauMatrix &m, std::ostream &osm) {
        osm << m.Size_ << std::endl;
        for (int i = 0; i < m.Size_; ++i) {
            for (int j = 0; j < m.Size_; ++j) {
                osm << m[i][j] << (j == (m.Size_ - 1) ? '\n' : ' ');
            }
        }
    }

    TauMatrix *deserialize_TauMatrix(std::istream &ism) {
        int size;
        ism >> size;
        auto result = new TauMatrix(size);
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                ism >> (*result)[i][j];
            }
        }
        return result;
    }
}

#endif //E_PTD_PS_TAU_MATRIX_H
