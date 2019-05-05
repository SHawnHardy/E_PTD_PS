#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_U.py
# @version v0.1
# @author SHawnHardy
# @date 2019-04-26
# @copyright MIT License
#########################

from tools import config
from tools.ctrl import Ctrl
from tools.solve import solve

import matplotlib.pyplot as plt
import multiprocessing
import numpy as np
import pandas as pd
import sys
import tqdm

noise_intensity = 0.160
uniform_distribution_mean = [2.0 + x * 0.1 for x in range(231)]
uniform_distribution_standard_deviation = [1.0, 0.8, 0.5]
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7]

csv_path = config.data_path + '/Q_U_D%.4f.csv' % (noise_intensity,)

try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print(csv_path + " not found. It will be created", file=sys.stderr)
    partial_time_delay_pr_m, uniform_distribution_standard_deviation_m, uniform_distribution_mean_m = np.meshgrid(
        partial_time_delay_pr,
        uniform_distribution_standard_deviation,
        uniform_distribution_mean
    )
    uniform_distribution_mean_m = uniform_distribution_mean_m.flatten()
    partial_time_delay_pr_m = partial_time_delay_pr_m.flatten()
    uniform_distribution_standard_deviation_m = uniform_distribution_standard_deviation_m.flatten()
    df = pd.DataFrame({'noise intensity': noise_intensity,
                       'uniform distribution mean': uniform_distribution_mean_m,
                       'uniform distribution standard deviation': uniform_distribution_standard_deviation_m,
                       'partial time delay probability': partial_time_delay_pr_m,
                       'Q': np.nan,
                       'Qsin': np.nan,
                       'Qcos': np.nan,
                       'ISI': np.nan
                       })

df = df.round(6)
ctrl = Ctrl(df, csv_path)

manager = multiprocessing.Manager()
queue = manager.Queue()


def worker(info):
    _task_index, _task = info
    queue.put((_task_index, solve(_task)[0]))


pool = multiprocessing.Pool(config.num_processes)

num_task = len(df)
num_task_left = 0
for task_index, task in ctrl.get_task():
    num_task_left += 1
    for _ in range(ctrl.num_times):
        pool.apply_async(worker, ((task_index, task),))

pool.close()

with tqdm.tqdm(total=num_task, initial=num_task - num_task_left) as pbar:
    while num_task_left > 0:
        if ctrl.add(*(queue.get())):
            num_task_left -= 1
            pbar.update(1)

df = ctrl.df

for SD in uniform_distribution_standard_deviation:
    t_df = df.loc[df["uniform distribution standard deviation"] == SD]
    Q_df = {'uniform distribution mean': uniform_distribution_mean}
    ISI_df = {'uniform distribution mean': uniform_distribution_mean}
    for P in partial_time_delay_pr:
        Q_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
        ISI_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    Q_df = pd.DataFrame(Q_df)
    ISI_df = pd.DataFrame(ISI_df)

    Q_df.plot(x='uniform distribution mean', title='Q_SD=%.4f' % (SD,))
    ISI_df.plot(x='uniform distribution mean', title='ISI_SD=%.4f' % (SD,))
    plt.show()
