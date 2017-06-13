#!/usr/bin/env python

#=====Python 2/3 compatibility=====#
try: input=raw_input
except: pass

#=====peer repo management=====#
def ensure_repo(path, name):
	import os, subprocess
	if not os.path.exists(os.path.join(path, name)):
		start=os.getcwd()
		os.chdir(path)
		print("I need to clone {} repo. I'll do so in {}. Is this OK? (y/n)".format(name, os.getcwd()))
		if input()!='y': raise Exception('user disallowed {} clone'.format(name))
		subprocess.check_call('git clone '.format(url), shell=True)
		os.chdir(start)

#=====Python 2/3 path stuff=====#
import os

class Path:
	def __init__(self, s=None, p=None, g=None, r=None):
		if s: self.p=os.path.join(*[i if i else '/' for i in s.split('/')])
		elif p: self.p=p
		elif g:
			import glob
			x=glob.glob(g)
			assert(len(x)==1)
			self.p=x[0]
		elif r:
			sep=os.path.sep
			if sep=='\\': sep='\\\\'
			r=r.replace('/', sep)
			import re
			for root, dirs, files in os.walk('.'):
				for i in dirs+files:
					x=os.path.join(root, i)
					if re.search(r, x):
						assert not hasattr(self, 'p')
						self.p=x
		else: self.p=os.getcwd()
		if hasattr(self, 'p'): self.p=os.path.realpath(self.p)

	def __nonzero__(self):
		if not hasattr(self, 'p'): return False
		return os.path.exists(self.p)

	def __bool__(self): return self.__nonzero__()

	def __str__(self): return self.p

	def __add__(self, s): return self.join(s)

	def join(self, s): return Path(os.path.join(self.p, Path(s).p))

	def cd(self): os.chdir(self.p)

	def parent(self, n=1):
		r=self.p
		for i in range(n): r=os.path.split(r)[0]
		return r
