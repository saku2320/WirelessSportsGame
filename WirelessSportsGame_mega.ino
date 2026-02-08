#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10);  // CE, CSNピン
const byte address[6] = "00001";  // 共通アドレス
int channels[] = {25, 50, 75, 100, 125};  // 5つのチャネル
int lastDeviceID = -1;  // 前回選ばれたデバイスID
int switchCount = 0;  // スイッチ押下回数をカウント
const int switchPin = 2;  // スイッチピン
bool gameStarted = false;  // ゲームが開始されているかのフラグ
// 7セグメントディスプレイのピン設定（Arduino Megaの未使用ピン）
const int segmentPinsTens[] = {29, 30, 31, 32, 33, 34, 35};  // 10の位（A-Gの各セグメントに対応するピン）
const int segmentPinsOnes[] = {22, 23, 24, 25, 26, 27, 28};  // 1の位（A-Gの各セグメントに対応するピン）
const int digitPins[] = {36, 37};  // 2桁目と1桁目を制御するためのピン（2桁の場合）
// 7セグメントディスプレイの数字表示用データ
const byte digits[10] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};
void setup() {
  pinMode(switchPin, INPUT_PULLUP);  // スイッチピンを内部プルアップに設定
  // 7セグメントディスプレイのピンを出力モードに設定
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPinsTens[i], OUTPUT);  // 10の位のセグメントピン
    pinMode(segmentPinsOnes[i], OUTPUT);  // 1の位のセグメントピン
    digitalWrite(segmentPinsTens[i], LOW);  // 初期状態で全てのセグメントを消灯
    digitalWrite(segmentPinsOnes[i], LOW);  // 初期状態で全てのセグメントを消灯
  }
  for (int i = 0; i < 2; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);  // 初期状態で桁を消灯
  }
  Serial.begin(9600);  // シリアル通信を開始
  radio.begin();  // nRF24L01の初期化
  radio.openWritingPipe(address);  // 送信用パイプを設定
  radio.setPALevel(RF24_PA_LOW);  // 省電力モード
  randomSeed(analogRead(A0));  // ランダムシードを設定
  Serial.println("Receiver Ready");
  // ゲーム開始前に「0」を表示
  displayNumber(0);
}
void loop() {
  // ゲーム開始チェック
  if (!gameStarted && digitalRead(switchPin) == LOW) {
    delay(50);  // デバウンス対策
    if (digitalRead(switchPin) == LOW) {
      gameStarted = true;  // ゲームを開始
      Serial.println("Game Started!");
      selectNextDevice();  // 最初のデバイスを選択
      delay(300);  // デバウンス用の短い遅延
    }
  }
  // ゲームがスタートしている場合、スイッチが押されるたびに次のLEDを点灯
  if (gameStarted && digitalRead(switchPin) == LOW) {
    delay(50);  // デバウンス対策
    if (digitalRead(switchPin) == LOW) {
      switchCount++;  // スイッチの押下回数をカウント
      selectNextDevice();  // 次のデバイスを選択
      Serial.print("Switch Count: ");
      Serial.println(switchCount);  // 押下回数を表示
      // 7セグメントディスプレイにカウントを表示
      displayNumber(switchCount);  // ここでカウントを表示
      // スイッチが押されている間待機
      while (digitalRead(switchPin) == LOW);
      delay(50);  // デバウンス用の短い遅延
    }
  }
}
void selectNextDevice() {
  int randomDeviceID;
  // 前回とは異なるランダムなデバイスIDを生成
  do {
    randomDeviceID = random(1, 6);  // 1～5のランダムIDを生成
  } while (randomDeviceID == lastDeviceID);
  lastDeviceID = randomDeviceID;  // 現在のIDを保存
  int channel = channels[randomDeviceID - 1];  // IDに対応するチャネルを選択
  // 選ばれたチャネルに切り替え
  radio.setChannel(channel);
  radio.stopListening();  // 送信モードに切り替え
  // メッセージを作成
  char message[32];
  snprintf(message, sizeof(message), "Device %d selected", randomDeviceID);
  // メッセージを送信
  bool success = radio.write(&message, sizeof(message));
  if (success) {
    Serial.print("Sent to Channel ");
    Serial.println(channel);
    Serial.println(message);
  } else {
    Serial.println("Failed to send message");
  }
}
// 7セグメントディスプレイに数字を表示
void displayNumber(int number) {
  int tens = number / 10;  // 10の位の数字
  int ones = number % 10;  // 1の位の数字
 
  // 10の位表示
  digitalWrite(digitPins[0], HIGH);  // 10の位の桁をON
  digitalWrite(digitPins[1], LOW);   // 1の位の桁をOFF
  displayDigitTens(tens);  // 10の位を表示
  delay(5);  // 少し待つ（10の位が点灯）
 
  // 1の位表示
  digitalWrite(digitPins[0], LOW);   // 10の位の桁をOFF
  digitalWrite(digitPins[1], HIGH);  // 1の位の桁をON
  displayDigitOnes(ones);  // 1の位を表示
  delay(5);  // 少し待つ（1の位が点灯）
}
 
// 10の位用の7セグメントディスプレイに数字を表示
void displayDigitTens(int digit) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPinsTens[i], (digits[digit] >> i) & 1);  // 対応するセグメントを制御
  }
}
 
// 1の位用の7セグメントディスプレイに数字を表示
void displayDigitOnes(int digit) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPinsOnes[i], (digits[digit] >> i) & 1);  // 対応するセグメントを制御
  }
}
