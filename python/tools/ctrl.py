#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file ctrl.py
# @version v0.2
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

import numpy as np
import pandas as pd
import sys


class Ctrl:
    task = []
    df_index = []
    result = []

    def __init__(self, df, csv_path, num_times=10):
        self.df = df
        self.csv_path = csv_path
        self.df.info(buf=sys.stderr)
        self.num_times = num_times

    def get_task(self):
        task_index = 0
        for index, row in self.df.iterrows():
            if np.any(np.isnan(row.drop('ISI'))):
                self.task.append(row)
                self.df_index.append(index)
                self.result.append([])
                yield task_index, self.task[-1]
                task_index += 1

    def proc(self, task_index):
        print(self.task[task_index].drop(['Q', 'Qsin', 'Qcos', 'ISI']))
        result = pd.DataFrame(self.result[task_index])
        print(result)
        if np.count_nonzero(np.isnan(result['ISI'])) > self.num_times * 0.3:
            print('ISI is invalid')
            result['ISI'] = np.nan
        result = np.mean(result)
        print(result)
        sys.stdout.flush()
        for label, data in result.iteritems():
            self.df.loc[self.df_index[task_index], label] = data

        self.df = self.df.round(6)
        self.df.to_csv(self.csv_path, index=False, float_format='%.6f')

    def add(self, task_index, result):
        self.result[task_index].append(result)
        if len(self.result[task_index]) == self.num_times:
            self.proc(task_index)
            return True
        return False
