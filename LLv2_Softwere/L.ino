#include <Bounce2.h>

const int buttonPins[] = {23, 9, 13, 8, 14, 10, 11, 12}; // dpadUpPin, dpadDownPin, dpadLeftPin, dpadRightPin, l3ButtonPin, backButtonPin, startButtonPin, guideButtonPin
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
Bounce buttons[numButtons];

void setup() {
  Serial1.begin(500000); // シリアル通信のボーレートを500000bpsに設定
  
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttons[i].attach(buttonPins[i]);
    buttons[i].interval(3); // デバウンスタイム3ms
  }
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].fell()) {
      Serial1.write(0x01); // 押下イベント
      Serial1.write(i);    // ボタン番号
    }
    if (buttons[i].rose()) {
      Serial1.write(0x02); // 解放イベント
      Serial1.write(i);    // ボタン番号
    }
  }
}