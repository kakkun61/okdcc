<!-- spell-checker:ignore NMRA photocoupler -->

# okdcc monitor

NMRA DCC 信号を受信してデコードし、結果をシリアル通信と画面に出力するテストアプリケーション。

- ボード: M5Stack Core（ESP32）
- 出力
  - シリアル（115200 bps）: `packet: {...}` 形式の JSON
  - 画面: 受信したパケットを新しいものほど上に並べる。上端に状態（RUNNING / PAUSED）・受信数・エラー数
- ボタン
  - A: 停止中、ひとつ古いパケットへさかのぼる
  - B: 表示の更新を停止・再開する（停止中も受信とシリアル出力は続く）
  - C: 停止中、ひとつ新しいパケットへ進む
  - 直近 128 個までさかのぼれる。停止中に古いパケットが上書きされたら、残っている最も古いものへ移る
- 画面は [Clay](https://github.com/nicbarker/clay) と [LovyanGFX](https://github.com/lovyan03/LovyanGFX) で、[clay-lgfx-renderer](https://github.com/kakkun61/clay-lgfx-renderer) を通して描く

## ハードウェア

レールの信号はフォトカプラーで絶縁して GPIO 5（`DCC_SIGNAL_GPIO`）ひとつで受ける。

```
Rail A --- R ---+--- LED (photocoupler) ---+--- Rail B
                |                          |
                +---- anti-parallel D -----+

3.3V --- R (pull-up) ---+--- GPIO 5
                        |
                        +--- photocoupler output (open collector) --- GND
```

### 注意

- **レールを M5Stack の GND に直結しないこと。**
  - DCC はレール間の極性が交互に反転する信号なので、片方を基準にするともう片方は負電圧になり GPIO を壊す。
  - 線路から整流して給電している場合、GND とレールを直結するとブリッジダイオードを短絡する。
  - USB で PC から給電している場合、PC のグラウンドを経由してブースター出力を短絡するおそれがある。
- **高速なフォトカプラーを使うこと**（6N137 など）。
  - フォトカプラーは立ち上がりと立ち下がりで遅延が異なり、半ビットの長さがずれて見える。
  - デコーダーの `1` の半ビットの許容範囲は 52〜64µs、前後の半ビットの差は 6µs までと狭い。PC817 などの低速品では外れることがある。
- **外付けのプルアップ抵抗を付けること**（1k〜4.7kΩ 程度を 3.3V へ）。
  - ファームウェアは内部プルアップを有効にしているが、約 45kΩ と弱く立ち上がりが遅れる。
- フォトカプラーの LED 側には、レール電圧（12〜18V 程度）に合わせた電流制限抵抗と、逆電圧から守る逆並列ダイオードを入れること。
- 出力の論理が反転していても、両エッジの時刻を使うのでデコード結果には影響しない。
- `hardware/` の KiCad 回路図はまだフォトカプラー方式に更新していない。

## ビルドと書き込み

`clay.h` はサブモジュール `board/lib/clay` から読むので、先に `git submodule update --init` しておく。`board/` で次を実行する。

```sh
make build.app.monitor
make upload.app.monitor PORT=/dev/ttyUSB0
```
