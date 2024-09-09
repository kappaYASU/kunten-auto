# kunten-auto
Kuntenプログラム
これは、漢文の白文と書き下し文、それぞれのテキストファイルから、漢文に訓点（返点、送り仮名）を打ったtexのソースファイルを生成する
プログラムです。フォルダにはWindows及びmacOS用のバイナリが入っています。なお、kuntenそのものは、特別なライブラリを必要としない
C言語のプログラムなので、Linux用が必要な方は、作者までご連絡ください。作者連絡先は、
kappakappa.yasu@gmail.com です。

これには、４つのファイルと２つのフォルダが入っています。これはMac用のバイナリkuntenとWindows用のバイナリkunten.exeに共通です。
フォルダは
１）data：これは必ず実行ファイルkuntenとディレクトリの同じ階層に入っている必要があります。実行ファイルがこのdataにあるデータを変換のために読み取りまが、dataはkunten(またはkunten.exe)と同じディレクトリにないと読み取れません。このディレクトリは
data 
style
kunten.exe -- Windows用のバイナリです。
kunten.       -- MacOS用（Intel Mチップのユニヴァーサル）バイナリです。
Forkunten.xlsm -- Windows用のVBAによるkuntenプログラムのインターフェイスです。Excelとメモ帳（Windowsの標準アクセサリ）がインス
　　　　　　　　トールされていることが前提です。
kuntenUI.app -- Mac用のkuntenプログラムのインターフェイスです。簡単なテキストエディターにもなっています。メニューバーFileのOpenを使えばtxtファイルが編集できます。
※ForkuntenもkuntenUIも最初にkunten（またはkunten.exe）及びdataフォルダが置いてあるディレクトリを指定する必要があります。
 白文ファイルなどを空にした状態で取り扱い説明が排出されればOKです。
