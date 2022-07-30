#pragma once

template<typename T>
class Singleton
{
public:
	// 프로그램 종료시까지 메모리 해제가 필요없는 클래스에 한해 상속 받게 한다.
	// 이렇게 하면 전역 객체의 소멸 순서에 의존하지 않게 된다.
	static T& Get()
	{
		static T* instance = new T();
		return *instance;
	}

protected:
	Singleton() = default;
};
