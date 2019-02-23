/**
 * @file solve.cpp
 * @version v0.3
 * @author SHawnHardy
 * @date 2019-02-10
 * @copyright MIT License
 */

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <string>

#include "fhn_model.h"
#include "time_delay_matrix.h"
#include "ws_network.h"

struct GlobalArgs {
    bool verbose = false;
    bool dry_run = false;
    int size = 200;

//    ws_network
    int regular_k = 10;
    double reconnection_pr = 0.15;

//    time_delay_matrix
    enum TimeDelayMatrixType {
        all_the_same,
        partial
    };
    TimeDelayMatrixType time_delay_matrix_type = all_the_same;
    int time_delay = 0;
    double partial_time_delay_pr = 2.0;

//    fhn_model_config
    double noise_intensity = 0.001;

//    log_pulse
    bool log_pulse = false;
    std::string log_pulse_filename = std::string("");
};


static const char *optString = "vs:D:T:P:";

static const struct option long_opts[] = {
        {"verbose",               0, nullptr, 'v'},
        {"size",                  1, nullptr, 's'},
        {"noise_intensity",       1, nullptr, 'D'},
        {"tau",                   1, nullptr, 'T'},
        {"partial_time_delay_pr", 1, nullptr, 'P'},
        {"log_pulse",             1, nullptr, 301},
        {"dry_run",               0, nullptr, 401}
};

int main(int argc, char **argv) {
    GlobalArgs global_args;
    int opt;
    while ((opt = getopt_long(argc, argv, optString, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'v':
                global_args.verbose = true;
                std::cout << "Verbose Mode On" << std::endl;
                break;
            case 's':
                global_args.size = std::atoi(optarg);
                break;
            case 'D':
                global_args.noise_intensity = std::atof(optarg);
                break;
            case 'T':
                global_args.time_delay = std::atoi(optarg);
                break;
            case 'P':
                global_args.time_delay_matrix_type = global_args.partial;
                global_args.partial_time_delay_pr = std::atof(optarg);
                break;
            case 401:
                global_args.dry_run = true;
                global_args.verbose = true;
                break;
            case 301:
                global_args.log_pulse = true;
                global_args.log_pulse_filename = std::string(optarg);
                break;
            case '?':
                std::cout << "unknown option" << std::endl;
                break;
            default:
                std::cout << "error" << std::endl;
        }
    }

    if (global_args.verbose) {
        std::cout << "size = " << global_args.size << std::endl;
        std::cout << "noise intensity = " << global_args.noise_intensity << std::endl;
        std::cout << "time delay = " << global_args.time_delay << " * 0.001 s ";
        switch (global_args.time_delay_matrix_type) {
            case global_args.all_the_same:
                std::cout << "(all the same)" << std::endl;
                break;
            case global_args.partial:
                std::cout << "(partial; probability = " << global_args.partial_time_delay_pr << " )" << std::endl;
                break;
            default:
                std::cout << "error" << std::endl;
        }

        std::cout << "log pulse: " << (global_args.log_pulse ? "ENABLED" : "DISABLED") << std::endl;
        if (global_args.log_pulse) {
            std::cout << "log_pulse_filename: " << global_args.log_pulse_filename << std::endl;
        }
    }
    if (global_args.dry_run) {
        return 0;
    }

    sh::WsNetwork network(global_args.size, global_args.regular_k, global_args.reconnection_pr);

    sh::TimeDelayMatrix tau(global_args.size);
    if (global_args.time_delay > 0) {
        if (global_args.time_delay_matrix_type == global_args.all_the_same) {
            tau.allTheSame(global_args.time_delay);
        } else if (global_args.time_delay_matrix_type == global_args.partial) {
            tau.partialTimeDelay(global_args.partial_time_delay_pr, global_args.time_delay);
        }
    }

    sh::FhnModelConfig config(global_args.noise_intensity);
    sh::FhnModelSolver solver(&network, &tau, &config, &(std::cout));
    if (global_args.log_pulse) {
        std::ostream *log_pulse_osm = new std::ofstream(global_args.log_pulse_filename.c_str(), std::ios::out);
        solver.enableLogPulse(log_pulse_osm);
    }
    solver.solve();
    return 0;
}