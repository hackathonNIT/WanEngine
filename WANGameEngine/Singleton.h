#pragma once

//シングルトンクラス
template<class T>
class Singleton {
	T& operator=(const Singleton<T>&);
	Singleton(const Singleton<T>&);
protected:
	Singleton() {};
	virtual ~Singleton() {};
public:
	static T& GetInstance() {
		static T ins;
		return ins;
	}
};

