//
// IchigoJam用プログラム保存ツール for Windows
// macro_setup コマンド サクラエディタ用マクロ環境セットアップ
//
// 【使い方】
//   macro_setup シリアルポート サクラエディタインストールディレクトリ
//   例： macro_setup com4 C:\tools\sakura C:\tools\sakura\macro
//
// 作成 2017/01/01 by たま吉さん
// 修正 2017/01/02 by たま吉さん, 引数変更
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAR_STR		"1.08"			// バージョン

// メインルーチン
int main(int argc, char* argv[]) {
	FILE* fp;						// 保存用ファイルハンドル
	int rc;							// 関数戻り値
	char *myname = argv[0];			// 自コマンド名

	// 引数チェック
	if (argc != 4) {
		printf("Ichigotools用サクラエディタマクロセットアップ");
		printf("%s v%s by たま吉さん\n", myname, VAR_STR);
		printf("%s: パラメタの指定が正しくありません。\n", myname);
		printf("\n【使い方】\n %s ポート サクラエディタINIファイル配置ディレクトリ マクロ格納ディレクトリ\n", myname);
		printf("\n【使用例】\n %s setup com5 C:\\tools\\sakura C:\\tools\\sakura\\macro\n  ", myname);
		exit(1);
	}

	char ini_path[512];				// 保存用ファイル名
	char macro_path[512];			// マクロ格納ディレクトリ
	char macro_fname[512];			// マクロファイル名
	char tools_path[512];			// ツールインストールパス
	char tools_path2[512];			// ツールインストールパス(補正)
	char* port = argv[1];			// シリアルポート
	char str[512];

	rc = GetModuleFileNameA(NULL, tools_path, 512);
	int i = strlen(tools_path) - 1;
	while (tools_path[i] != '\\') i--;
	tools_path[i] = 0;

	//tools_pathの\を\\に
	i = 0;
	for (unsigned int j = 0; j < strlen(tools_path); j++) {
		tools_path2[i] = tools_path[j];
		i++;
		if (tools_path[j] == '\\') {
			tools_path2[i] = '\\';
			i++;
		}
	}
	tools_path2[i] = 0;
	sprintf_s(ini_path, "%s\\sakura.ini", argv[2]);

	// sakura.iniファイルのチェック
	rc = fopen_s(&fp, ini_path, "r");
	if (rc) {
		printf("%s: sakura.iniのオープンに失敗しました:%s.\n", myname, ini_path);
		exit(1);
	}
	fclose(fp);

	//マクロパスの取得
/*
	if (!(rc = GetPrivateProfileStringA("Folders", "szMACROFOLDER", "", macro_path, 512, ini_path))) {
		printf("%s: szMACROFOLDERの値取得に失敗しました.\n", myname);
		exit(1);
	}
	//printf("path=%s\n", macro_path);
*/
	strcpy_s(macro_path, argv[3]);
	//printf("path=%s\n", macro_path);

	//*************************
	// マクロファイルの作成
	//*************************

	sprintf_s(macro_fname, "%s\\rload.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rload.macの作成に失敗しました.\n", myname);
		exit(1);
	}

	sprintf_s(str, "ExecCommand('%s\\\\rload.exe %s $F');", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rload.macの作成に失敗しました.\n", myname);
		exit(1);
	}	
	fclose(fp);

	sprintf_s(macro_fname, "%s\\rsave.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rsave.macの作成に失敗しました.\n", myname);
		exit(1);
	}
	sprintf_s(str, "ExecCommand('%s\\\\rcmd.exe %s list',3);", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rsave.macの作成に失敗しました.\n", myname);
		exit(1);
	}
	fclose(fp);

	sprintf_s(macro_fname, "%s\\run.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: run.macの作成に失敗しました.\n", myname);
		exit(1);
	}
	sprintf_s(str, "ExecCommand('%s\\\\rcmd.exe %s run');", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: run.macの作成に失敗しました.\n", myname);
		exit(1);
	}
	fclose(fp);

	sprintf_s(macro_fname, "%s\\rcmd.vbs", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rcmd.vbsの作成に失敗しました.\n", myname);
		exit(1);
	}
	sprintf_s(str,
		"Dim str\nstr = Editor.InputBox(\"コマンドライン\", \"\", 255)\n"
		"Editor.ExecCommand \"%s\\rcmd.exe %s \"& str, 3", tools_path, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rcmd.vbsの作成に失敗しました.\n", myname);
		exit(1);
	}
	fclose(fp);

	//*************************
	//sakura.iniファイルの設定
	//*************************
	
	// マクロフォルダの登録
	if (!(rc = WritePrivateProfileStringA("Folders", "szMACROFOLDER", macro_path, ini_path))) {
		printf("%s: File[000]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	// マクロファイルの登録
	if (!(rc = WritePrivateProfileStringA("Macro", "File[000]", "rsave.mac", ini_path))) {
		printf("%s: File[000]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[001]", "rload.mac", ini_path))) {
		printf("%s: File[001]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[002]", "run.mac", ini_path))) {
		printf("%s: File[002]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[003]", "rcmd.vbs", ini_path))) {
		printf("%s: File[003]の値設定に失敗しました.\n", myname);
		exit(1);
	}

	// マクロ名称の登録
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[000]",
		"Ichigo \xe3\x82\xbd\xe3\x83\xbc\xe3\x82\xb9\xe5\x8f\x96\xe3\x82\x8a\xe8\xbe\xbc\xe3\x81\xbf", ini_path))) {
		printf("%s: Name[000]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[001]",
		"Ichigo \xe3\x82\xa2\xe3\x83\x83\xe3\x83\x97\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x89", ini_path))) {
		printf("%s: Name[001]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[002]", 
		"Ichigo \xe3\x83\x97\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe5\xae\x9f\xe8\xa1\x8c", ini_path))) {
		printf("%s: Name[002]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[003]", 
		"Ichigo \xe3\x82\xb3\xe3\x83\x9e\xe3\x83\xb3\xe3\x83\x89\xe5\xae\x9f\xe8\xa1\x8c", ini_path))) {
		printf("%s: File[003]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	// マクロ実行モードの登録
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[000]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[000]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[001]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[001]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[002]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[002]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[003]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[003]の値設定に失敗しました.\n", myname);
		exit(1);
	}
	return 0;
}