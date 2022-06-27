#include<iostream>
#include<Windows.h>
#include"EngineManager.h"


using namespace std;


#ifdef _DEBUG
int main() {
#else
#include<Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	bool Result = true;
	try {
		EngineManager& engineManager = EngineManager::GetInstance();

		//������
		Result = engineManager.initializeManager();

		//���s
		Result &= engineManager.MainLoopProcess();
	}
	catch (bad_alloc) {
		cout << "�������ߋ��ێ��ɃG���[���������܂���" << endl;
		Result = false;
	}

	if (!Result) {
		cout << "�����I�����܂���" << endl;
		return -1;
	}
	cout << "����I��" << endl;
	return 0;
}
