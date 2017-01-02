//
// IchigoJam�p�v���O�����ۑ��c�[�� for Windows
// macro_setup �R�}���h �T�N���G�f�B�^�p�}�N�����Z�b�g�A�b�v
//
// �y�g�����z
//   macro_setup �V���A���|�[�g �T�N���G�f�B�^�C���X�g�[���f�B���N�g��
//   ��F macro_setup com4 C:\tools\sakura C:\tools\sakura\macro
//
// �쐬 2017/01/01 by ���܋g����
// �C�� 2017/01/02 by ���܋g����, �����ύX
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAR_STR		"1.08"			// �o�[�W����

// ���C�����[�`��
int main(int argc, char* argv[]) {
	FILE* fp;						// �ۑ��p�t�@�C���n���h��
	int rc;							// �֐��߂�l
	char *myname = argv[0];			// ���R�}���h��

	// �����`�F�b�N
	if (argc != 4) {
		printf("Ichigotools�p�T�N���G�f�B�^�}�N���Z�b�g�A�b�v");
		printf("%s v%s by ���܋g����\n", myname, VAR_STR);
		printf("%s: �p�����^�̎w�肪����������܂���B\n", myname);
		printf("\n�y�g�����z\n %s �|�[�g �T�N���G�f�B�^INI�t�@�C���z�u�f�B���N�g�� �}�N���i�[�f�B���N�g��\n", myname);
		printf("\n�y�g�p��z\n %s setup com5 C:\\tools\\sakura C:\\tools\\sakura\\macro\n  ", myname);
		exit(1);
	}

	char ini_path[512];				// �ۑ��p�t�@�C����
	char macro_path[512];			// �}�N���i�[�f�B���N�g��
	char macro_fname[512];			// �}�N���t�@�C����
	char tools_path[512];			// �c�[���C���X�g�[���p�X
	char tools_path2[512];			// �c�[���C���X�g�[���p�X(�␳)
	char* port = argv[1];			// �V���A���|�[�g
	char str[512];

	rc = GetModuleFileNameA(NULL, tools_path, 512);
	int i = strlen(tools_path) - 1;
	while (tools_path[i] != '\\') i--;
	tools_path[i] = 0;

	//tools_path��\��\\��
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

	// sakura.ini�t�@�C���̃`�F�b�N
	rc = fopen_s(&fp, ini_path, "r");
	if (rc) {
		printf("%s: sakura.ini�̃I�[�v���Ɏ��s���܂���:%s.\n", myname, ini_path);
		exit(1);
	}
	fclose(fp);

	//�}�N���p�X�̎擾
/*
	if (!(rc = GetPrivateProfileStringA("Folders", "szMACROFOLDER", "", macro_path, 512, ini_path))) {
		printf("%s: szMACROFOLDER�̒l�擾�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	//printf("path=%s\n", macro_path);
*/
	strcpy_s(macro_path, argv[3]);
	//printf("path=%s\n", macro_path);

	//*************************
	// �}�N���t�@�C���̍쐬
	//*************************

	sprintf_s(macro_fname, "%s\\rload.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rload.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}

	sprintf_s(str, "ExecCommand('%s\\\\rload.exe %s $F');", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rload.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}	
	fclose(fp);

	sprintf_s(macro_fname, "%s\\rsave.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rsave.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	sprintf_s(str, "ExecCommand('%s\\\\rcmd.exe %s list',3);", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rsave.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	fclose(fp);

	sprintf_s(macro_fname, "%s\\run.mac", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: run.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	sprintf_s(str, "ExecCommand('%s\\\\rcmd.exe %s run');", tools_path2, port);
	if (fputs(str, fp) < 0) {
		printf("%s: run.mac�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	fclose(fp);

	sprintf_s(macro_fname, "%s\\rcmd.vbs", macro_path);
	rc = fopen_s(&fp, macro_fname, "w");
	if (rc) {
		printf("%s: rcmd.vbs�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	sprintf_s(str,
		"Dim str\nstr = Editor.InputBox(\"�R�}���h���C��\", \"\", 255)\n"
		"Editor.ExecCommand \"%s\\rcmd.exe %s \"& str, 3", tools_path, port);
	if (fputs(str, fp) < 0) {
		printf("%s: rcmd.vbs�̍쐬�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	fclose(fp);

	//*************************
	//sakura.ini�t�@�C���̐ݒ�
	//*************************
	
	// �}�N���t�H���_�̓o�^
	if (!(rc = WritePrivateProfileStringA("Folders", "szMACROFOLDER", macro_path, ini_path))) {
		printf("%s: File[000]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	// �}�N���t�@�C���̓o�^
	if (!(rc = WritePrivateProfileStringA("Macro", "File[000]", "rsave.mac", ini_path))) {
		printf("%s: File[000]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[001]", "rload.mac", ini_path))) {
		printf("%s: File[001]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[002]", "run.mac", ini_path))) {
		printf("%s: File[002]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "File[003]", "rcmd.vbs", ini_path))) {
		printf("%s: File[003]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}

	// �}�N�����̂̓o�^
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[000]",
		"Ichigo \xe3\x82\xbd\xe3\x83\xbc\xe3\x82\xb9\xe5\x8f\x96\xe3\x82\x8a\xe8\xbe\xbc\xe3\x81\xbf", ini_path))) {
		printf("%s: Name[000]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[001]",
		"Ichigo \xe3\x82\xa2\xe3\x83\x83\xe3\x83\x97\xe3\x83\xad\xe3\x83\xbc\xe3\x83\x89", ini_path))) {
		printf("%s: Name[001]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[002]", 
		"Ichigo \xe3\x83\x97\xe3\x83\xad\xe3\x82\xb0\xe3\x83\xa9\xe3\x83\xa0\xe5\xae\x9f\xe8\xa1\x8c", ini_path))) {
		printf("%s: Name[002]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "Name[003]", 
		"Ichigo \xe3\x82\xb3\xe3\x83\x9e\xe3\x83\xb3\xe3\x83\x89\xe5\xae\x9f\xe8\xa1\x8c", ini_path))) {
		printf("%s: File[003]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	// �}�N�����s���[�h�̓o�^
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[000]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[000]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[001]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[001]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[002]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[002]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	if (!(rc = WritePrivateProfileStringA("Macro", "ReloadWhenExecute[003]", "0", ini_path))) {
		printf("%s: ReloadWhenExecute[003]�̒l�ݒ�Ɏ��s���܂���.\n", myname);
		exit(1);
	}
	return 0;
}