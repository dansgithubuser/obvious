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

	def __nonzero__(self):
		if not hasattr(self, 'p'): return False
		return os.path.exists(self.p)
	def __bool__(self): return self.__nonzero__()

	def __str__(self): return self.p
	def __add__(self, s): return self.join(s)
	def join(self, s): return Path(os.path.join(self.p, Path(s).p))
	def cd(self): os.chdir(self.p)
	def real(self): return os.path.realpath(self.p)
	def parent(self): return Path(os.path.split(self.p)[0])
