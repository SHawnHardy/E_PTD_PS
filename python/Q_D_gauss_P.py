#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D_gauss_P.py
# @version v0.1
# @author SHawnHardy
# @date 2019-03-12
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

noise_intensity = [x * 0.001 for x in range(501)]
normal_distribution_mean = 3.0
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7, 0.9]

csv_path = config.data_path + '/Q_D_gauss_P_N%.4f.csv' % (normal_distribution_mean,)

try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print(csv_path + " not found. It will be created", file=sys.stderr)
    noise_intensity_m, partial_time_delay_pr_m = np.meshgrid(noise_intensity, partial_time_delay_pr)
    partial_time_delay_pr_m = partial_time_delay_pr_m.flatten()
    noise_intensity_m = noise_intensity_m.flatten()
    df = pd.DataFrame({'noise intensity': noise_intensity_m,
                       'normal distribution mean': normal_distribution_mean,
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
Q_df = {'noise intensity': noise_intensity}
ISI_df = {'noise intensity': noise_intensity}
for P in partial_time_delay_pr:
    Q_df['P=%.6f' % (P,)] = (df.loc[df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
    ISI_df['P=%.6f' % (P,)] = (df.loc[df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

Q_df = pd.DataFrame(Q_df)
ISI_df = pd.DataFrame(ISI_df)

Q_df.plot(x='noise intensity')
ISI_df.plot(x='noise intensity')
plt.show()
