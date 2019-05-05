/**
 * @file tau_matrix.h
 * @version v0.8
 * @author SHawnHardy
 * @date 2019-04-26
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

        explicit TimeDelayMatrix(int size) : Size_(size) {
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

        void partial(double pr) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dtb(0.0, 1.0);

            for (int i = 0; i < Size_; ++i) {
                for (int j = i; j < Size_; ++j) {
                    if (dtb(gen) > pr)
                        tau_[i][j] = tau_[j][i] = 0;
                }
            }
        }

        void normalDistributionTimeDelay(double mean, double standard_deviation,
                                         int max_time_delay, int min_time_delay = 0, double step = 0.001) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::normal_distribution<> dtb(mean, standard_deviation);

            for (int i = 0; i < Size_; ++i) {
                for (int j = i; j < Size_; ++j) {
                    tau_[i][j] = tau_[j][i] = std::min(std::max(int(dtb(gen) / step), min_time_delay), max_time_delay);
                }
            }
        }

        void uniformDistributionTimeDelay(double a, double b, double step = 0.001) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dtb(a, b);

            for (int i = 0; i < Size_; ++i) {
                for (int j = i; j < Size_; ++j) {
                    tau_[i][j] = tau_[j][i] = int(dtb(gen) / step);
                }
            }

        }

        inline int *operator[](int x) const {
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
