#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10);  // CE, CSNピン
const byte address[6] = "00001";  // 共通アドレス
const int ledPin = 3;  // LEDピン
const int switchPin = 2;  // スイッチピン
bool ledState = false;  // LEDの状態を保持
bool lastSwitchState = HIGH;  // スイッチの前回の状態
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // デバウンス遅延
void setup() {
  pinMode(ledPin, OUTPUT);  // LEDピンを出力モードに設定
  digitalWrite(ledPin, LOW);  // 初期状態でLEDを消灯
  pinMode(switchPin, INPUT_PULLUP);  // スイッチピンを内部プルアップに設定
  radio.begin();  // nRF24L01の初期化
  radio.setChannel(125);  // 各送信機で異なるチャネルに設定
  radio.openReadingPipe(1, address);  // 受信用パイプを設定
  radio.setPALevel(RF24_PA_LOW);  // 省電力モード
  radio.startListening();  // 受信モードを開始
  Serial.begin(9600);  // シリアル通信を開始
  Serial.println("Transmitter Ready");
}
void loop() {
  // nRF24L01からのメッセージ受信を確認
  if (radio.available()) {
    char receivedMessage[32];  // メッセージバッファ
    radio.read(&receivedMessage, sizeof(receivedMessage));  // メッセージを受信
    // シリアルモニタに受信メッセージを表示
    Serial.print("Received: ");
    Serial.println(receivedMessage);
    // 自分が選ばれた場合、LEDを点灯
    if (strstr(receivedMessage, "Device 5")) {  // 自分に対応するデバイス番号
      digitalWrite(ledPin, HIGH);  // LEDを点灯
      ledState = true;  // LED状態を更新
      Serial.println("LED ON");
    }
  }
  // スイッチの状態を取得
  bool switchState = digitalRead(switchPin);
  // スイッチが押されたときにLEDを消灯
  if (switchState == LOW && lastSwitchState == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      if (ledState) {  // LEDが点灯している場合のみ消灯
        digitalWrite(ledPin, LOW);  // LEDを消灯
        ledState = false;  // LED状態を更新
        Serial.println("LED OFF");
      }
      lastDebounceTime = millis();  // デバウンス用タイムスタンプを更新
    }
  }
  // スイッチの状態を保存
  lastSwitchState = switchState;
}
