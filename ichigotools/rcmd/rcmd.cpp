//
// IchigoJam用プログラム保存ツール for Windows
// rcmd コマンド (IchogoJam用 リモート命令文実行コマンド)
//
// 【使い方】
//   rcmd ポート名 命令文
//   例1： rcmd com5 RUN
//   例2： rcmd com5 "PRINT 1+1"
//
// 作成 2015/01/23 by たま吉さん
// 修正 2015/06/14 by たま吉さん, 不具合対応 
// 修正 2016/12/31 by たま吉さん, 出力メッセージの修正
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

#define VAR_STR		"1.07"			// バージョン
#define RD_BUFSIZE	2048			// 受信バッファサイズ
#define SD_BUFSIZE	2948			// 送信バッファサイズ
#define COM_SPEED	115200			// シリアル通信速度
#define BLOCKSIZE	1				// 送信単位

// メインルーチン
int main(int argc, char* argv[]) {
	HANDLE hCom;					// シリアル通信用ハンドル
	int rc;							// 関数戻り値
	char *myname = argv[0];			// 自コマンド名

	// 引数チェック
	if (argc < 3) {
		printf("IchogoJam用 リモート命令文実行コマンド ");
		printf("%s v%s by たま吉さん\n", myname, VAR_STR);
		printf("%s: パラメタの指定が正しくありません。\n", myname);
		printf("\n【使い方】\n %s ポート名 命令文\n", myname);
		printf("\n【使用例】\n %s com5 \"PRINT 1 + 1\"\n", myname);
		exit(1);
	}


	// シリアルポートファイル名
	char com_name[16];
	sprintf_s(com_name, 16, "\\\\.\\%s", argv[1]);

	// 命令文の再定義
	char cmd[1024];
	cmd[0] = '\0';
	for (int i = 2; i < argc; i++) {
		strcat_s(cmd, 1024, argv[i]);
		strcat_s(cmd, 1024, " ");
	}
	strcat_s(cmd, 1024, "\n");

	// シリアルポートオープン
	hCom = CreateFileA(com_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		// ハンドル取得失敗
		printf("%s: ポート[%s]のオープンに失敗しました.\n", myname, com_name);
		exit(1);
	}

	// 送受信バッファサイズの設定
	rc = SetupComm(hCom, RD_BUFSIZE, SD_BUFSIZE);
	if (!rc){
		printf("%s: 送受信バッファの設定に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// 送受信バッファのクリア
	rc = PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!rc){
		printf("%s: 送受信バッファのクリアに失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// 基本通信条件の設定
	DCB dcb;								// デバイス制御ブロック
	GetCommState(hCom, &dcb);				// 現在の設定の取得
	dcb.DCBlength = sizeof(DCB);			// BCDのサイズの取得
	dcb.BaudRate = COM_SPEED;				// 伝送速度
	dcb.fBinary = TRUE;						// バイナリモード
	dcb.ByteSize = 8;						// データサイズ8ビット
	dcb.fParity = NOPARITY;					// パリティ無
	dcb.StopBits = ONESTOPBIT;				// ストップビット１ビット


	//ハードウェアフロー制御
	dcb.fOutxCtsFlow = FALSE;				// CTSハードウェアフロー制御：使用しない
	dcb.fOutxDsrFlow = FALSE;				// DSRハードウェアフロー制御：使用しない
	dcb.fDtrControl = DTR_CONTROL_DISABLE;	// DTR無効
	dcb.fRtsControl = RTS_CONTROL_DISABLE;  // RTS制御無効

	// ソフトウェアフロー制御
	dcb.fOutX = FALSE;						// 送信時XON/OFF制御無し
	dcb.fInX = FALSE;						// 受信時XON/XOFF制御無し
	dcb.fTXContinueOnXoff = TRUE;			// 受信バッファー満杯＆XOFF受信後の継続送信可
	dcb.XonLim = 512;						// XONが送られるまでに格納できる最小バイト数
	dcb.XoffLim = 512;						// XOFFが送られるまでに格納できる最小バイト数
	dcb.XonChar = 0x11;						// 送信時XON文字
	dcb.XoffChar = FALSE;					// XOFF文字指定なし

	//その他
	dcb.fNull = TRUE;						// NULLバイトの破棄をする
	dcb.fAbortOnError = TRUE;				// エラー時は読み書き操作を終了する
	dcb.fErrorChar = FALSE;					// パリティエラー発生時のキャラクタ置換なし
	dcb.ErrorChar = 0x00;					// パリティエラー発生時の置換キャラクタ
	dcb.EofChar = 0x03;						// データ終了通知キャラクタ
	dcb.EvtChar = 0x02;						// イベント通知キャラクタ

	rc = SetCommState(hCom, &dcb);
	if (!rc){
		printf("%s: 通信条件の設定の設定に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// タイムアウト時間の設定
	COMMTIMEOUTS timeout;					// COMMTIMEOUTS構造体の変数を宣言
	timeout.ReadIntervalTimeout = 2000;		// 文字読込時の全体待ち時間（msec）
	timeout.ReadTotalTimeoutMultiplier = 10;//読込の１文字あたりの時間
	timeout.ReadTotalTimeoutConstant = 2000;//読込エラー検出用のタイムアウト時間
	timeout.WriteTotalTimeoutMultiplier= 10;//書き込み１文字あたりの待ち時間
	timeout.WriteTotalTimeoutConstant = 500;//書き込みエラー検出用のタイムアウト時間

	rc = SetCommTimeouts(hCom, &timeout);
	if (!rc) {
		printf("%s: タイムアウト時間の設定に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	DWORD	dwSendSize;						// 送信データサイズ
	unsigned char rcv[RD_BUFSIZE];			// 受信データ
	DWORD lRead;							// 受信バイト数

	// コマンドの送信
	rc = WriteFile(hCom, cmd, strlen(cmd), &dwSendSize, NULL);
	if (rc == FALSE){
		printf("%s:コマンドの送信に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// IchigoJamからのデータ受信
	rc = ReadFile(hCom, rcv, (DWORD)RD_BUFSIZE, &lRead,	NULL);
	
	if (lRead > 0) {
		// "OK"の削除
		char *p = strrchr((char *)rcv, 'K');
		if (p != NULL) {
			*(p + 1) = 0;
		}
		lRead -= 3;

		// コンソール出力
		fflush(stdout);
		_setmode(_fileno(stdout), _O_TEXT);
		fwrite(rcv, 1, lRead, stdout);
	}

	CloseHandle(hCom);
	return 0;
}