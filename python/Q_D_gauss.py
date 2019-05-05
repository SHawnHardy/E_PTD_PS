#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D_gauss.py
# @version v0.1
# @author SHawnHardy
# @date 2019-03-10
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
normal_distribution_mean = [6.0, 3.0]

csv_path = config.data_path + '/Q_D_gauss.csv'
Q_D_Tau_csv_path = config.data_path + '/Q_D_Tau.csv'

try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print(csv_path + "not found. It will be created", file=sys.stderr)
    noise_intensity_m, normal_distribution_mean_m = np.meshgrid(noise_intensity, normal_distribution_mean)
    normal_distribution_mean_m = normal_distribution_mean_m.flatten()
    noise_intensity_m = noise_intensity_m.flatten()
    df = pd.DataFrame({'noise intensity': noise_intensity_m,
                       'normal distribution mean': normal_distribution_mean_m,
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
Q_D_Tau_df = pd.read_csv(Q_D_Tau_csv_path)

Q_df = {'noise intensity': noise_intensity}
ISI_df = {'noise intensity': noise_intensity}
for N in normal_distribution_mean:
    Q_df['N=%.1fs' % (N,)] = (df.loc[df["normal distribution mean"] == N, "Q"]).reset_index(drop=True)
    Q_df['tau=%.1fs' % (N,)] = (Q_D_Tau_df.loc[Q_D_Tau_df["time delay"] == int(N * 1000), "Q"]).reset_index(drop=True)
    ISI_df['N=%.1fs' % (N,)] = (df.loc[df["normal distribution mean"] == N, "ISI"]).reset_index(drop=True)
    ISI_df['tau=%.1fs' % (N,)] = (Q_D_Tau_df.loc[Q_D_Tau_df["time delay"] == int(N * 1000), "ISI"]).reset_index(drop=True)

Q_df = pd.DataFrame(Q_df)
ISI_df = pd.DataFrame(ISI_df)

Q_df.plot(x='noise intensity')
ISI_df.plot(x='noise intensity')
plt.show()
