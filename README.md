# ichigotools
##IchigoJam支援コマンド Windows版

## はじめに
このマニュアルは、IchigoJamを支援するためのコマンドの簡易解説マニュアルです。  
本コマンドを使うことによりIchigoJam上のプログラムをパソコンに保存したり、  
パソコン上のプログラムをIchigoJamに読み込むことが出来ます。  
制約がありますが、パソコンからIchigoJamに命令文を送って実行することも出来ます。  
支援コマンドは、全てコマンドプロンプト上で動作するキャラクタインタフェースで利用します。  

## 利用環境
- IchigoJam本体  
- パソコン  OSとして Windows 10 搭載機(多分,XP以降なら動くと思います)  
- パソコンとIchigoJamでシリアル通信が出来ること(115,200bps)  


## インストール 
ディレクトリ構成の **配布/ichigotools** が実行モジュールです。  

- rsave.exe	リモートセーブコマンド … IchigoJam上のプログラムをパソコン上に保存します。	  　
- rload.exe	リモートロードコマンド  … パソコン上のプログラムをIchigoJamにロードします。 
- rcmd.exe	リモート実行コマンド     … IchigoJam上で任意のコマンドを実行します。  
- マニュアル.txt	本マニュアル

上記の3つのコマンドを各自の用途に応じて、配置して下さい。  
追加ランタイムモジュールも多分不要です。  

### 注意
OSのセキュリティ対策で、インターネット経由で取得した場合は  
各実行モジュール(拡張子 EXEのファイル)にセキュリティブロックが、  
付加される場合があります。この場合はコマンドを実行することが出来ません。  
対策として、エクスプローラで実行モジュール(拡張子 EXEのファイル)の  
プロパティを開き、セキュリティの項目の[ブロック解除]ボタンを押してブロックを解除して下さい。

## 使い方  
事前に、IchigoJamとパソコン間でシリアル接続をして下さい。  

### rsave.exe リモートセーブコマンド  
#### 形式
rsave COMn ファイル名  
#### 例
rsave COM5 たのしいゲーム.txt  
   
#### 説明  
シリアル通信経由で、IchigoJamにてLISTコマンドを実行して、その出力を  
パソコン上に取り込み、指定したファイル名にて保存します。  
保存するファイル形式はプレーンテキストです。  

### rload.exe リモートロードコマンド 
#### 形式
rload COMn ファイル名 [文字ウェイト時間ms  行ウェイト時間ms] 
#### 例
rload COM5 たのしいゲーム.txt  
rload COM5 たのしいゲーム.txt 25 400  
#### 説明  
シリアル通信経由で、IchigoJamにてNEWコマンドを実行して、プログラムを消去後、  
パソコン上の指定したファイルを行単位でIchigoJamに転送し、IchigoJamにロードします。  

デフォルトでは処理として、一行毎に250ミリ秒、１文字毎に15ミリ秒の待ちを行っています。  
IchigoJamのファームウェアバージョンが1.2.1以前で利用の場合は、  
文字ウェイト時間ms、行ウェイト時間msをそれぞれ25、400程度に設定して下さい。  
プログラムサイズが大きい場合、場合時間がかかります。  

### rcmd.exe リモート実行コマンド  
#### 形式
rcmd COMn 命令文   
####例
rcmd COM5 LIST  
rcmd COM5 PRINT \"Hello World\"  
rcmd COM5 \"PRINT 1+1\"  
rcmd COM5 PRINT 1+1  
rcmd COM5 LIST > hogehoge.txt  
#### 説明  
シリアル通信経由で、IchigoJam上でコマンドを実行します。  
実行した出力結果はシリアル通信でパソコンで取り込み、表示します。  
PRINT文などで使う "(ダブルクォーテーション)文字を使う場合は、\” とします。  
本コマンドは、ゲーム等のスクリーン座標を制御するプログラムの実行には向きません。  

