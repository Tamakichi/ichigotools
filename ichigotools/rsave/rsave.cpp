//
// IchigoJam�p�v���O�����ۑ��c�[�� for Windows
// rsave �R�}���h (IchogoJam�p �����[�g�v���O�������[�h�R�}���h)
//
// �y�g�����z
//   rsave �|�[�g�� �ۑ�����t�@�C����
//   ��F rsave com5 sample.bas 
//
// �쐬 2015/01/23 by ���܋g����
// �C�� 2015/06/14 by ���܋g����, �s��Ή� 
// �C�� 2016/12/31 by ���܋g����, �o�̓��b�Z�[�W�̏C��
// 

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define VAR_STR		"1.07"			// �o�[�W����
#define RD_BUFSIZE	2048			// ��M�o�b�t�@�T�C�Y
#define SD_BUFSIZE	2948			// ���M�o�b�t�@�T�C�Y
#define COM_SPEED	115200			// �V���A���ʐM���x
#define BLOCKSIZE	1				// ���M�P��


// ���C�����[�`��
int main(int argc, char* argv[]) {
	HANDLE hCom;					// �V���A���ʐM�p�n���h��
	FILE* fp;						// �ۑ��p�t�@�C���n���h��
	int rc;							// �֐��߂�l
	char *myname = argv[0];			// ���R�}���h��

	// �����`�F�b�N
	if (argc != 3) {
		printf("IchogoJam�p �����[�g�v���O�����Z�[�u�R�}���h ");
		printf("%s v%s by ���܋g����\n", myname, VAR_STR);
		printf("%s: �p�����^�̎w�肪����������܂���B\n", myname);
		printf("\n�y�g�����z\n %s �|�[�g�� �t�@�C����\n", myname);
		printf("\n�y�g�p��z\n %s com5 sample.bas\n", myname);
		exit(1);
	}

	char *prm_com = argv[1];		// �V���A���|�[�g��
	char *fname = argv[2];			// �ۑ��p�t�@�C����

	// �V���A���|�[�g�t�@�C����
	char com_name[16];
	sprintf_s(com_name, 16,"\\\\.\\%s", prm_com);

	// �V���A���|�[�g�I�[�v��
	hCom = CreateFileA(com_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
	timeout.ReadIntervalTimeout = 2000;		// �����Ǎ����̑S�̑҂����ԁimsec�j
	timeout.ReadTotalTimeoutMultiplier =10;	//�Ǎ��̂P����������̎���
	timeout.ReadTotalTimeoutConstant =2000;	//�Ǎ��G���[���o�p�̃^�C���A�E�g����
	timeout.WriteTotalTimeoutMultiplier =10;//�������݂P����������̑҂�����(ms)
	timeout.WriteTotalTimeoutConstant = 500;//�������݃G���[���o�p�̃^�C���A�E�g����

	rc = SetCommTimeouts(hCom, &timeout);
	if (!rc) {
		printf("%s: �^�C���A�E�g���Ԃ̐ݒ�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}


	DWORD dwSendSize;						// ���M�f�[�^�T�C�Y
	unsigned char rcv[RD_BUFSIZE];			// ��M�f�[�^
	DWORD lRead;							// ��M�o�C�g��

	char* cmd;

	// ���s�𑗐M���ăS�~�s�̃N���A
	cmd = "\x1b\nCLS\n";
	memset(rcv, 0, RD_BUFSIZE);
	rc = WriteFile(hCom, cmd, strlen(cmd), &dwSendSize, NULL);
	if (rc == FALSE){
		printf("%s: LIST�R�}���h�̑��M�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}
	// IchigoJam����̃f�[�^�ꊇ��M
	rc = ReadFile(hCom, rcv, (DWORD)RD_BUFSIZE, &lRead, NULL);
	if (rc == false) {
		printf("%s: �f�[�^��M�Ɏ��s���܂���. \n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// "LIST"�R�}���h�̑��M
	cmd = "LIST\n";
	memset(rcv, 0, RD_BUFSIZE);
	rc = WriteFile(hCom, cmd , strlen(cmd), &dwSendSize, NULL);
	if (rc == FALSE){
		printf("%s: LIST�R�}���h�̑��M�Ɏ��s���܂���.\n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// IchigoJam����̃f�[�^�ꊇ��M
	rc = ReadFile(hCom, rcv,(DWORD)RD_BUFSIZE, &lRead, NULL);
	if (rc == false) {
		printf("%s: �f�[�^��M�Ɏ��s���܂���. \n", myname);
		CloseHandle(hCom);
		exit(1);
	}

	// �ۑ��p�t�@�C���̃I�[�v��
	rc = fopen_s(&fp, fname, "w");
	if (rc) {
		printf("%s: �t�@�C���̃I�[�v���Ɏ��s���܂���:%s\n", myname, fname);
		CloseHandle(hCom);
		exit(1);
	}

	lRead -= 4;	
	if (NULL == fwrite(rcv, lRead, 1, fp)) {
		printf("%s: �t�@�C���̏����݂Ɏ��s���܂���:%s\n", myname, fname);
		CloseHandle(hCom);
		fclose(fp);
		exit(1);
	}

	fclose(fp);
	CloseHandle(hCom);
	printf("%s: ����I���B�v���O�������w��t�@�C���ɕۑ����܂���.\n",myname);

	return 0;
}