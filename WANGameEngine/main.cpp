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

		//初期化
		Result = engineManager.initializeManager();

		//実行
		Result &= engineManager.MainLoopProcess();
	}
	catch (bad_alloc) {
		cout << "メモリ過去保持にエラーが発生しました" << endl;
		Result = false;
	}

	if (!Result) {
		cout << "強制終了しました" << endl;
		return -1;
	}
	cout << "正常終了" << endl;
	return 0;
}
