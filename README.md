# Motor-Controller

現在制作中のファームウェアです。

## システム概要

- 以下スマホアプリ（Motor Controller Helper）とマイコンをBluetooth(HC-05)でハンドシェイクしUART通信
  https://github.com/Ryo-Takahashi-0422/Motor-Contoller-Helper
- マイコンはEEPROM(24LC64)、Bluetooth(HC-05)、モータードライバ(L298N)と接続
- モータードライバ(L298N)はDCギアモータ(25GA370)に接続

## 電子回路図
![Image 1](https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/Motor%20Controller%20-Circuit%20Diagram.jpg)

## 動作の流れ
1 アプリ起動
2 アプリとBluetoothモジュールとの通信開始
3 アプリより、EEPROMに保存された速度値のリードリクエスト(red, 0000)を送信
4 MCUより応答(ACK)
5 アプリより、読み込みアドレスのリクエスト(,1)送信
6 MCUがI2C通信でEEPROMより速度値を取得し、アプリへ送信
7 アプリが速度値を受信して、スライドバー値に反映する。
8 アプリ上で、redコマンドの返信：x(float)を表示

## アプリ画面で見る動作の流れ
### 1. アプリ起動

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/entry.jpg" width="200">
</p>

<p align="center">
  <em>
    HC-05をタップすると通信確認後、通信開始画面に遷移
  </em>
</p>

---

### 2. 通信開始（2〜8）

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/com%20start.jpg" width="200">
</p>

---

### 3. MCU側に保存された速度値の確認

<p align="center">
  <img src="https://github.com/Ryo-Takahashi-0422/Motor-Contoller/blob/main/figures/read%20from%20eeprom.jpg" width="200">
</p>

<p align="center">
  <em>
    最後に設定された速度値(18)がMCU側EEPROMに記録されており、  
    アプリ再起動後も正しく読み出されていることを確認
  </em>
</p>

## 動作の様子(音声有り)
https://youtube.com/shorts/VmLjHJ2dSxI

## 2025/12/4時点

### 実装済の主な機能

- アプリとマイコン間のハンドシェイクとUART相互通信
- アプリからマイコンへコマンド送信(コマンド名＋EEPROMアドレス、デリミタは"\r\n")
- マイコンでは関数ポインタを使ったコマンド受付・検索・実行
- マイコン↔EEPROMのI2C通信、読み書き
- アプリのスライドバー(0〜100)→デューティ比計算→PWM（MTU 書き込み）でモータ制御

### 今後実装予定の機能

1. スマホアプリで波形生成
    - 例：モータ速度を10ms間隔で100サンプル取得→rpmに変換→アプリ送信→波形描画
2. PIDモデル構築（オシロで取得した波形からシステム同定を行い、PID制御用の伝達関数を算出）
3. PIDを用いた速度制御・位置制御
4. スマホでPID値入力→マイコンへ反映

### 開発環境

- IDE : CS+ for CC, HEW
- その他ソフト : PDG2, Flash Development Toolkit
- デバッガ : E2エミュレータLite
- ロジックアナライザー : DAOKAI製 JA-DA-026
- オシロスコープ : RIGOL製 DS1202Z-E

## 動作環境

- RX62N(Renesas製)