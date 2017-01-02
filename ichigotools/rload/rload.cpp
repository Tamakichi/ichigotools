//
// IchigoJam用プログラム保存ツール for Windows
// rload コマンド (IchogoJam用 リモートプログラムロードコマンド)
//
// 【使い方】
//   rload ポート名 ロードするファイル名 [1文字単位のウェイトms １行処理の待ち時間ms]
//   例1： rload com5 sample.bas 
//   例2： rload com5 sample.bas 25 400
//
// 作成 2015/01/23 by たま吉さん
// 修正 2015/02/08 by たま吉さん,送信1バイトごとにウェイトするように修正
// 修正 2016/12/30 by たま吉さん,1文字単位のウェイト時間、1行単位の待ち時間のオプション指定出来るように修正
// 修正 2016/12/31 by たま吉さん,デフォルトの1文字単位のウェイト時間、1行単位の待ち時間をIchigoJam1.2.1用に変更
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define VAR_STR		"1.07"			// バージョン
#define RD_BUFSIZE	2048			// 受信バッファサイズ
#define SD_BUFSIZE	2948			// 送信バッファサイズ
#define COM_SPEED	115200			// シリアル通信速度
#define BLOCKSIZE	1				// 送信単位
#define LINE_WAIT	18				// 行単位のウェイト(ms)
#define CHAR_WAIT	18				// 1文字単位のウェイト(ms)
#define MAXLINENUM	512				// 1行の最大文字数

// メインルーチン
int main(int argc, char* argv[]) {

	HANDLE hCom;					// シリアル通信用ハンドル
	FILE* fp;						// 保存用ファイルハンドル
	int rc;							// 関数戻り値
	char *myname=argv[0];			// 自コマンド名
	int charwaittm = CHAR_WAIT;		// 文字単位のウェイト時間(msec)
	int waittm = LINE_WAIT;			// 行単位のウェイト時間(msec)

	// 引数チェック
	if (argc != 3 && argc != 5) {
		printf("IchogoJam用 リモートプログラムロードコマンド ");
		printf("%s v%s by たま吉さん\n", myname, VAR_STR);
		printf("%s: パラメタの指定が正しくありません。\n", myname);
		printf("\n【使い方】\n %s ポート名 ファイル名 [1文字単位の送信間隔(ms) １行処理のウェイト時間(ms)] \n", myname);
		printf("\n【使用例】\n %s com5 sample.bas\n", myname);
		printf(" %s com5 sample.bas 25 400\n", myname);
		exit(1);
	}

	char *prm_com = argv[1];		// シリアルポート名
	char *fname = argv[2];			// 保存用ファイル名

	// 省略可能引数の処理
	if (argc == 5) {
		charwaittm = atoi(argv[3]);
		if (charwaittm == 0) {
			charwaittm = LINE_WAIT;
		}
		waittm = atoi(argv[4]);
		if (waittm == 0) {
			waittm = CHAR_WAIT;
		}
	}

	// シリアルポートファイル名
	char com_name[16];			
	sprintf_s(com_name, 16,"\\\\.\\%s", prm_com);

	// シリアルポートオープン
	hCom = CreateFileA(com_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
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
	timeout.ReadIntervalTimeout = 500;		// 文字読込時の全体待ち時間（msec）
	timeout.ReadTotalTimeoutMultiplier = 0;	//読込の１文字あたりの時間
	timeout.ReadTotalTimeoutConstant = 500;	//読込エラー検出用のタイムアウト時間
	timeout.WriteTotalTimeoutMultiplier = 0;//書き込み１文字あたりの待ち時間(ms)
	timeout.WriteTotalTimeoutConstant = 500;//書き込みエラー検出用のタイムアウト時間

	rc = SetCommTimeouts(hCom, &timeout);	
	if (!rc) {
		printf("%s: タイムアウト時間の設定に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	DWORD	dwSendSize;		// 送信データサイズ

	// "NEW"コマンドの送信
	rc = WriteFile(hCom, "\nNEW\n", 5, &dwSendSize, NULL);
	if (rc == FALSE){
		printf("%s: NEWコマンドの送信に失敗しました.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	char text[MAXLINENUM];	// 1行分のテキスト

	// 読み込みファイルのオープン
	rc = fopen_s(&fp, fname, "r");
	if (rc) {
		printf("%s: ファイルのオープンに失敗しました:%s", myname, fname);
		exit(1);
	}

	Sleep(100);

	// 1行ずつ読み込んで送信する
	while (NULL != fgets(text, MAXLINENUM, fp)) {
		// データの送信
		for (unsigned int i = 0; i < strlen(text); i++) {
			rc = WriteFile(hCom, &text[i], 1, &dwSendSize, NULL);
			if (!rc){
				printf("%s: 送信に失敗しました.\n", myname);
				CloseHandle(hCom);
				exit(1);
			}
			Sleep(charwaittm);	// 1文字毎のウェイト
		}
		Sleep(waittm); // 行毎のウェイト
	}
	fclose(fp);
	CloseHandle(hCom);
	printf("%s: 正常終了。プログラムをロードしました.\n", myname);
	return 0;
}