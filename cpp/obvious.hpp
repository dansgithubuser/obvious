#ifndef OBVIOUS_HPP_INCLUDED
#define OBVIOUS_HPP_INCLUDED

/*
obvious stuff
in the sense that you obviously want this
and it's obvious conceptually
but it's not necessarily obvious to make into convenient C++...
*/

#include <algorithm>
#include <climits>
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
#define OBV_EXPECT(ACTUAL, EXPECTED){\
	auto ACTUAL_VALUE=(ACTUAL);\
	auto EXPECTED_VALUE=(EXPECTED);\
	if(EXPECTED_VALUE!=ACTUAL_VALUE){\
		std::stringstream ss;\
		ss<<__FILE__<<"@"<<__LINE__<<": expected " #ACTUAL " to be "<<::str(EXPECTED_VALUE)<<" but got "<<::str(ACTUAL_VALUE);\
		throw std::logic_error(ss.str());\
	}\
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

template <typename T, typename U> void eraseKey(const T& key, U& map){
	auto i=map.find(key);
	if(i!=map.end()) map.erase(i);
}

template <typename T, typename U> U& rekey(std::map<T, U>& map, const T& keyI, const T& keyF){
	U& r=map[keyF]=map.at(keyI);
	map.erase(keyI);
	return r;
}

#define MAP_GET(M, I, D) (M.count(I)?M.at(I):D)

//-----typical string operations-----//
static inline std::string& replace(std::string& s, const std::string& a, const std::string& b){
	size_t i=0;
	while(true){
		i=s.find(a, i);
		if(i==std::string::npos) break;
		s.replace(i, a.size(), b);
		i+=b.size();
	}
	return s;
}

static inline std::string replace(const std::string& s, const std::string& a, const std::string& b){
	std::string r=s;
	return replace(r, a, b);
}

static inline bool startsWith(const std::string& s, const std::string& t){
	for(unsigned i=0; i<t.size(); ++i){
		if(i>=s.size()) return false;
		if(s[i]!=t[i]) return false;
	}
	return true;
}

static inline std::string read(std::istream& istream, unsigned size){
	std::vector<char> result(size+1, '\0');
	istream.read(result.data(), size);
	return std::string(result.data());
}

static inline std::string peek(std::istream& istream, unsigned size){
	auto position=istream.tellg();
	auto result=read(istream, size);
	istream.seekg(position);
	return result;
}

static inline bool more(std::istream& istream){
	auto position=istream.tellg();
	std::string s;
	bool result=false;
	if(istream>>s) result=true;
	istream.seekg(position);
	return result;
}

//=====printing=====//
template<typename T, typename U> struct _ObvKeyValuePairConst{
	_ObvKeyValuePairConst(const T& t, const U& u): key(t), value(u) {}
	const T& key;
	const U& value;
};

//-----generic-----//
template<
	typename T,
	typename std::enable_if<!std::is_pointer<T>::value, int>::type=0,
	typename std::enable_if<!std::is_class<T>::value, int>::type=0
> std::string _obvSerialize(T t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

template<
	typename T,
	typename std::enable_if<!std::is_pointer<T>::value, int>::type=0,
	typename std::enable_if<std::is_class<T>::value, int>::type=0
> std::string _obvSerialize(T t){
	return t.str();
}

template<
	typename T,
	typename std::enable_if<std::is_pointer<T>::value, int>::type=0
> std::string _obvSerialize(T t){
	return t->str();
}

//-----specific-----//
static inline std::string _obvSerialize(const std::string& s){
	std::stringstream ss;
	replace(s, "\"", "\\\"");
	replace(s, "\\", "\\\\");
	ss<<'"'<<s<<'"';
	return ss.str();
}

static inline std::string _obvSerialize(const char* s){
	return s;
}

static inline std::string _obvSerialize(char* s){
	return s;
}

static inline std::string _obvSerialize(const void* v){
	std::stringstream ss;
	ss<<v;
	return ss.str();
}

static inline std::string _obvSerialize(void* v){
	return _obvSerialize((const void*)v);
}

static inline std::string _obvSerialize(uint8_t c){
	return _obvSerialize((unsigned)c);
}

//-----container-----//
template<typename T> std::string _obvSerialize(const std::vector<T>& c);
template<typename T> std::string _obvSerialize(const std::set<T>& c);
template<typename T, typename U> std::string _obvSerialize(const _ObvKeyValuePairConst<T, U>& p);
template<typename T, typename U> std::string _obvSerialize(const std::map<T, U>& c);
template<typename T, typename U> std::string _obvSerialize(const std::pair<T, U>& p);

template<typename T> std::string _obvSerializeContainer(
	const T& t,
	std::string prefix,
	std::string open="<",
	std::string close=">"
){
	//figure out if big or not
	bool big=false;
	{
		std::stringstream ss;
		for(const auto& i: t) ss<<_obvSerialize(i)<<", ";
		if(in('\n', ss.str())||ss.str().size()>72) big=true;
	}
	//meat
	std::stringstream ss;
	ss<<prefix<<open;
	if(big) ss<<"\n";
	bool first=true;
	for(const auto& i: t){
		if(!first) ss<<","<<(big?"\n":" ");
		first=false;
		std::string s=_obvSerialize(i);
		if(big){
			s="\t"+s;
			replace(s, "\n", "\n\t");
		}
		ss<<s;
	}
	if(big) ss<<"\n";
	ss<<close;
	return ss.str();
}

template<typename T> std::string _obvSerialize(const std::vector<T>& c){
	return _obvSerializeContainer(c, "", "[", "]");
}

template<typename T> std::string _obvSerialize(const std::set<T>& c){
	return _obvSerializeContainer(c, "s");
}

template<typename T, typename U> std::string _obvSerialize(const _ObvKeyValuePairConst<T, U>& p){
	std::stringstream ss;
	ss<<_obvSerialize(p.key)<<": "<<_obvSerialize(p.value);
	return ss.str();
}

template<typename T, typename U> std::string _obvSerialize(const std::map<T, U>& c){
	std::vector<_ObvKeyValuePairConst<T, U>> x;
	for(const auto& i: c) x.push_back(_ObvKeyValuePairConst<T, U>(i.first, i.second));
	return _obvSerializeContainer(x, "", "{", "}");
}

template<typename T, typename U> std::string _obvSerialize(const std::pair<T, U>& p){
	std::stringstream ss;
	ss<<"["<<_obvSerialize(p.first)<<", "<<_obvSerialize(p.second)<<"]";
	return ss.str();
}

//-----interface-----//
template<typename T> void _obvStream(std::stringstream& ss, T t){
	ss<<_obvSerialize(t);
}

template<typename T, typename... Ts> void _obvStream(std::stringstream& ss, T t, Ts... ts){
	ss<<_obvSerialize(t)<<" ";
	_obvStream(ss, ts...);
}

template<typename... Ts> std::string str(Ts... ts){
	std::stringstream ss;
	_obvStream(ss, ts...);
	return ss.str();
}

template<typename T> void _obvStreamSmall(std::stringstream& ss, T t){
	ss<<_obvSerialize(t);
}

template<typename T, typename... Ts> void _obvStreamSmall(std::stringstream& ss, T t, Ts... ts){
	ss<<_obvSerialize(t);
	_obvStream(ss, ts...);
}

template<typename... Ts> std::string strs(Ts... ts){
	std::stringstream ss;
	_obvStreamSmall(ss, ts...);
	return ss.str();
}

//=====parsing=====//
template<typename T, typename U> struct _ObvKeyValuePair{
	T key;
	U value;
};

//-----generic-----//
template<
	typename T,
	typename std::enable_if<!std::is_pointer<T>::value, int>::type=0,
	typename std::enable_if<!std::is_class<T>::value, int>::type=0
> void _obvDeserialize(std::stringstream& ss, T& t){
	ss>>t;
}

template<
	typename T,
	typename std::enable_if<!std::is_pointer<T>::value, int>::type=0,
	typename std::enable_if<std::is_class<T>::value, int>::type=0
> void _obvDeserialize(std::stringstream& ss, T& t){
	t.dstr(ss);
}

template<
	typename T,
	typename std::enable_if<std::is_pointer<T>::value, int>::type=0
> void _obvSerialize(std::stringstream& ss, T& t){
	t->dstr(ss);
}

//-----specific-----//
static inline void _obvDeserialize(std::stringstream& ss, std::string& s){
	if(read(ss, 1)!="\"") throw std::runtime_error("std::string doesn't start with \"");
	while(true){
		auto next=read(ss, 1);
		if(next=="\"") break;
		if(next=="\\") next=read(ss, 1);
		s+=next;
	}
}

static inline void _obvDeserialize(std::stringstream& ss, const char* c){
	std::string s(c);
	if(read(ss, s.size())!=s) throw std::runtime_error("bad serialization");
}

static inline void _obvDeserialize(std::stringstream& ss, uint8_t& c){
	unsigned u;
	_obvDeserialize(ss, u);
	c=(uint8_t)u;
}

//-----container-----//
template<typename T> void _obvDeserialize(std::stringstream& ss, std::vector<T>& c);
template<typename T> void _obvDeserialize(std::stringstream& ss, std::set<T>& c);
template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, _ObvKeyValuePair<T, U>& p);
template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, std::map<T, U>& c);
template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, std::pair<T, U>& p);

template<typename T> void _obvAppendFromSerialized(std::vector<T>& c, std::stringstream& ss){
	T t;
	_obvDeserialize(ss, t);
	c.push_back(t);
}

template<typename T> void _obvAppendFromSerialized(std::set<T>& c, std::stringstream& ss){
	T t;
	_obvDeserialize(ss, t);
	c.insert(t);
}

template<typename T> std::istream& _obvDeserializeContainer(
	std::stringstream& ss,
	T& t,
	std::string prefix,
	std::string open="<",
	std::string close=">"
){
	std::string s;
	s=read(ss, prefix.size());
	if(s!=prefix) throw std::runtime_error("container prefix is \""+s+"\" instead of \""+prefix+"\"");
	s=read(ss, 1);
	if(s!=open) throw std::runtime_error("container open delimiter is \""+s+"\" instead of \""+open+"\"");
	while(true){
		ss>>std::ws;
		s=peek(ss, 1);
		if(s==close){
			read(ss, 1);
			break;
		}
		if(s==","){ read(ss, 1); ss>>std::ws; }
		_obvAppendFromSerialized(t, ss);
	}
}

template<typename T> void _obvDeserialize(std::stringstream& ss, std::vector<T>& c){
	_obvDeserializeContainer(ss, c, "", "[", "]");
}

template<typename T> void _obvDeserialize(std::stringstream& ss, std::set<T>& c){
	_obvDeserializeContainer(ss, c, "s");
}

template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, _ObvKeyValuePair<T, U>& p){
	_obvDeserialize(ss, p.key);
	_obvDeserialize(ss, ": ");
	_obvDeserialize(ss, p.value);
}

template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, std::map<T, U>& c){
	std::vector<_ObvKeyValuePair<T, U>> x;
	_obvDeserializeContainer(ss, x, "", "{", "}");
	for(const auto& i: x) c[i.key]=i.value;
}

template<typename T, typename U> void _obvDeserialize(std::stringstream& ss, std::pair<T, U>& p){
	_obvDeserialize(ss, "(");
	_obvDeserialize(ss, p.first);
	_obvDeserialize(ss, ", ");
	_obvDeserialize(ss, p.second);
	_obvDeserialize(ss, ")");
}

//-----interface-----//
template<typename T> void _obvDestream(std::stringstream& ss, T& t){
	_obvDeserialize(ss, t);
}

template<typename T, typename... Ts> void _obvDestream(std::stringstream& ss, T& t, Ts&... ts){
	_obvDeserialize(ss, t);
	_obvDeserialize(ss, " ");
	_obvDestream(ss, ts...);
}

template<typename... Ts> void dstr(std::stringstream& ss, Ts&... ts){
	_obvDestream(ss, ts...);
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

static inline void operator+=(std::vector<uint8_t>& a, const std::string& b){
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

static inline bool toss(const char* message){ throw std::runtime_error(message); return false; }

#define OBV_FOR_MAP(MAP, F){\
	size_t i=0;\
	for(const auto& j: MAP){\
		const auto& k=j.first;\
		const auto& v=j.second;\
		F;\
		++i;\
	}\
}

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
template<typename T> struct CartesianPair{
	CartesianPair(){}
	CartesianPair(T x, T y): x(x), y(y) {}
	bool operator<(const CartesianPair& other) const{
		if(x<other.x) return true;
		if(x>other.x) return false;
		if(y<other.y) return true;
		return false;
	}
	bool operator==(const CartesianPair& other) const{
		return x==other.x&&y==other.y;
	}
	std::string str() const{
		return ::str("[", x, ", ", y, "]");
	}
	void dstr(std::stringstream& ss){
		ss>>x>>y>>" ";
	}
	T x, y;
};

//-----bytes-----//
typedef std::vector<uint8_t> Bytes;

static inline Bytes bytes(){ return Bytes(); }

template<typename T, typename...Ts> Bytes bytes(T byte, Ts...args){
	if(byte<0||byte>=0x100) throw std::logic_error("invalid byte");
	Bytes r;
	r.push_back(byte);
	for(auto i: bytes(args...)) r.push_back(i);
	return r;
}

//=====slices=====//
template<typename T> class Slice;

template<typename T> Slice<T> slice(const std::vector<T>& v){
	return Slice<T>(v.data(), v.size());
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned size){
	Slice<T> r(v.data(), OBV_MIN(size, v.size()));
	return r;
}
template<typename T> Slice<T> slice(const std::vector<T>& v, unsigned start, unsigned size){
	if(start>v.size()) throw std::logic_error("bad slice -- start too far");
	Slice<T> r(v.data()+start, OBV_MIN(size, v.size()-start));
	return r;
}
static inline Slice<uint8_t> slice(const std::string& s);

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
		const T* ptr() const { return _ptr; }
		std::size_t size() const { return _size; }
		std::string str() const { return ::str(std::vector<T>(*this)); }
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

#endif
