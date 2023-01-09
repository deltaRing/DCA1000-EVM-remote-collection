#pragma once
#ifndef _TCP_DEFINE_H_
#define _TCP_DEFINE_H_

#define _DATA_TYPE_COMMAND_ 0x01 // 數據命令
#define _DATA_TYPE_REPLY_ 0x02 // 數據回復
#define _DATA_TYPE_DATA_ 0x03 // 數據格式
#define _DATA_TYPE_DATA_LENGTH_ 0x04 // 數據長度校驗
#define _DATA_TYPE_CODING_ 0x05 // 我會用什麽編碼格式
#define _DATA_MAX_LENGTH_ 1466 // 數據最大長度

// 使用歷程 
// 例如：一字節：數據類型、一字節編碼
// data coding 1 byte CMD 1 byte CODING CMD 
// data type 1 byte CMD/REPLY 1 byte CMD/REPLY CONTENT
// data length 1 CMD 4 BYTE DATA_LENGTH 
// data content 1 CMD 2 LENGTH the rest are BYTE DATA======================= MAX 1469 BYTE
//                              0x03 0xFF(低八位) 0xFF(高八位)
//                              0x04 0xFF 0xFF 0xFF 0xFF

/* ======================命令======================= */
#define _CMD_CLOSE_CLIENT_ 0x01
#define _CMD_RECORD_ 0x02 // 开始采集数据
#define _CMD_RECORD_DONOT_TRANSFER_ 0x03 // 開始采集數據 但不要回傳
#define _CMD_ARE_U_ALIVE_ 0xFF // 心跳包檢測 （并沒有什麽用處）

/* ======================回復======================= */
#define _REPLY_SUCCESS_ 0x00 // 回復：成功
#define _REPLY_FAILED_ 0x01 // 回復：失敗
#define _REPLY_DATA_IS_TRANSFERRED_ 0x02 // 回復 數據已經傳輸
#define _REPLY_NOT_INIT_ 0x03 // 回復：未初始化
#define _REPLY_RECORD_OVER_ 0x04 // 数据采集完毕
#define _REPLY_YEAP_RA9_ 0xFF // 回復：是的我還活著（還鏈接上的）

/* ============================編碼定義============================ */
#define _CODE_NO_ENCODE_ 0x00 // 無編碼
#define _CODE_DHUFF_ 0x01 // 動態哈夫曼


#endif // !_TCP_DEFINE_H_
