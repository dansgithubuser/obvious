#canonicalize
import os
start_expected=os.getcwd()
home_expected=os.path.dirname(os.path.realpath(__file__))

import sys
sys.path.append(os.path.realpath(os.path.join(home_expected, '..')))

from obvious import Path
start, home=Path.canonicalize(__file__)
assert start.p==start_expected
assert home.p==home_expected
assert home.p==os.getcwd()

#s and p constructors
x=os.path.realpath(os.path.join('foo', 'a', 'x'))
assert Path('foo/a/x').p==x
assert Path(p=x).p==x

#make
Path(p=x).make()
assert os.path.isdir(x)
Path(p=os.path.join(home_expected, 'foo', 'b')).make()

#g and r constructors
assert Path(g='foo/*/x').p==x
assert Path(r='foo/.*/x').p==x

#__nonzero__ (python 2) and __bool__ (python 3)
assert not Path('foo/1')
assert not Path(p=os.path.join('foo', '1'))
assert not Path(g='foo/1')
assert not Path(r='foo/1')

assert Path('foo/a')
assert Path(p=x)
assert Path(g='foo/*/x')
assert Path(r='foo/.*/x')

#cd
Path('foo').cd()
assert os.getcwd()==os.path.join(home_expected, 'foo')

#join
assert home.join('foo/b').p==os.path.join(home_expected, 'foo', 'b')

#parent
assert home.join('foo/b').parent(2).p==home.p
