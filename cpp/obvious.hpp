/*
obvious stuff
in the sense that you obviously want this
and it's obvious conceptually
but it's not necessarily obvious to make into convenient C++...
*/

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//=====debug stuff=====//
#define OBV_LOUD(X)\
	std::cout<<"/===="<<#X<<"====\\\n";\
	std::cout<<X<<"\n";\
	std::cout<<"\\===="<<#X<<"====/\n";

//=====test stuff=====//
#define EXPECT(ACTUAL, EXPECTED){\
	std::stringstream ss;\
	ss<<__FILE__<<"@"<<__LINE__<<": expected " #ACTUAL " to be "<<(EXPECTED)<<" but got "<<(ACTUAL);\
	if((EXPECTED)!=(ACTUAL)) throw std::logic_error(ss.str());\
}

//=====typical operations=====//
//-----typical container operations-----//
template<typename T, typename U> unsigned index(const T& value, const U& container){
	return std::find(container.begin(), container.end(), value)-container.begin();
}

template<typename T, typename U> bool in(const T& value, const U& container){
	return index(value, container)!=container.size();
}

template<typename T, typename U> void erase(const T& value, U& container){
	auto i=std::find(container.begin(), container.end(), value);
	if(i!=container.end()) container.erase(i);
}

//-----typical map operations-----//
template<typename T, typename U> std::vector<T> keys(const std::map<T, U>& map){
	std::vector<T> result;
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
std::ostream& operator<<(std::ostream& o, uint8_t c){
	std::stringstream ss;
	ss<<std::hex<<std::setfill('0')<<std::setw(2)<<(unsigned)c;
	return o<<ss.str();
}

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
#define OBV_PLUS_EQUALS_BASE(CONTAINER1, CONTAINER2, F)\
	template<typename T> void operator+=(CONTAINER1<T>& r, const CONTAINER2<T>& a){\
		for(auto& i: a) F;\
	}

#define OBV_PLUS_EQUALS_SET(CONTAINER)\
	OBV_PLUS_EQUALS_BASE(std::set, CONTAINER, r.insert(i))

#define OBV_PLUS_EQUALS_VECTOR(CONTAINER)\
	OBV_PLUS_EQUALS_BASE(std::vector, CONTAINER, r.push_back(i))

#define OBV_PLUS_EQUALS(CONTAINER)\
	OBV_PLUS_EQUALS_SET(CONTAINER)\
	OBV_PLUS_EQUALS_VECTOR(CONTAINER)

OBV_PLUS_EQUALS(std::set)
OBV_PLUS_EQUALS(std::vector)

void operator+=(std::vector<uint8_t>& a, const std::string& b){
	a.insert(a.end(), b.data(), b.data()+b.size());
}

//-----operator+ overloads-----//
template<typename T> std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b){
	std::vector<T> r=a;
	r.insert(r.end(), b.begin(), b.end());
	return r;
}

//=====min and max=====//
#define OBV_MIN(X, Y) (X<Y?X:Y)
#define OBV_MAX(X, Y) (X>Y?X:Y)

#define OBV_MINI(X, Y) X=OBV_MIN(X, Y)
#define OBV_MAXI(X, Y) X=OBV_MAX(X, Y)

//=====expressions=====//
#define OBV_IF(PREDICATE, ACTION) (PREDICATE?((ACTION), 0):0)

bool toss(const char* message){ throw std::runtime_error(message); return false; }

//-----list comprehensions-----//
#define OBV_FOR(CONTAINER, F, INITIAL)[&](){\
	auto r=INITIAL;\
	for(auto i=CONTAINER.begin(); i!=CONTAINER.end(); ++i) F;\
	return r;\
}()

#define OBV_FOR2(CONTAINER, F, INITIAL)[&](){\
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
	bool operator==(const Pair& other) const{
		return x==other.x&&y==other.y;
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
	return Slice<T>(v.data(), v.size());
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned size){
	Slice<T> r(v.data(), size);
	r.checkSize(v);
	return r;
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned start, unsigned finish){
	if(start>finish) throw std::logic_error("bad slice -- start after finish");
	Slice<T> r(v.data()+start, finish-start);
	r.checkSize(v);
	return r;
}
Slice<uint8_t> slice(const std::string& s);

template<typename T> class Slice{
	public:
		Slice(const T* ptr, std::size_t size): _ptr(ptr), _size(size) {}
		operator std::vector<T>() const { return std::vector<T>(_ptr, _ptr+_size); }
		bool operator==(Slice other) const { return _size==other._size&&!memcmp(_ptr, other._ptr, _size); }
		bool operator!=(Slice other) const { return !(*this==other); }
		bool operator!=(const std::vector<T>& other) const { return *this!=slice(other); }
		bool operator!=(const std::string& other) const { return *this!=slice(other); }
		std::vector<T> operator+(Slice other){
			std::vector<T> r(_ptr, _ptr+_size);
			r.insert(r.end(), other._ptr, other._ptr+other._size);
			return r;
		}
		std::vector<T> operator+(const std::vector<T>& other){
			return *this+slice(other);
		}
		void checkSize(const std::vector<T>& v) const {
			if(_size>v.size()) throw std::logic_error("bad slice -- size too large");
		}
		const T* ptr() const { return _ptr; }
		std::size_t size() const { return _size; }
	private:
		const T* _ptr;
		const std::size_t _size;
};

Slice<uint8_t> slice(const std::string& s){
	return Slice<uint8_t>((uint8_t*)s.data(), s.size());
}

template<typename T> void operator+=(std::vector<T>& a, Slice<T> b){
	a.insert(a.end(), b.ptr(), b.ptr()+b.size());
}