#!/usr/bin/env python

import ctypes
import datetime
import glob
import os
import platform
import pprint
import re
import subprocess

#=====Python 2/3 compatibility=====#
try: input=raw_input
except: pass

#=====Python 2/3 path stuff=====#
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
			x=glob.glob(g)
			assert(len(x)<=1)
			if len(x)>0: self.p=x[0]
		elif r:
			sep=os.path.sep
			if sep=='\\': sep='\\\\'
			r=r.replace('/', sep)
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
	format='{:%Y-%m'
	if not ambiguous: format+='-%b'
	format+='-%d %H:%M:%S.%f}'
	return format.format(datetime.datetime.now()).lower()

#=====subprocess=====#
def invoke(invocation, capture_stdout=False, silent=False, capture_all=False, asynchronous=False, library_path=None):
	if library_path:
		if platform.system()=='Darwin': invocation='DYLD_LIBRARY_PATH={} {}'.format(library_path, invocation)
		elif platform.system()=='Linux': invocation='LD_LIBRARY_PATH={} {}'.format(library_path, invocation)
	if not silent:
		print('time: '+timestamp())
		print('invoking: '+invocation)
		print('in: '+os.getcwd())
	if asynchronous:
		return subprocess.Popen(invocation, shell=True, stdin=subprocess.PIPE)
	if capture_stdout:
		return subprocess.check_output(invocation, shell=True).decode('utf-8')
	if capture_all:
		p=subprocess.Popen(invocation, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
		p.wait()
		return (p.returncode, p.stdout.read().decode('utf-8'))
	subprocess.check_call(invocation, shell=True)

#=====ctypes=====#
def load_lib(name, paths=['.']):
	attempted_paths=[]
	exceptions=[]
	for base in paths:
		attempted_paths.append(os.path.realpath(base))
		for path in glob.glob(os.path.join(base, '*{}.*'.format(name))):
			file_name=os.path.basename(path)
			if not re.match(r'(lib)?{}\.(so|dylib|dll)$'.format(name), file_name): continue
			try: return ctypes.CDLL(path)
			except Exception as e: exceptions.append(e)
	else:
		raise Exception(
			"couldn't load lib {}, attempted paths:\n{}\nexceptions:\n{}".format(
				name,
				pprint.pformat(attempted_paths),
				pprint.pformat(exceptions),
			)
		)

def set_ffi_types(ff, restype=None, *argtypes):
	conversions={
		bool: ctypes.c_bool,
		int: ctypes.c_int,
		float: ctypes.c_float,
		str: ctypes.c_char_p,
		'void*': ctypes.c_void_p,
	}
	ff.restype=conversions.get(restype, restype)
	ff.argtypes=[conversions.get(i, i) for i in argtypes]

def python_3_string_prep(lib):
	def code_if_str(x, d):
		if type(x)==str and d=='en': return x.encode('utf-8')
		if type(x)==bytes and d=='de': return x.decode('utf-8')
		return x
	class Coder:
		def __init__(self, raw): self.raw=raw
		def __call__(self, *args): return code_if_str(
			self.raw(*[
				code_if_str(i, 'en') for i in args
			]),
			'de',
		)
	for member_name in dir(lib):
		if member_name.startswith('_'): continue
		setattr(lib, member_name, Coder(getattr(lib, member_name)))
