@rem **********************************************************
@rem  �T�N���G�f�B�^�p�}�N���Z�b�g�A�b�v�o�b�`�t�@�C��
@rem �y�@  �\�zichigotool�R�}���h���p�}�N�����Z�b�g�A�b�v����
@rem �y�ݒ�l�z
@rem   port   : �V���A���ʐM�|�[�g��
@rem   sakura : �T�N���G�f�B�^ini�t�@�C���i�[�A�h���X
@rem   macro  : �T�N���G�f�B�^�}�N���t�@�C���i�[�A�h���X
@rem **********************************************************
set port=com5
set sakura=C:\tools\sakura2-1-1-3_x64
set macro=%sakura\macro
@rem #### �ȉ��͕ύX�֎~! ####
md %macro%
cd /d %~dp0
macro_setup.exe %port% %sakura% %macro%
