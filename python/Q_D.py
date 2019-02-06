#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D.py
# @version v0.2
# @author SHawnHardy
# @date 2019-02-06
# @copyright MIT License
#########################

import multiprocessing
import matplotlib
import subprocess

import config

ctrl = config.Ctrl(50)
ctrl.label = [x * 0.001 for x in range(50)]
lock = multiprocessing.Lock()
manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(tp):
    index, label = tp
    result = subprocess.check_output([config.solve_command, '-D ' + str(label)])
    queue.put((index, result))


pool = multiprocessing.Pool(4)

for i in range(len(ctrl.label)):
    for j in range(ctrl.num_times):
        tp = (i, ctrl.label[i])
        pool.apply_async(solve, (tp,))

pool.close()

while True:
    t = queue.get()
    ctrl.add(*t)
