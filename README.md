# Wireless Sports Game 
Arduino + nRF24L01 を使ったスポーツゲーム

## 使った物達
Arduino Mega×１
Arduino uno×５
7セグメントLED×2
無線モジュールnRF24×６
百均のLED付きの大きめのボタン×６
２×４木材　１０本

## 構成
丸形の机とスイッチ×1　←スタートボタン
四角型の机とスイッチ×5

それぞれの机にLED付きボタンがついている

## ルール
- スタートボタンを押すと、５台の端末のうち1つがランダムに点灯
- 点灯したボタンを押してスタートボタンを押すと1ポイント
- 制限時間30秒でより多く押せた方の勝利

## Files
- WirelessSportsGame_mega.ino : ArduinoMega機プログラム（スタートボタン用）
- WirelessSportsGame_uno.ino : ArduinoUno機プログラム（5台のボタン用）
