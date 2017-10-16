/*
obvious stuff
in the sense that you obviously want this
and it's obvious conceptually
but it's not necessarily obvious to make into convenient C++...
*/

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//=====debug stuff=====//
#define OBVIOUS_LOUD(X)\
	std::cout<<"/===="<<#X<<"====\\\n";\
	std::cout<<X<<"\n";\
	std::cout<<"\\===="<<#X<<"====/\n";

//=====typical operations=====//
//-----typical container operations-----//
template<typename T, typename U> unsigned index(const T& value, const U& container){
	return std::find(container.begin(), container.end(), value)-container.begin();
}

template<typename T, typename U> bool in(const T& value, const U& container){
	return index(value, container)!=container.size();
}

template<typename T, typename U> void erase(const T& value, U& container){
	container.erase(std::find(container.begin(), container.end(), value));
}

//-----typical map operations-----//
template<typename T, typename U> std::vector<const T&> keys(const std::map<T, U>& map){
	std::vector<const T&> result;
	for(const auto& i: map) result.push_back(i.first);
	return result;
}

#define MAP_GET(M, I, D) (M.count(I)?M.at(I):D)

//-----typical string operations-----//
static void replace(std::string& s, const std::string& a, const std::string& b){
	size_t i=0;
	while(true){
		i=s.find(a, i);
		if(i==std::string::npos) break;
		s.replace(i, a.size(), b);
		i+=b.size();
	}
}

//=====printing=====//
//-----container printing-----//
template<typename T> std::ostream& streamContainer(std::ostream& o, const T& t, std::string prefix){
	//figure out if big or not
	bool big=false;
	{
		std::stringstream ss;
		for(const auto& i: t) ss<<i<<", ";
		if(in('\n', ss.str())||ss.str().size()>72) big=true;
	}
	//meat
	o<<prefix<<"{";
	if(big) o<<"\n";
	bool first=true;
	for(const auto& i: t){
		if(!big&&!first) o<<", ";
		first=false;
		std::stringstream ss;
		ss<<i;
		std::string s=ss.str();
		if(big){
			s="\t"+s;
			replace(s, "\n", "\n\t");
		}
		o<<s;
		if(big) o<<",\n";
	}
	o<<"}";
	return o;
}

template<typename T> std::ostream& operator<<(std::ostream& o, const std::vector<T>& c){
	return streamContainer(o, c, "v");
}

template<typename T> std::ostream& operator<<(std::ostream& o, const std::set<T>& c){
	return streamContainer(o, c, "s");
}

template<typename T, typename U> struct KeyValuePair{
	KeyValuePair(const T& t, const U& u): key(t), value(u) {}
	const T& key;
	const U& value;
};
template<typename T, typename U> std::ostream& operator<<(std::ostream& o, const KeyValuePair<T, U>& p){
	return o<<p.key<<": "<<p.value;
}

template<typename T, typename U> std::ostream& operator<<(std::ostream& o, const std::map<T, U>& c){
	std::vector<KeyValuePair<T, U>> x;
	for(const auto& i: c) x.push_back(KeyValuePair<T, U>(i.first, i.second));
	return streamContainer(o, x, "m");
}

template<typename T, typename U> std::ostream& operator<<(std::ostream& o, const std::pair<T, U>& p){
	o<<"("<<p.first<<", "<<p.second<<")";
	return o;
}

//=====operator overloads=====//
//-----operator+= overloads-----//
#define OBVIOUS_PLUS_EQUALS_BASE(CONTAINER1, CONTAINER2, F)\
	template<typename T> void operator+=(CONTAINER1<T>& r, const CONTAINER2<T>& a){\
		for(auto& i: a) F;\
	}

#define OBVIOUS_PLUS_EQUALS_SET(CONTAINER)\
	OBVIOUS_PLUS_EQUALS_BASE(std::set, CONTAINER, r.insert(i))

#define OBVIOUS_PLUS_EQUALS_VECTOR(CONTAINER)\
	OBVIOUS_PLUS_EQUALS_BASE(std::vector, CONTAINER, r.push_back(i))

#define OBVIOUS_PLUS_EQUALS(CONTAINER)\
	OBVIOUS_PLUS_EQUALS_SET(CONTAINER)\
	OBVIOUS_PLUS_EQUALS_VECTOR(CONTAINER)

OBVIOUS_PLUS_EQUALS(std::set)
OBVIOUS_PLUS_EQUALS(std::vector)

void operator+=(std::vector<uint8_t>& a, const std::string& b){
	a.insert(a.end(), b.data(), b.data()+b.size());
}

//-----operator+ overloads-----//
template<typename T> std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b){
	std::vector<T> r=a;
	r.insert(a.end(), b.begin(), b.end());
	return r;
}

//=====min and max=====//
#define OBVIOUS_MIN(X, Y) (X<Y?X:Y)
#define OBVIOUS_MAX(X, Y) (X>Y?X:Y)

#define OBVIOUS_MINI(X, Y) X=OBVIOUS_MIN(X, Y)
#define OBVIOUS_MAXI(X, Y) X=OBVIOUS_MAX(X, Y)

//=====expressions=====//
#define OBVIOUS_IF(PREDICATE, ACTION) (PREDICATE?((ACTION), 0):0)

bool toss(const char* message){ throw std::runtime_error(message); return false; }

//-----list comprehensions-----//
#define OBVIOUS_TRANSFORM(CONTAINER, F, INITIAL)[&](){\
	auto r=INITIAL;\
	for(auto i=CONTAINER.begin(); i!=CONTAINER.end(); ++i) F;\
	return r;\
}()

#define OBVIOUS_BINARY_TRANSFORM(CONTAINER, F, INITIAL)[&](){\
	auto r=INITIAL;\
	auto a=CONTAINER.end();\
	for(auto b=CONTAINER.begin(); b!=CONTAINER.end(); ++b){\
		if(a!=CONTAINER.end()) F;\
		a=b;\
	}\
	return r;\
}()

//=====types=====//
//-----Cartesian coordinates-----//
struct Pair{
	Pair(int x, int y): x(x), y(y) {}
	bool operator<(const Pair& other) const{
		if(x<other.x) return true;
		if(x>other.x) return false;
		if(y<other.y) return true;
		return false;
	}
	int x, y;
};

static std::ostream& operator<<(std::ostream& o, const Pair& p){
	return o<<"("<<p.x<<", "<<p.y<<")";
}

//-----bytes-----//
typedef std::vector<uint8_t> Bytes;

Bytes bytes(){ return Bytes(); }

template<typename T, typename...Ts> Bytes bytes(T byte, Ts...args){
	if(byte<0||byte>=0x100) throw std::logic_error("invalid byte");
	Bytes r;
	r.push_back(byte);
	for(auto i: bytes(args...)) r.push_back(i);
	return r;
}

//=====slices=====//
template<typename T> struct Slice;

template<typename T> Slice<T> slice(const std::vector<T>& v){
	return Slice<T>{v.data(), v.size()};
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned size){
	Slice<T> r{v.data(), v.size()};
	r.checkSize(v);
	return r;
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned start, unsigned finish){
	if(start>finish) throw std::logic_error("bad slice -- start after finish");
	Slice<T> r{v.data()+start, finish-start};
	r.checkSize(v);
	return r;
}
Slice<uint8_t> slice(const std::string& s);

template<typename T> struct Slice{
	operator std::vector<T>() const { return std::vector<T>(_p, _p+_size); }
	bool operator==(Slice other) const { return _size==other._size&&!memcmp(_p, other._p, _size); }
	bool operator!=(Slice other) const { return !(*this==other); }
	bool operator!=(const std::vector<T>& other) const { return *this!=slice(other); }
	bool operator!=(const std::string& other) const { return *this!=slice(other); }
	std::vector<T> operator+(Slice other){
		std::vector<T> r(_p, _p+_size);
		r.insert(r.end(), other._p, other._p+other._size);
		return r;
	}
	std::vector<T> operator+(const std::vector<T>& other){
		return *this+slice(other);
	}
	void checkSize(const std::vector<T>& v) const {
		if(_size>v.size()) throw std::logic_error("bad slice -- size too large");
	}
	const T* _p;
	const std::size_t _size;
};

Slice<uint8_t> slice(const std::string& s){
	return Slice<uint8_t>{(uint8_t*)s.data(), s.size()};
}

template<typename T> void operator+=(std::vector<T>& a, Slice<T> b){
	a.insert(a.end(), b._p, b._p+b._size);
}
