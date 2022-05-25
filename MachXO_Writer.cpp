// MachXO_Writer.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <chrono>
#include <thread>
#include "MachXO_if.h"

int main( int argc, char * argv[] )
{
	MachXO  iMachXO;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	uint32_t	DeviceID = iMachXO.readDeviceID();

	printf("\n");
	printf("-------------------------------------\n");
	printf("     MachXO configuration writer     \n");
	printf("-------------------------------------\n");
	printf("\n");
	printf("DeviceID    = 0x%08x\n", iMachXO.readDeviceID());
	printf("UserCode    = 0x%08x\n", iMachXO.readUserCode());
	printf("Status      = 0x%08x\n", iMachXO.readStatus());
	printf("FeatureBits =     0x%04x\n", iMachXO.readFeatureBits());
	printf("\n");

	if (2 <= argc)
	{
		std::ifstream   ifs(argv[1]);

		if (ifs.fail())
		{
			printf("[ERROR] HEX file open error, %s\n", argv[1]);
			return	-1;
		}

		if (DeviceID == 0)
		{
			printf("[ERROR] Illigal Device ID 0x%08x\n", DeviceID );
			return	-1;
		}

		printf("Entering Offline Configuration\n");
		iMachXO.enableConfigOffline();
		printf("Erasing Configuration and UFM\n");
		iMachXO.erase(MachXO::MACHXO_ERASE_CONFIG_FLASH | MachXO::MACHXO_ERASE_UFM);
		printf("Waiting for erase to complete\n");
		iMachXO.waitBusy();
		printf("Loading HEX\n");
		iMachXO.loadHex(ifs);
		printf("Programming DONE status bit\n");
		iMachXO.programDone();
		printf("Refreshing image\n");
		iMachXO.refresh();

		printf("\n");
		printf("DONE.\n");
	}

	return	0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
