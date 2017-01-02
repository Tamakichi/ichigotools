@rem **********************************************************
@rem  サクラエディタ用マクロセットアップバッチファイル
@rem 【機  能】ichigotoolコマンド利用マクロをセットアップする
@rem 【設定値】
@rem   port   : シリアル通信ポート名
@rem   sakura : サクラエディタiniファイル格納アドレス
@rem   macro  : サクラエディタマクロファイル格納アドレス
@rem **********************************************************
set port=com5
set sakura=C:\tools\sakura2-1-1-3_x64
set macro=%sakura\macro
@rem #### 以下は変更禁止! ####
md %macro%
cd /d %~dp0
macro_setup.exe %port% %sakura% %macro%
