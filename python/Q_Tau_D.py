#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_Tau_D.py
# @version v0.3
# @author SHawnHardy
# @date 2019-03-01
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

noise_intensity = [0.015, 0.134, 0.160]
time_delay = [x * 100 for x in range(301)]

csv_path = config.data_path + '/Q_Tau_D.csv'

try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print('Q_Tau_D.csv not found. It will be created', file=sys.stderr)
    time_delay_m, noise_intensity_m = np.meshgrid(time_delay, noise_intensity)
    noise_intensity_m = noise_intensity_m.flatten()
    time_delay_m = time_delay_m.flatten()
    df = pd.DataFrame({'noise intensity': noise_intensity_m,
                       'time delay': time_delay_m,
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
Q_df = {'time delay': time_delay}
ISI_df = {'time delay': time_delay}
for D in noise_intensity:
    Q_df['D=%.6f' % (D,)] = (df.loc[df['noise intensity'] == D, 'Q']).reset_index(drop=True)
    ISI_df['D=%.6f' % (D,)] = (df.loc[df['noise intensity'] == D, 'ISI']).reset_index(drop=True)

Q_df = pd.DataFrame(Q_df)
ISI_df = pd.DataFrame(ISI_df)

Q_df.plot(x='time delay')
ISI_df.plot(x='time delay')
plt.show()
