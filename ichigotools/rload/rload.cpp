//
// IchigoJam�p�v���O�����ۑ��c�[�� for Windows
// rload �R�}���h (IchogoJam�p �����[�g�v���O�������[�h�R�}���h)
//
// �y�g�����z
//   rload �|�[�g�� ���[�h����t�@�C���� [1�����P�ʂ̃E�F�C�gms �P�s�����̑҂�����ms]
//   ��1�F rload com5 sample.bas 
//   ��2�F rload com5 sample.bas 25 400
//
// �쐬 2015/01/23 by ���܋g����
// �C�� 2015/02/08 by ���܋g����,���M1�o�C�g���ƂɃE�F�C�g����悤�ɏC��
// �C�� 2016/12/30 by ���܋g����,1�����P�ʂ̃E�F�C�g���ԁA1�s�P�ʂ̑҂����Ԃ̃I�v�V�����w��o����悤�ɏC��
// �C�� 2016/12/31 by ���܋g����,�f�t�H���g��1�����P�ʂ̃E�F�C�g���ԁA1�s�P�ʂ̑҂����Ԃ�IchigoJam1.2.1�p�ɕύX
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define VAR_STR		"1.07"			// �o�[�W����
#define RD_BUFSIZE	2048			// ��M�o�b�t�@�T�C�Y
#define SD_BUFSIZE	2948			// ���M�o�b�t�@�T�C�Y
#define COM_SPEED	115200			// �V���A���ʐM���x
#define BLOCKSIZE	1				// ���M�P��
#define LINE_WAIT	18				// �s�P�ʂ̃E�F�C�g(ms)
#define CHAR_WAIT	18				// 1�����P�ʂ̃E�F�C�g(ms)
#define MAXLINENUM	512				// 1�s�̍ő啶����

// ���C�����[�`��
int main(int argc, char* argv[]) {

	HANDLE hCom;					// �V���A���ʐM�p�n���h��
	FILE* fp;						// �ۑ��p�t�@�C���n���h��
	int rc;							// �֐��߂�l
	char *myname=argv[0];			// ���R�}���h��
	int charwaittm = CHAR_WAIT;		// �����P�ʂ̃E�F�C�g����(msec)
	int waittm = LINE_WAIT;			// �s�P�ʂ̃E�F�C�g����(msec)

	// �����`�F�b�N
	if (argc != 3 && argc != 5) {
		printf("IchogoJam�p �����[�g�v���O�������[�h�R�}���h ");
		printf("%s v%s by ���܋g����\n", myname, VAR_STR);
		printf("%s: �p�����^�̎w�肪����������܂���B\n", myname);
		printf("\n�y�g�����z\n %s �|�[�g�� �t�@�C���� [1�����P�ʂ̑��M�Ԋu(ms) �P�s�����̃E�F�C�g����(ms)] \n", myname);
		printf("\n�y�g�p��z\n %s com5 sample.bas\n", myname);
		printf(" %s com5 sample.bas 25 400\n", myname);
		exit(1);
	}

	char *prm_com = argv[1];		// �V���A���|�[�g��
	char *fname = argv[2];			// �ۑ��p�t�@�C����

	// �ȗ��\�����̏���
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

	// �V���A���|�[�g�t�@�C����
	char com_name[16];			
	sprintf_s(com_name, 16,"\\\\.\\%s", prm_com);

	// �V���A���|�[�g�I�[�v��
	hCom = CreateFileA(com_name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		// �n���h���擾���s
		printf("%s: �|�[�g[%s]�̃I�[�v���Ɏ��s���܂���.\n", myname, com_name);
		exit(1);
	}

	// ����M�o�b�t�@�T�C�Y�̐ݒ�
	rc = SetupComm(hCom, RD_BUFSIZE, SD_BUFSIZE);
	if (!rc){
		printf("%s: ����M�o�b�t�@�̐ݒ�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// ����M�o�b�t�@�̃N���A
	rc = PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!rc){
		printf("%s: ����M�o�b�t�@�̃N���A�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// ��{�ʐM�����̐ݒ�
	DCB dcb;								// �f�o�C�X����u���b�N
	GetCommState(hCom, &dcb);				// ���݂̐ݒ�̎擾
	dcb.DCBlength = sizeof(DCB);			// BCD�̃T�C�Y�̎擾
	dcb.BaudRate = COM_SPEED;				// �`�����x
	dcb.fBinary = TRUE;						// �o�C�i�����[�h
	dcb.ByteSize = 8;						// �f�[�^�T�C�Y8�r�b�g
	dcb.fParity = NOPARITY;					// �p���e�B��
	dcb.StopBits = ONESTOPBIT;				// �X�g�b�v�r�b�g�P�r�b�g

	
	//�n�[�h�E�F�A�t���[����
	dcb.fOutxCtsFlow = FALSE;				// CTS�n�[�h�E�F�A�t���[����F�g�p���Ȃ�
	dcb.fOutxDsrFlow = FALSE;				// DSR�n�[�h�E�F�A�t���[����F�g�p���Ȃ�
	dcb.fDtrControl = DTR_CONTROL_DISABLE;	// DTR����
	dcb.fRtsControl = RTS_CONTROL_DISABLE;  // RTS���䖳��

	// �\�t�g�E�F�A�t���[����
	dcb.fOutX = FALSE;						// ���M��XON/OFF���䖳��
	dcb.fInX = FALSE;						// ��M��XON/XOFF���䖳��
	dcb.fTXContinueOnXoff = TRUE;			// ��M�o�b�t�@�[���t��XOFF��M��̌p�����M��
	dcb.XonLim = 512;						// XON��������܂łɊi�[�ł���ŏ��o�C�g��
	dcb.XoffLim = 512;						// XOFF��������܂łɊi�[�ł���ŏ��o�C�g��
	dcb.XonChar = 0x11;						// ���M��XON����
	dcb.XoffChar = FALSE;					// XOFF�����w��Ȃ�

	//���̑�
	dcb.fNull = TRUE;						// NULL�o�C�g�̔j��������
	dcb.fAbortOnError = TRUE;				// �G���[���͓ǂݏ���������I������
	dcb.fErrorChar = FALSE;					// �p���e�B�G���[�������̃L�����N�^�u���Ȃ�
	dcb.ErrorChar = 0x00;					// �p���e�B�G���[�������̒u���L�����N�^
	dcb.EofChar = 0x03;						// �f�[�^�I���ʒm�L�����N�^
	dcb.EvtChar = 0x02;						// �C�x���g�ʒm�L�����N�^

	rc = SetCommState(hCom, &dcb);
	if (!rc){
		printf("%s: �ʐM�����̐ݒ�̐ݒ�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}
	
	// �^�C���A�E�g���Ԃ̐ݒ�
	COMMTIMEOUTS timeout;					// COMMTIMEOUTS�\���̂̕ϐ���錾
	timeout.ReadIntervalTimeout = 500;		// �����Ǎ����̑S�̑҂����ԁimsec�j
	timeout.ReadTotalTimeoutMultiplier = 0;	//�Ǎ��̂P����������̎���
	timeout.ReadTotalTimeoutConstant = 500;	//�Ǎ��G���[���o�p�̃^�C���A�E�g����
	timeout.WriteTotalTimeoutMultiplier = 0;//�������݂P����������̑҂�����(ms)
	timeout.WriteTotalTimeoutConstant = 500;//�������݃G���[���o�p�̃^�C���A�E�g����

	rc = SetCommTimeouts(hCom, &timeout);	
	if (!rc) {
		printf("%s: �^�C���A�E�g���Ԃ̐ݒ�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	DWORD	dwSendSize;		// ���M�f�[�^�T�C�Y

	// "NEW"�R�}���h�̑��M
	rc = WriteFile(hCom, "\nNEW\n", 5, &dwSendSize, NULL);
	if (rc == FALSE){
		printf("%s: NEW�R�}���h�̑��M�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	char text[MAXLINENUM];	// 1�s���̃e�L�X�g

	// �ǂݍ��݃t�@�C���̃I�[�v��
	rc = fopen_s(&fp, fname, "r");
	if (rc) {
		printf("%s: �t�@�C���̃I�[�v���Ɏ��s���܂���:%s", myname, fname);
		exit(1);
	}

	Sleep(100);

	// 1�s���ǂݍ���ő��M����
	while (NULL != fgets(text, MAXLINENUM, fp)) {
		// �f�[�^�̑��M
		for (unsigned int i = 0; i < strlen(text); i++) {
			rc = WriteFile(hCom, &text[i], 1, &dwSendSize, NULL);
			if (!rc){
				printf("%s: ���M�Ɏ��s���܂���.\n", myname);
				CloseHandle(hCom);
				exit(1);
			}
			Sleep(charwaittm);	// 1�������̃E�F�C�g
		}
		Sleep(waittm); // �s���̃E�F�C�g
	}
	fclose(fp);
	CloseHandle(hCom);
	printf("%s: ����I���B�v���O���������[�h���܂���.\n", myname);
	return 0;
}