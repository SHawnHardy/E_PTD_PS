#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file solve.py
# @version v0.4
# @author SHawnHardy
# @date 2019-04-26
# @copyright MIT License
#########################

from . import config

import re
import subprocess


def solve(series, verbose=False, log_pulse=False, log_pulse_filename="pulse_log.temp"):
    command = [config.solve_command, '-D' + str(series['noise intensity'])]

    if 'time delay' in series:
        command.append('-T' + str(series['time delay']))
    elif 'normal distribution mean' in series:
        command.append('-N' + str(series['normal distribution mean']))
        if 'normal distribution standard deviation' in series:
            command.append('-S' + str(series['normal distribution standard deviation']))
    elif 'uniform distribution mean' in series:
        _mean = series['uniform distribution mean']
        _standard_deviation = series['uniform distribution standard deviation']
        _a, _b = _mean - (3 ** 0.5) * _standard_deviation, _mean + (3 ** 0.5) * _standard_deviation
        command.append('-a' + str(_a))
        command.append('-b' + str(_b))

    if 'partial time delay probability' in series:
        command.append('-P' + str(series['partial time delay probability']))

    if verbose:
        command.append('-v')
    if log_pulse:
        command.append('--log_pulse')
        command.append(log_pulse_filename)

    detail = subprocess.check_output(command).decode()
    result = re.search(r'(?=\n?).*$', detail).group().split(' ')
    result = [float(x) for x in result]
    return dict(zip(['Q', 'Qsin', 'Qcos', 'ISI'], result)), detail
