/**
 * @file tau_matrix.h
 * @version v0.4
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#ifndef E_PTD_PS_TAU_MATRIX_H
#define E_PTD_PS_TAU_MATRIX_H

#include <cstring>
#include <algorithm>
#include <iostream>
#include <random>

namespace sh {
    class TimeDelayMatrix {
    public:
        const int Size_;

        explicit TimeDelayMatrix(const int size) : Size_(size) {
            tau_ = new int *[Size_];
            for (int i = 0; i < Size_; ++i) {
                tau_[i] = new int[Size_]();
            }
        }

        ~TimeDelayMatrix() {
            for (int i = 0; i < Size_; ++i) {
                delete[] tau_[i];
            }
            delete[] tau_;
        }

        void allTheSame(int x) {
            std::fill(tau_[0], tau_[0] + Size_, x);
            for (int i = 1; i < Size_; ++i) {
                memcpy(tau_[i], tau_[0], sizeof(int) * Size_);
            }
        }

        void partialTimeDelay(double pr, int time_delay) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dtb(0.0, 1.0);

            for (int i = 0; i < Size_; ++i) {
                tau_[i][i] = 0;
                for (int j = (i + 1); j < Size_; ++j) {
                    tau_[i][j] = tau_[j][i] = (dtb(gen) < pr ? time_delay : 0);
                }
            }
        }

        int *operator[](const int &x) const {
            return tau_[x];
        }

    private:
        int **tau_ = nullptr;
    };

    void serialize_TauMatrix(const TimeDelayMatrix &m, std::ostream &osm) {
        osm << m.Size_ << std::endl;
        for (int i = 0; i < m.Size_; ++i) {
            for (int j = 0; j < m.Size_; ++j) {
                osm << m[i][j] << (j == (m.Size_ - 1) ? '\n' : ' ');
            }
        }
    }

    TimeDelayMatrix *deserialize_TauMatrix(std::istream &ism) {
        int size;
        ism >> size;
        auto result = new TimeDelayMatrix(size);
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                ism >> (*result)[i][j];
            }
        }
        return result;
    }
}

#endif //E_PTD_PS_TAU_MATRIX_H
