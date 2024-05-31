#include <XInput.h>
#include <Bounce2.h>

// ボタンのピン定義
const int buttonPins[] = {21, 12, 22, 14, 33, 35, 9, 34, 23}; // A, B, X, Y, LB, RB, LT, RT, RS
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]); // ボタンの数

// Bounceオブジェクトの配列
Bounce buttons[numButtons];

// スイッチに対応するボタン割り当て
const int xinputButtons[numButtons] = {
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_LB,
    BUTTON_RB,
    BUTTON_LT,
    BUTTON_RT,
    BUTTON_RS,
};

// 子機のスイッチに対応するボタン割り当て
const int serialButtons[] = {
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT,
    BUTTON_LS,
    BUTTON_BACK,
    BUTTON_START,
    BUTTON_LOGO,
};

// D-padの状態を保持する変数
bool dpadUp = false;
bool dpadDown = false;
bool dpadLeft = false;
bool dpadRight = false;

// 子機の入力を処理する関数
void handleSerialInput(int xinputButton, bool state) {
    switch (xinputButton) {
        case BUTTON_DPAD_UP:
            dpadUp = state;
            break;
        case BUTTON_DPAD_DOWN:
            dpadDown = state;
            break;
        case BUTTON_DPAD_LEFT:
            dpadLeft = state;
            break;
        case BUTTON_DPAD_RIGHT:
            dpadRight = state;
            break;
        default:
            XInput.setButton(xinputButton, state);
            break;
    }
}

void setup() {
  // 各ボタンピンの初期化
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttons[i].attach(buttonPins[i]);
    buttons[i].interval(3); // デバウンスタイム3ms
  }

  // XInputの初期化
  XInput.setAutoSend(false);
  XInput.begin();

  // シリアル通信の初期化
  Serial1.begin(500000); // シリアル通信のボーレートを500000bpsに設定
}

void loop() {
  // ボタンの状態を更新し、XInputに反映
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    XInput.setButton(xinputButtons[i], !buttons[i].read());
  }

  // 受信した子機のデータを処理
  if (Serial1.available() >= 2) { // 2バイト受信できるか確認
    uint8_t eventType = Serial1.read(); // イベントタイプの読み取り
    uint8_t buttonNumber = Serial1.read(); // ボタン番号の読み取り

    // ボタン番号が有効な範囲内であることを確認
    if (buttonNumber >= 0 && buttonNumber < sizeof(serialButtons) / sizeof(serialButtons[0])) {
      int xinputButton = serialButtons[buttonNumber]; // XInputボタンの取得

      // 押下時の処理
      if (eventType == 0x01) {
        handleSerialInput(xinputButton, true);
      }
      // 解放時の処理
      else if (eventType == 0x02) {
        handleSerialInput(xinputButton, false);
      }

      // SOCDクリーニング
      if (dpadRight && dpadLeft) {
        dpadRight = false;
        dpadLeft = false;
      }

      if (dpadUp && dpadDown) {
        dpadUp = false;
        dpadDown = false;
      }

      XInput.setDpad(dpadUp, dpadDown, dpadLeft, dpadRight);

      // XInputの状態を送信
      XInput.send();
    }
  }
}
