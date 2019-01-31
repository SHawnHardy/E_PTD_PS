/**
 * @file ws_network.h
 * @version v0.3
 * @author SHawnHardy
 * @date 2019-01-31
 * @copyright MIT License
 */

#ifndef E_PTD_PS_WS_NETWORK_H
#define E_PTD_PS_WS_NETWORK_H

#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <random>

namespace sh {
    class WsNetwork {
    public:
        const int Num_Vertices_;
        const int Num_Edges_;
        const int Regular_K_;
        const double Reconnection_Pr_;

        WsNetwork(int size, int regular_k, double reconnection_pr) :
                Num_Vertices_(size), Num_Edges_(Num_Vertices_ * regular_k * 2),
                Regular_K_(regular_k), Reconnection_Pr_(reconnection_pr) {
            adjacency_matrix_ = new bool *[Num_Vertices_];
            edges_ = new int[Num_Edges_];
            forward_star_ = new int *[Num_Vertices_ + 1];

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
                forward_star_[i] = point;
                for (int j = 0; j < Num_Vertices_; ++j) {
                    if (adjacency_matrix_[i][j]) {
                        *point = j;
                        ++point;
                    }
                }
            }
            forward_star_[Num_Vertices_] = point;
        }

        WsNetwork(int num_vertices, int num_edges, int regular_k, double reconnection_pr,
                  int edges[], int degree[]) :
                Num_Vertices_(num_vertices), Num_Edges_(num_edges),
                Regular_K_(regular_k), Reconnection_Pr_(reconnection_pr) {
            adjacency_matrix_ = new bool *[Num_Vertices_];
            edges_ = new int[Num_Edges_];
            forward_star_ = new int *[Num_Vertices_ + 1];

            memcpy(edges_, edges, sizeof(int) * num_edges);
            int *point = edges_;
            for (int i = 0; i < Num_Vertices_; i++) {
                forward_star_[i] = point;

                adjacency_matrix_[i] = new bool[Num_Vertices_]();
                for (int j = 0; j < degree[i]; j++) {
                    adjacency_matrix_[i][*point] = true;
                    ++point;
                }
            }
            forward_star_[Num_Vertices_] = point;
        }

        ~WsNetwork() {
            for (int i = 0; i < Num_Vertices_; ++i) {
                delete[] adjacency_matrix_[i];
            }
            delete[] adjacency_matrix_;
            delete[] edges_;
            delete[] forward_star_;
        }

        std::pair<int *, int *> get_edges(int x) const {
            return std::make_pair(forward_star_[x], forward_star_[x + 1]);
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
            int k = (int) (edges.second - edges.first);
            return 2.0 * (double) lambda / (double) (k * (k - 1));
        }

        double get_clustering_coefficient() const {
            double rst = 0.0;
            for (int i = 0; i < Num_Vertices_; ++i) {
                rst += get_clustering_coefficient(i);
            }
            return rst / (double) Num_Vertices_;
        }

    private:
        bool **adjacency_matrix_;
        int *edges_;
        int **forward_star_;
    };

    void serialize_WsNetwork(const WsNetwork &w, std::ostream &osm) {
        osm << w.Num_Vertices_ << std::endl;
        osm << w.Num_Edges_ << std::endl;
        osm << w.Regular_K_ << ' ' << w.Reconnection_Pr_ << std::endl;
        for (int i = 0; i < w.Num_Vertices_; i++) {
            auto edge = w.get_edges(i);
            osm << edge.second - edge.first;
            while (edge.first != edge.second) {
                osm << ' ' << *(edge.first);
                ++edge.first;
            }
            osm << std::endl;
        }
    }

    WsNetwork *deserialize_WsNetwork(std::istream &ism) {
        int num_vertices, num_edges, regular_k;
        double reconnection_pr;
        ism >> num_vertices >> num_edges >> regular_k >> reconnection_pr;
        int *edge = new int[num_edges];
        int *degree = new int[num_vertices];
        int *point = edge;
        for (int i = 0; i < num_vertices; i++) {
            ism >> degree[i];
            for (int j = 0; j < degree[i]; j++) {
                ism >> *point;
                ++point;
            }
        }
        return new WsNetwork(num_vertices, num_edges, regular_k, reconnection_pr, edge, degree);
    }
}
#endif //E_PTD_PS_WS_NETWORK_H
