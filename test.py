#!/usr/bin/env python

import os, subprocess
os.chdir(os.path.dirname(os.path.realpath(__file__)))

subprocess.check_call('python test/path.py', shell=True)
from obvious import Path

Path('test').cd()
subprocess.check_call('cmake .', shell=True)
subprocess.check_call('cmake --build .', shell=True)
subprocess.check_call(Path(r=r'.*ObviousCppTest(\.exe)?$').p, shell=True)
