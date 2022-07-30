#pragma once

template<typename T>
class Singleton
{
public:
	// ���α׷� ����ñ��� �޸� ������ �ʿ���� Ŭ������ ���� ��� �ް� �Ѵ�.
	// �̷��� �ϸ� ���� ��ü�� �Ҹ� ������ �������� �ʰ� �ȴ�.
	static T& Get()
	{
		static T* instance = new T();
		return *instance;
	}

protected:
	Singleton() = default;
};
