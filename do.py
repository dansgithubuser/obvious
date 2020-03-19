#!/usr/bin/env python

import argparse
import os
import shutil
import subprocess

parser=argparse.ArgumentParser()
parser.add_argument('--test', '-t', action='store_true')
parser.add_argument('--package', '-p', action='store_true')
args=parser.parse_args()

LOC=os.path.dirname(os.path.realpath(__file__))
os.chdir(LOC)

if args.test:
	subprocess.check_call('python test/path.py', shell=True)
	from obvious import Path
	Path('test').cd()
	subprocess.check_call('cmake .', shell=True)
	subprocess.check_call('cmake --build .', shell=True)
	subprocess.check_call(Path(r=r'.*ObviousCppTest(\.exe)?$').p, shell=True)
	os.chdir(LOC)

if args.package:
	shutil.copyfile('obvious.py', os.path.join('package', 'py', 'obvious', 'obvious.py'))
	os.chdir(os.path.join('package', 'py'))
	subprocess.check_call('python3 setup.py sdist', shell=True)
	subprocess.check_call('twine upload dist/*', shell=True)
