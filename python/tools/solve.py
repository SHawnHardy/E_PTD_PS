#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file solve.py
# @version v0.2
# @author SHawnHardy
# @date 2019-03-10
# @copyright MIT License
#########################

from . import config

import re
import subprocess


def solve(series, verbose=False, log_pulse=False, log_pulse_filename="pulse_log.temp"):
    command = [config.solve_command, '-D' + str(series['noise intensity'])]

    if 'time delay' in series:
        command.append('-T' + str(series['time delay']))
        if 'partial time delay probability' in series:
            command.append('-P' + str(series['partial time delay probability']))
    elif 'normal distribution mean' in series:
        command.append('-N' + str(series['normal distribution mean']))

    if verbose:
        command.append('-v')
    if log_pulse:
        command.append('--log_pulse')
        command.append(log_pulse_filename)

    detail = subprocess.check_output(command).decode()
    result = re.search(r'(?=\n?).*$', detail).group().split(' ')
    result = [float(x) for x in result]
    return dict(zip(['Q', 'Qsin', 'Qcos', 'ISI'], result)), detail
