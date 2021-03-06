/**
 * @file fhn_model.h
 * @version v0.11
 * @author SHawnHardy
 * @date 2019-02-27
 * @copyright MIT License
 */

#ifndef E_PTD_PS_FHN_MODEL_H
#define E_PTD_PS_FHN_MODEL_H

#include <cassert>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>

#include "time_delay_matrix.h"
#include "ws_network.h"

namespace sh {
    struct FhnModelConfig {
        explicit FhnModelConfig(double noise_intensity = 0.001) : noise_intensity(noise_intensity) {}

        double epsilon = 0.01;
        double coupling_strength = 0.015;
        double systematic_param_a = 1.1;
        double subthreshold_signal_amplitude = 0.125;
        double subthreshold_signal_period = 6.0;
        double pulse_threshold = 0.0;
        double noise_intensity = 0.001;
    };

    void serialize_FhnModelConfig(const FhnModelConfig &Config, std::ostream &osm) {
        osm << "epsilon: " << Config.epsilon
            << " coupling_strength: " << Config.coupling_strength
            << " systematic_param_a: " << Config.systematic_param_a << std::endl;
        osm << "subthreshold_signal_amplitude: " << Config.subthreshold_signal_amplitude
            << " subthreshold_signal_period: " << Config.subthreshold_signal_period << std::endl;
        osm << "pulse_threshold: " << Config.pulse_threshold
            << " noise_intensity: " << Config.noise_intensity << std::endl;
    };

    FhnModelConfig *deserialize_FhnModelConfit(std::istream &ism) {
        auto config = new FhnModelConfig;
        std::string tmp;
        ism >> tmp >> config->epsilon >> tmp >> config->coupling_strength >> tmp >> config->systematic_param_a;
        ism >> tmp >> config->subthreshold_signal_amplitude >> tmp >> config->subthreshold_signal_period;
        ism >> tmp >> config->pulse_threshold >> tmp >> config->noise_intensity;
        return config;
    }

    class FhnModelSolver {
    public:
        const double PI = acos(-1.0);
        const int Size_;

        FhnModelSolver(const sh::WsNetwork *Ws_Network, const sh::TimeDelayMatrix *Tau_Matrix,
                       const sh::FhnModelConfig *Fhn_Config, std::ostream *result_osm) :
                Ws_Network_(Ws_Network), Tau_Matrix_(Tau_Matrix), Config_(Fhn_Config),
                Size_(Ws_Network->Num_Vertices_), result_osm_(result_osm) {
            assert(Size_ <= Tau_Matrix_->Size_);
            buffer_length_ = new int[Size_ + 1];
            current_ = new double *[Size_];
            next_ = new double *[Size_];
            x_with_delay_ = new double **[Size_];

            buffer_length_[0] = 0;
            for (int i = 0; i < Size_; ++i) {
                int buf_len = 0;
                for (int j = 0; j < Size_; ++j) {
                    buf_len = std::max(buf_len, (*Tau_Matrix_)[j][i]);
                }
                buffer_length_[i + 1] = buffer_length_[i] + buf_len + 2;
                x_with_delay_[i] = new double *[Size_];
            }
            x_ = new double[end_time_ - start_time_ + buffer_length_[Size_]];
            y_ = new double[Size_];
            dx_ = new double[Size_];
            dy_ = new double[Size_];
            first_pulse_time_ = new int[Size_];
            last_pulse_time_ = new int[Size_];
        }

        ~FhnModelSolver() {
            for (int i = 0; i < Size_; ++i) {
                delete[] x_with_delay_[i];
            }
            delete[] x_;
            delete[] x_with_delay_;
            delete[] buffer_length_;
            delete[] current_;
            delete[] next_;

            delete[] y_;
            delete[] dx_;
            delete[] dy_;
            delete[] first_pulse_time_;
            delete[] last_pulse_time_;
        }

        void set_time(int start_time, int steady_time, int end_time, double step_duration = 0.001) {
            start_time_ = start_time;
            steady_time_ = steady_time;
            end_time_ = end_time;
            step_ = step_duration;
            delete[] x_;
            x_ = new double[end_time_ - start_time_ + buffer_length_[Size_]];
        }

        void enableLogX(std::ostream *osm, bool (*log_x_filter)(int)) {
            log_x_ = true;
            log_x_osm_ = osm;
            log_x_filter_ = log_x_filter;
        }

        void disableLogX() {
            log_x_ = false;
            log_x_osm_ = nullptr;
        }

        void enableLogPulse(std::ostream *osm) {
            log_pulse_ = true;
            log_pulse_osm_ = osm;
        }

        void disableLogPulse() {
            log_pulse_ = false;
            log_pulse_osm_ = nullptr;
        }

        void set_osm(std::ostream *result_osm) {
            result_osm_ = result_osm;
        }

        double solve() {
            (*result_osm_) << std::setprecision(8) << std::setiosflags(std::ios::fixed);
            (*result_osm_) << info_;
            if (log_pulse_) {
                (*log_pulse_osm_) << std::setprecision(8) << std::setiosflags(std::ios::fixed);
                (*log_pulse_osm_) << info_;
                (*log_pulse_osm_) << "time(s)," << "neuron," << "x_neu(t)," << "x_neu(t + dt)" << std::endl;
            }
            if (log_x_) {
                (*log_x_osm_) << std::setprecision(8) << std::setiosflags(std::ios::fixed);
                (*log_x_osm_) << info_;
                (*log_x_osm_) << "time(s),";
                for (int i = 0; i < Size_; ++i) {
                    (*log_x_osm_) << i << (i == (Size_ - 1) ? '\n' : ',');
                }
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dtb(0, 1);

            std::fill(y_, y_ + Size_, 0.0);
            std::fill(x_, x_ + buffer_length_[Size_], 0.0);
            std::fill(first_pulse_time_, first_pulse_time_ + Size_, -1);
            std::fill(last_pulse_time_, last_pulse_time_ + Size_, -1);

            for (int i = 0; i < Size_; ++i) {
                next_[i] = x_ + buffer_length_[Size_] - 1 - buffer_length_[i];
                current_[i] = next_[i] - 1;
                for (int j = 0; j < Size_; ++j) {
                    x_with_delay_[j][i] = current_[i] - (*Tau_Matrix_)[j][i];
                }
            }

            double qsin = 0.0, qcos = 0.0;
            int data_num = 0;
            long long int pulse_num = 0;

            for (int now = start_time_; now < end_time_; ++now) {
                getDx();
                getDy(now);

                bool log_x_this_time = log_x_ && log_x_filter_(now);
                double mean_x = 0.0;
                if (log_x_this_time) {
                    (*log_x_osm_) << now * step_ << ',';
                }
                for (int i = 1; i < Size_; ++i) {
                    double noise = sqrt(-4.0 * step_ * log(dtb(gen))) *
                                   cos(2.0 * PI * dtb(gen));

                    *next_[i] = *current_[i] + step_ * dx_[i];
                    y_[i] += (step_ * dy_[i] + Config_->noise_intensity * noise);

                    if (now >= steady_time_) {
                        mean_x += *next_[i];
                        if (*current_[i] <= Config_->pulse_threshold &&
                            *next_[i] > Config_->pulse_threshold) {

                            if (last_pulse_time_[i] > 0) {
                                last_pulse_time_[i] = now;
                                pulse_num++;
                            } else {
                                first_pulse_time_[i] = last_pulse_time_[i] = now;
                            }

                            if (log_pulse_) {
                                (*log_pulse_osm_) << now * step_ << ',' << i << ','
                                                  << *current_[i] << ',' << *next_[i]
                                                  << std::endl;
                            }
                        }


                        if (log_x_this_time) {
                            (*log_x_osm_) << *next_[i] << (i == (Size_ - 1) ? '\n' : ',');
                        }

                    }
                    current_[i] = next_[i];
                    next_[i]++;
                }

                if (now >= steady_time_) {
                    mean_x /= Size_;
                    double tmp = (2.0 * PI * now * step_ / Config_->subthreshold_signal_period);
                    qsin += 2.0 * mean_x * sin(tmp);
                    qcos += 2.0 * mean_x * cos(tmp);
                    data_num++;
                }
            }
            qsin /= data_num;
            qcos /= data_num;
            double result = sqrt(qsin * qsin + qcos * qcos);
            long long int pulse_length = 0;
            for (int i = 0; i < Size_; i++) {
                pulse_length += (last_pulse_time_[i] - first_pulse_time_[i]);
            }
            (*result_osm_) << result << ' ' << qsin << ' ' << qcos << ' '
                           << (double) pulse_length / (double) pulse_num * step_ << std::endl;
            return result;
        }

    private:
        const sh::WsNetwork *const Ws_Network_;
        const sh::TimeDelayMatrix *const Tau_Matrix_;
        const sh::FhnModelConfig *const Config_;
        std::ostream *result_osm_ = nullptr;
        std::ostream *log_pulse_osm_ = nullptr;
        std::ostream *log_x_osm_ = nullptr;

        std::string info_ = std::string("");

        double *x_ = nullptr;
        int *buffer_length_ = nullptr;
        double **current_ = nullptr;
        double **next_ = nullptr;
        double ***x_with_delay_ = nullptr;
        double *y_ = nullptr;
        double *dx_ = nullptr;
        double *dy_ = nullptr;
        int *first_pulse_time_ = nullptr;
        int *last_pulse_time_ = nullptr;

        double step_ = 0.001;
        int start_time_ = 0;
        int steady_time_ = 400000;
        int end_time_ = 500000;

        bool log_pulse_ = false;
        bool log_x_ = false;

        bool (*log_x_filter_)(int) = [](int x) { return false; };

        void getDx() {
            for (int i = 0; i < Size_; ++i) {
                double xi = *current_[i];
                dx_[i] = xi - xi * xi * xi / 3.0 - y_[i];
                double coupling_term = 0.0;

                auto edges = Ws_Network_->get_edges(i);
                auto edges_start = edges.first;
                auto edges_end = edges.second;
                while (edges_start != edges_end) {
                    int t = *(edges_start);
                    coupling_term += (*x_with_delay_[i][t] - xi);

                    x_with_delay_[i][t]++;
                    edges_start++;
                }
                dx_[i] += Config_->coupling_strength * coupling_term;
                dx_[i] /= Config_->epsilon;
            }
        }

        void getDy(int now) {
            double addon = Config_->systematic_param_a +
                           Config_->subthreshold_signal_amplitude *
                           sin((2 * now) * (PI / Config_->subthreshold_signal_period * step_));
            for (int i = 0; i < Size_; ++i) {
                dy_[i] = *current_[i] + addon;
            }
        }
    };
}
#endif //E_PTD_PS_FHN_MODEL_H
