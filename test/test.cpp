#include "../cpp/obvious.hpp"

int main(){
	const std::vector<int> v0{0};
	const std::vector<int> v0123{0, 1, 2, 3};
	const std::vector<int> v123{1, 2, 3};
	//test stuff
	std::cout<<"test stuff\n";
	{
		bool ok=false;
		try{ EXPECT(true, false) }
		catch(const std::exception& e){ ok=true; }
		if(!ok) throw std::logic_error("expectations aren't working");
	}
	//typical vector operations
	std::cout<<"typical vector operations\n";
	{
		EXPECT(index(2, v123), 1)
		EXPECT(index(4, v123), 3)
		EXPECT(in(1, v123), true)
		EXPECT(in(5, v123), false)
		auto v=v123;
		erase(1, v);
		erase(5, v);
		EXPECT(in(1, v), false)
		EXPECT(in(2, v), true)
		EXPECT(in(3, v), true)
	}
	//typical map operations
	std::cout<<"typical map operations\n";
	{
		const std::map<int, std::string> m{{1, "a"}, {2, "b"}, {3, "c"}};
		auto k=keys(m);
		EXPECT(k, v123);
		EXPECT(MAP_GET(m, 1, "z"), "a")
		EXPECT(MAP_GET(m, 4, "z"), "z")
	}
	//typical string operations
	std::cout<<"typical string operations\n";
	{
		std::string s("a man a plan a canal panama");
		std::string r=s;
		replace(r, " a ", "a");
		EXPECT(r, "a manaplanacanal panama")
		EXPECT(startsWith(s, ""), true)
		EXPECT(startsWith(s, "a man"), true)
		EXPECT(startsWith(s, "banana"), false)
		std::stringstream ss(s);
		EXPECT(peek(ss, 5), "a man")
		EXPECT(peek(ss, 5), "a man")
		EXPECT(read(ss, 5), "a man")
		EXPECT(read(ss, 5), " a pl")
		EXPECT(read(ss, 80), "an a canal panama")
		EXPECT(read(ss, 5), "")
		EXPECT(peek(ss, 5), "")
	}
	{
		std::stringstream ss(" \n a ");
		EXPECT(more(ss), true)
		std::string s;
		ss>>s;
		EXPECT(more(ss), false)
	}
	//print/parse
	std::cout<<"print/parse\n";
	{
		std::vector<std::string> v{"a", "b", "c"};
		std::map<std::string, std::string> m{{"a", "A"}, {"b", "B"}, {"c", "C"}};
		std::vector<uint8_t> b{0, 1, 2};
		std::stringstream ss;
		ss<<v<<" "<<m<<" "<<b;
		decltype(v) v2;
		decltype(m) m2;
		decltype(b) b2;
		ss>>v2;
		ss>>" ";
		ss>>m2;
		ss>>" ";
		ss>>b2;
		EXPECT(v, v2)
		EXPECT(m, m2)
		EXPECT(b, b2)
	}
	//vector operator+= overload
	std::cout<<"vector operator+= overload\n";
	{
		auto v=v0;
		v+=v123;
		EXPECT(v, v0123);
	}
	//set operator+= overload
	std::cout<<"set operator+= overload\n";
	{
		const std::set<int> s123{1, 2, 3};
		std::set<int> s{0, 1};
		s+=s123;
		std::vector<int> v;
		for(const auto& i: s) v.push_back(i);
		EXPECT(v, v0123);
	}
	//vector operator+ overload
	std::cout<<"vector operator+ overload\n";
	{
		auto v=v0+v123;
		EXPECT(v, v0123);
	}
	//min and max
	std::cout<<"min and max\n";
	{
		EXPECT(OBV_MIN(1, 2), 1);
		EXPECT(OBV_MAX(1, 2), 2);
		int a=1, b=2, c=3;
		OBV_MINI(a, b); EXPECT(a, 1)
		OBV_MINI(b, a); EXPECT(b, 1)
		OBV_MAXI(c, a); EXPECT(c, 3)
		OBV_MAXI(a, c); EXPECT(a, 3)
	}
	//obvious if
	std::cout<<"obvious if\n";
	{
		std::vector<int> v;
		for(unsigned i=0; i<5; ++i, OBV_IF(i%2, v.push_back(i)));
		std::vector<int> v135{1, 3, 5};
		EXPECT(v, v135)
	}
	//list comprehensions
	std::cout<<"list comprehensions\n";
	{
		std::vector<int> v13{1, 3};
		EXPECT(OBV_FOR(v123, if(*i%2) r.push_back(*i), std::vector<int>()), v13)
		EXPECT(OBV_FOR(v123, OBV_MAXI(r, *i), 0), 3)
		EXPECT(OBV_FOR2(v123, r&=*a+1==*b, true), true)
		EXPECT(OBV_FOR2(v123, r&=*a==*b, true), false)
	}
	//Cartesian coordinates
	std::cout<<"Cartesian coordinates\n";
	{
		std::vector<CartesianPair<int>> p{{3, 4}, {6, 7}, {3, 5}};
		EXPECT(p[1].x, 6) EXPECT(p[1].y, 7)
		std::sort(p.begin(), p.end());
		std::vector<CartesianPair<int>> p2{{3, 4}, {3, 5}, {6, 7}};
		EXPECT(p, p2)
	}
	//bytes
	std::cout<<"bytes\n";
	{
		int x=3;
		auto b=bytes(0, x);
		std::vector<uint8_t> v{0, 3};
		EXPECT(b, v)
	}
	//slices
	std::cout<<"slices\n";
	{
		auto s1=slice(v123);
		EXPECT(s1.size(), 3)
		int a[5]={1, 2, 3};
		EXPECT(memcmp(s1.ptr(), a, 3), 0)
		auto s2=slice(v123, 2);
		EXPECT(s2.size(), 2)
		EXPECT(memcmp(s2.ptr(), a, 2), 0)
		auto s3=slice(v123, 1, 3);
		EXPECT(s3.size(), 2)
		EXPECT(memcmp(s3.ptr(), a+1, 2), 0)
		EXPECT(slice(v0123, 1, 4)==s1, true)
		EXPECT(slice(v0123, 1, 4)!=s1, false)
		EXPECT(slice(v123)!=v123, false)
		auto b=bytes('a', 'b', 'c');
		EXPECT(slice(b)!=std::string("abc"), false)
		std::vector<int> v{1, 2, 3, 1, 2};
		EXPECT(s1+s2==v, true)
		std::vector<int> u{1, 2, 3, 0};
		EXPECT(s1+v0==u, true)
		EXPECT(slice(std::string("abc"))!=slice(b), false)
		std::vector<int> t{1, 2, 3};
		t+=s2;
		EXPECT(t, v)
	}
	//
	return 0;
}
