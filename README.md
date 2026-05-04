# UDP Library

Windows（WinSock2）向けのシンプルなUDP送受信ラッパーライブラリです。
送信・受信・コールバック処理を最小APIで扱えるように設計されています。

---

# 特徴

* UDP送信・受信を簡易化
* 受信は自動でバッファへ格納
* 受信時コールバック対応（任意）
* スレッド内で非同期受信
* WinSock2を内部で管理

---

# ファイル構成

```
udp_library.h
udp_library.cpp
```

---

# 使い方

## 1. 初期化

```c
udp_library_init("192.168.11.2", 5678);
```

* 送信先IPとポートを設定
* WinSock初期化も内部で実行

---

## 2. 受信開始

```c
char buffer[256];

udp_library_start_receive(
    1234,
    buffer,
    256,
    NULL
);
```

### 引数

* `port` : 受信ポート
* `buffer` : 受信データ格納用バッファ
* `buffer_size` : バッファサイズ
* `cb` : コールバック（NULL可）

---

## 3. 送信

```c
char msg[8] = {1,2,3,4,5,6,7,8};

udp_library_send(msg, 8);
```

* バイト数は可変
* UDPで指定先へ送信

---

## 4. 受信（コールバックあり）

```c
void on_receive(const char* data, int size)
{
    printf("recv: %d bytes\n", size);
}

udp_library_start_receive(1234, buffer, 256, on_receive);
```

---

## 5. 終了処理

```c
udp_library_stop_receive();
udp_library_close();
```

---

# 動作仕様

## 受信方式

* 別スレッドで `recvfrom()` をブロッキング実行
* 受信データは `buffer` に直接格納される
* 必要ならコールバックが呼ばれる

---

## バッファ仕様

* ユーザーが確保したメモリを使用
* 常に上書きされる（最新データのみ保持）
* サイズ超過時は警告表示

---

## 送信仕様

* UDPパケットとして送信
* サイズ制限なし（UDP上限内）
* 非同期処理なし（即送信）

---

# 注意事項

* UDPのためパケットロスは保証されない
* 受信バッファは上書きされ続ける
* スレッドセーフ設計ではない
* callback内で重い処理を行うと遅延が発生する可能性がある

---

# 設計思想

このライブラリは以下を目的としています：

* 最小APIでUDP通信を実装
* リアルタイム用途（入力・制御信号）向け
* WinSockの複雑さを隠蔽
* 低遅延優先設計

---

# 使用例（最小構成）

```c
#include "udp_library.h"

char buffer[256];

int main()
{
    udp_library_init("192.168.11.2", 5678);

    udp_library_start_receive(1234, buffer, 256, NULL);

    while (1)
    {
        char msg[8] = {1,2,3,4,5,6,7,8};

        udp_library_send(msg, 8);

        Sleep(10);
    }

    udp_library_close();
    return 0;
}
```

必要なら、このREADMEを「GitHub向け（バッジ・構成図付き）」や「英語版」にもできます。
