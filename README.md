# Motor-Controller

2025年9月より製作中のファームウェアです。  
主な機能として、スマホアプリと通信して得た数値データに基づき、モーターの回転速度を変化させます。  
  
## 目的
研修や実務で得た組み込みソフトウェアの知見を活かし、独自でシステムを  
構築することで理解を正し、知見を形として残すことです。

## システム概要
- 以下スマホアプリ(Motor Controller Helper)とMCU(RX62N)をBluetooth(HC-05 / SPP, v2.1)でUART通信する。  
  https://github.com/Ryo-Takahashi-0422/Motor-Contoller-Helper
- マイコンはEEPROM(24LC64)、Bluetooth(HC-05)、モータードライバ(L298N)と接続
- モータードライバ(L298N)はDCギアモータ(25GA370)に接続

## 開発環境
- IDE : CS+ for CC, HEW
- その他ソフト : PDG2, Flash Development Toolkit
- デバッガ : E2エミュレータLite
- ロジックアナライザー : DAOKAI製 JA-DA-026
- オシロスコープ : RIGOL製 DS1202Z-E

## 動作環境
- RX62N(Renesas製)

## 電子回路図
<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/Motor%20Controller%20-Circuit%20Diagram.jpg" width="800">
</p>

## 動作の様子(音声有り)
https://youtube.com/shorts/VmLjHJ2dSxI

## 動作の流れ
1  アプリ起動  
2  アプリとBluetoothモジュールとの通信開始  
3  アプリより、EEPROMに保存された速度値のリードリクエスト(red, 0000)を送信  
4  MCUより応答(ACK)  
5  アプリより、読み込みアドレスのリクエスト(,1)送信  
6  MCUがI2C通信でEEPROMより速度値を取得し、アプリへ送信  
7  アプリが速度値を受信して、スライドバー値に反映  
8  アプリ上で、redコマンドの返信：x(float)を表示  
9  アプリ上のスライドバーで数値(0~100)を操作し、送信(spd, xを送信)  
10 タイマ動作周期の周期レジスタ値× (0〜100)を用いてPWMデューティ比を算出する。  
モータ特性を考慮し、デューティ比の下限を1%にクリップすることで、  
起動時のクリック音および不安定動作を回避している。

## アプリ画面で見る動作の流れ
### 1. アプリ起動

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/entry.jpg" width="300">
</p>

<p align="center">
  <em>
    HC-05をタップすると通信確認後、通信開始画面に遷移
  </em>
</p>

---

### 2. 通信開始（2〜8）

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/com%20start.jpg" width="300">
</p>

---

### 3. 数値送信（9~10）

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/transmit.jpg" width="300">
</p>

<p align="center">
  <em>
    「spd: 18」から速度値(18)がMCUに送信されていることが分かり、<br>
    「ACK」を受信していることから送信が成功したことが分かる
  </em>
</p>

---

### 4. MCU側に保存された速度値の確認

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/read%20from%20eeprom.jpg" width="300">
</p>

<p align="center">
  <em>
    最後に設定された速度値(18)がMCU側EEPROMに記録されており、<br>
    アプリ再起動後も正しく読み出されていることを確認
  </em>
</p>

## デバイス写真
<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/devices.jpg" width="500">
</p>

## MTU設定
メインクロックは12MHz、周辺モジュールクロックは2逓倍の24MHzとなっている。  
MTUのカウントソースは周辺モジュールクロックの64分周の0.375MHz。  
タイマ動作周期は20msecに設定しているため、PWM周期は7499 (= 7500 - 1)となっている。  
レジスタはTGRAを利用しており、コンペアマッチにより20msec毎にコンペアマッチ割り込み処理Aを発生させる。  
  
割り込み処理Aでは、モータドライバのIN1と接続したMTIOC0B(TGRB)に対してスマホからの速度指示数値  
を75(PWM周期を速度計数の最大値100で分割したデューティ分解能 : 7500 / 100)に乗じて、1を加算する。  
例えば結果が3749となる場合はデューティ比が0.5となり、モータに2.5Vが給電される。  
  
TGRB = 速度計数 * 75 - 1  
  
## 通信コマンドの設計
ファームウェア内部で関数テーブルを保持しており、アプリからの受信データを解析してコマンドテーブルを  
検索、コマンドが見つかればラムダ式でコマンドに応じた関数ポインタを実行する。  
  
各コマンドはvoid*型の値を返り値とする。  
続けてコマンドを実行しない場合(Type1)はNULLを返し、続けてコマンドを実行する場合(Type2)は任意の関数  
ポインタを返す。呼び出し元はコマンドからの返り値を保持しており、関数ポインタを保持している場合は、  
アプリからの続く受信データを解析し、続く関数ポインタに値のみ渡す。  
  
#### Type1のコマンド  
spd : デューティ比に乗ずる係数を設定する。実装済み 。(例 spd,18) 
pvl : PIDのP値を設定する。未実装  
ivl : PIDのI値を設定する。未実装  
dvl : PIDのD値を設定。未実装  
  
#### Type2のコマンド  
red : 上位8bit, 下位8bitからなるEEPROMのメモリアドレスを指定して、MCU側に記憶させる。(例　red,0000)  
MCU側では読み出しバイト数を引数とする関数ポインタを保持する。  
続いて読み出しバイト数を指定する。(例 ,1)  
MCU側では上位・下位アドレスと読み出しバイト数を用いてEEPROMにランダム読み出しを行い、記憶された  
値を得る。  

## UART通信について
受信及び送信の割り込み優先度は以下のように共通して"1"としている。  
IPR_SCIn = 1;  
しかし、RX62Nでは以下のようにベクタ番号の小さい順に割り込みが優先される。  
ERI0(エラー割り込み)、RXI0(受信割り込み)、TXI0(送信割り込み)、TEI0(送信完了割り込み)の順である。  
  
// SCI0_ERI0  
#pragma interrupt (Excep_SCI0_ERI0(vect=214))  
void Excep_SCI0_ERI0(void);  
  
// SCI0_RXI0  
#pragma interrupt (Excep_SCI0_RXI0(vect=215))  
void Excep_SCI0_RXI0(void);  
  
// SCI0_TXI0  
#pragma interrupt (Excep_SCI0_TXI0(vect=216))  
void Excep_SCI0_TXI0(void);  
  
// SCI0_TEI0  
#pragma interrupt (Excep_SCI0_TEI0(vect=217))  
void Excep_SCI0_TEI0(void);  
  
#### 受信処理
MCUでの受信処理の発生タイミングはアプリからの指示に依存するため、基本的に常に受信可能な状態  
となっている。  
アプリからの通信データはBluetoothデバイスを介して1byte単位でMCUに送信されてくる。  
受信データはSCI0.RDRレジスタに順次格納されていく。この受信の際に、受信割り込み処理を発生させて  
SCI0.RDRレジスタの値をuint8_t型ポインタの示す領域に格納し、ポインタをインクリメントする処理を繰り返す  
ことで、受信データを連続して保持している。  
  
受信完了タイミングは、あらかじめ設定した回数の受信が行われた場合か、デリミタ"\r\n"を受信した場合とする。    
前者について、受信のたびにカウンタをデクリメントし、結果が0かどうかを判定している。  
後者について、同様に受信のたびに、受信データがデリミタかどうかを判定している。  
受信完了と判定した場合は、受信割り込みの禁止・受信割り込み要求のクリア・受信中かどうかのメモリ上  
フラグをオフにするなどの処理を実行し、さらに受信完了コールバック処理を呼び出すことで、受信完了時の  
処理につなげている。  
  
#### 送信処理
MCUからの送信は、コマンドに対する応答データを作成したタイミングで行う。  
大まかな流れは以下のとおり。  
送信中かどうかのメモリ上フラグをオンにして、送信データを用意し、送信割り込みを許可する。  
直後に送信割り込みが発生するので、SCI0.TDRに送信データの先頭ポインタを代入することで、ポインタ  
の示すメモリ領域にある値がSCI0.TDRに書き込まれる。(なお、SCI0.TDRの値は自動的にSCI0.TSRに転送  
され、TXD0端子を通じて送信されていく)  
続けてポインタをインクリメントし、カウンタをデクリメントする。この処理を繰り返し、カウンタが0になると  
送信データは全てSCI0.TDRに書き込み終えたとして、通信を完了するために送信割り込みの禁止、  
SCI送信割り込み有効化・送信完了割り込みの許可などの処理を行い、送信完了割り込みを発生させる。  

送信完了割り込み処理では、送信中かどうかのメモリ上フラグをオフにしてから、送信完了コールバック処理を  
呼び出すことで、送信完了時の処理につなげている。  
  
## 2025/12/17時点

### 実装済の主な機能

- Bluetoothモジュールを用いた、アプリとMCU間でのUART相互通信
- アプリからマイコンへコマンドを送信し、MCU側でコマンドに応じた処理を行う。  
  spd, redコマンド実装済み。デリミタは"\r\n"。
- MCUでの関数ポインタを使ったコマンド受付・検索・実行機能。
- MCU↔EEPROMのI2C通信、読み書き
- アプリのスライドバー(0〜100)→デューティ比計算→PWM(MTU 書き込み)でモータ制御を行う。

### 今後実装予定の機能

1. アプリで波形生成
    - 例：モータ速度を10ms間隔で100サンプル取得→rpmに変換→アプリ送信→波形描画
2. PIDモデル構築（オシロで取得した波形からシステム同定を行い、PID制御用の伝達関数を算出）
3. PIDを用いた速度制御・位置制御
4. アプリでPID値入力→MCUへ反映