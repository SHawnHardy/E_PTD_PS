/**
 * @file ws_network.h
 * @version v0.1
 * @author SHawnHardy
 * @date 2019-01-27
 * @copyright MIT License
 */

#ifndef E_PTD_PS_WS_NETWORK_H
#define E_PTD_PS_WS_NETWORK_H

#include <cmath>
#include <algorithm>
#include <random>


class WsNetwork {
public:
    const int Num_Vertices_;
    const int Num_Edges_;
    const int Regular_K_;
    const double Reconnection_Pr_;

    WsNetwork(int size, int regular_k, double reconnection_pr) :
            Num_Vertices_(size), Regular_K_(regular_k), Reconnection_Pr_(reconnection_pr),
            Num_Edges_(Num_Vertices_ * regular_k * 2) {
        adjacency_matrix_ = new bool *[Num_Vertices_];
        edges_ = new int[Num_Edges_ + 1];
        start_ = new int *[Num_Vertices_];
        end_ = new int *[Num_Vertices_];

        for (int i = 0; i < Num_Vertices_; ++i) {
            adjacency_matrix_[i] = new bool[Num_Vertices_]();
            for (int j = 0; j < Num_Vertices_; ++j) {
                if (i == j) {
                    continue;
                }
                int dis = abs(i - j);
                adjacency_matrix_[i][j] = (std::min(dis, Num_Vertices_ - dis) <= Regular_K_);
            }
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> int_dtb(0, Num_Vertices_ - 1);
        std::uniform_real_distribution<> real_dtb(0.0, 1.0);

        for (int i = 1; i <= Regular_K_; ++i) {
            for (int neu_s = 0; neu_s < Num_Vertices_; ++neu_s) {
                if (real_dtb(gen) < Reconnection_Pr_) {
                    int neu_t = (neu_s + i) % Num_Vertices_;
                    adjacency_matrix_[neu_s][neu_t] = adjacency_matrix_[neu_t][neu_s] = false;
                    int new_neu_t = int_dtb(gen);
                    while (new_neu_t == neu_s || adjacency_matrix_[neu_s][new_neu_t]) {
                        new_neu_t = int_dtb(gen);
                    }
                    adjacency_matrix_[neu_s][new_neu_t] = adjacency_matrix_[new_neu_t][neu_s] = true;
                }
            }
        }

        int *point = edges_;
        for (int i = 0; i < Num_Vertices_; ++i) {
            start_[i] = point;
            for (int j = 0; j < Num_Vertices_; ++j) {
                if (adjacency_matrix_[i][j]) {
                    *point = j;
                    ++point;
                }
            }
            end_[i] = point;
        }
    }

    ~WsNetwork() {
        for (int i = 0; i < Num_Vertices_; ++i) {
            delete[] adjacency_matrix_[i];
        }
        delete[] adjacency_matrix_;
    }

    std::pair<int *, int *> get_edges(int x) const {
        return std::make_pair(start_[x], end_[x]);
    }

    bool check(int x, int y) const {
        return adjacency_matrix_[x][y];
    }

    double get_clustering_coefficient(int x) const {
        int lambda = 0;
        auto edges = get_edges(x);
        for (auto i = edges.first; i != edges.second; ++i) {
            for (auto j = (i + 1); j != edges.second; ++j) {
                if (check(*i, *j)) {
                    ++lambda;
                }
            }
        }
        int k = (int)(edges.second - edges.first);
        return 2.0 * (double)lambda / (double)(k * (k - 1));
    }

    double get_clustering_coefficient() const {
        double rst = 0.0;
        for (int i = 0; i < Num_Vertices_; ++i) {
            rst += get_clustering_coefficient(i);
        }
        return rst / (double)Num_Vertices_;
    }

private:
    bool **adjacency_matrix_;
    int *edges_;
    int **start_;
    int **end_;
};

#endif //E_PTD_PS_WS_NETWORK_H
