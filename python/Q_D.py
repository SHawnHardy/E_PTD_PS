#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D.py
# @version v0.5
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

import config
from ctrl import Ctrl

import matplotlib.pyplot as plt
import multiprocessing
import numpy as np
import pandas as pd
import subprocess

try:
    df = pd.read_csv(config.data_path + 'Q_D.csv')
except FileNotFoundError:
    print("Q_D.csv not found. It will be created")
    df = pd.DataFrame({'noise intensity': [x * 0.001 for x in range(501)],
                       'Q': np.nan
                       })

df = df.round(6)
ctrl = Ctrl(df, ['noise intensity'], ['Q'], config.data_path + 'Q_D.csv')
ctrl.num_times = 10

lock = multiprocessing.Lock()
manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(info):
    _index, _task = info
    result = subprocess.check_output([config.solve_command, '-D ' + str(_task['noise intensity'])])
    queue.put((_index, pd.Series({'Q': float(result)})))


pool = multiprocessing.Pool(config.num_processes)

no_task_left = True
for index, task in ctrl.get_task():
    no_task_left = False
    for i in range(ctrl.num_times):
        pool.apply_async(solve, ((index, task),))

pool.close()

while not no_task_left:
    t = queue.get()
    no_task_left = ctrl.add(*t)

plt.figure()
ctrl.df.plot(x='noise intensity', y='Q')
plt.show()
