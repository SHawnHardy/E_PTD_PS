#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D.py
# @version v0.7
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

noise_intensity = [x * 0.001 for x in range(501)]

csv_path = config.data_path + '/Q_D.csv'

try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print('Q_D.csv not found. It will be created', file=sys.stderr)
    df = pd.DataFrame({'noise intensity': noise_intensity,
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

ctrl.df.plot(x='noise intensity', y='Q')
ctrl.df.plot(x='noise intensity', y='ISI')
plt.show()
