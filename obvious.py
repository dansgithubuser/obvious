#!/usr/bin/env python

#=====Python 2/3 compatibility=====#
try: input=raw_input
except: pass

#=====peer repo management=====#
def ensure_repo(path, name, url):
	import os, subprocess
	if not os.path.exists(os.path.join(path, name)):
		start=os.getcwd()
		os.chdir(path)
		print("I need to clone {} repo. I'll do so in {}. Is this OK? (y/n)".format(name, os.getcwd()))
		if input()!='y': raise Exception('user disallowed {} clone'.format(name))
		subprocess.check_call('git clone {}'.format(url), shell=True)
		os.chdir(start)

#=====Python 2/3 path stuff=====#
import os

class Path:
	@staticmethod
	def canonicalize(path):
		start=os.getcwd()
		home=os.path.dirname(os.path.realpath(path))
		os.chdir(home)
		return (Path(start), Path(home))

	def __init__(self, s=None, p=None, g=None, r=None):
		if s:
			s=s.split('/')
			if s[0]=='': s[0]='/'
			self.p=os.path.join(*s)
		elif p: self.p=p
		elif g:
			import glob
			x=glob.glob(g)
			assert(len(x)<=1)
			if len(x)>0: self.p=x[0]
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

	def join(self, s): return Path(os.path.join(self.p, *s.split('/')))

	def cd(self): os.chdir(self.p)

	def parent(self, n=1):
		r=self.p
		for i in range(n): r=os.path.split(r)[0]
		return Path(r)

	def make(self):
		try: os.makedirs(self.p)
		except: pass
		assert os.path.isdir(self.p)

#=====timestamp=====#
def timestamp(ambiguous=True):
	import datetime
	format='{:%Y-%m'
	if not ambiguous: format+='-%b'
	format+='-%d %H:%M:%S.%f}'
	return format.format(datetime.datetime.now()).lower()

#=====subprocess=====#
def invoke(invocation, capture_stdout=False, silent=False, capture_all=False, asynchronous=False, library_path=None):
	if library_path:
		import platform
		if platform.system()=='Darwin': invocation='DYLD_LIBRARY_PATH={} {}'.format(library_path, invocation)
		elif platform.system()=='Linux': invocation='LD_LIBRARY_PATH={} {}'.format(library_path, invocation)
	if not silent:
		print('time: '+timestamp())
		print('invoking: '+invocation)
		print('in: '+os.getcwd())
	import subprocess
	if asynchronous:
		return subprocess.Popen(invocation, shell=True, stdin=subprocess.PIPE)
	if capture_stdout:
		return subprocess.check_output(invocation, shell=True).decode('utf-8')
	if capture_all:
		p=subprocess.Popen(invocation, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
		p.wait()
		return (p.returncode, p.stdout.read().decode('utf-8'))
	subprocess.check_call(invocation, shell=True)
