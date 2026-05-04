#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UDP送信と受信の初期化
 *
 * 指定したIPとポートを送信先として設定し、
 * UDP通信を開始できる状態にします。
 *
 * @param ip 送信先IPアドレス（例 "192.168.11.2"）
 * @param port 送信先ポート番号
 * @return 成功で1、失敗で0
 */
int udp_library_init(const char* ip, int port);

/**
 * @brief UDPデータ送信
 *
 * 指定したバイト列をUDPで送信します。
 *
 * @param data 送信データのポインタ
 * @param size 送信バイト数
 */
void udp_library_send(const void* data, int size);

/**
 * @brief UDP送信終了処理
 *
 * ソケットを閉じて通信を終了します。
 */
void udp_library_close();

/**
 * @brief 受信時コールバック関数型
 *
 * @param data 受信データ
 * @param size 受信バイト数
 */
typedef void (*udp_receive_callback)(const char* data, int size);

/**
 * @brief UDP受信開始
 *
 * 指定ポートでUDP受信を開始し、
 * 受信データをバッファに格納します。
 *
 * @param port 受信ポート
 * @param buffer 受信バッファ
 * @param buffer_size バッファサイズ
 * @param cb 受信時コールバック（NULL可）
 * @return 成功で1、失敗で0
 */
int udp_library_start_receive(int port,
                              char* buffer,
                              int buffer_size,
                              udp_receive_callback cb);

/**
 * @brief UDP受信停止
 */
void udp_library_stop_receive();

#ifdef __cplusplus
}
#endif