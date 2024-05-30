#include <XInput.h>
#include <Bounce2.h>

// ボタンのピン定義
const int buttonPins[] = {21, 12, 22, 14, 33, 35, 9, 34, 23}; // aButtonPin, bButtonPin, xButtonPin, yButtonPin, lbButtonPin, rbButtonPin, ltButtonPin, rtButtonPin, r3ButtonPin
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]); // ボタンの数

// Bounceオブジェクトの配列
Bounce buttons[numButtons];

// 各ボタンに対応するXInputボタン
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

// 子機の各ボタンに対応するXInputボタン
const int sirialxinputButtons[] = {
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
    // ボタンの状態を更新し、XInputに設定
    for (int i = 0; i < numButtons; i++) {
        buttons[i].update();
        XInput.setButton(xinputButtons[i], !buttons[i].read());
    }

    // シリアル通信で受信したデータを処理
    if (Serial1.available() >= 2) { // 2バイト受信できるか確認
        uint8_t eventType = Serial1.read(); // イベントタイプの読み取り
        uint8_t buttonNumber = Serial1.read(); // ボタン番号の読み取り

        // ボタン番号が有効な範囲内であることを確認
        if (buttonNumber >= 0 && buttonNumber < sizeof(sirialxinputButtons) / sizeof(sirialxinputButtons[0])) {
            int xinputButton = sirialxinputButtons[buttonNumber]; // XInputボタンの取得

            // 押下イベント
            if (eventType == 0x01) {
                XInput.setButton(xinputButton, true);

                // D-padの状態更新
                if (xinputButton == BUTTON_DPAD_UP) dpadUp = true;
                if (xinputButton == BUTTON_DPAD_DOWN) dpadDown = true;
                if (xinputButton == BUTTON_DPAD_LEFT) dpadLeft = true;
                if (xinputButton == BUTTON_DPAD_RIGHT) dpadRight = true;
            }
            // 解放イベント
            else if (eventType == 0x02) {
                XInput.setButton(xinputButton, false);


                // D-padの状態更新
                if (xinputButton == BUTTON_DPAD_UP) dpadUp = false;
                if (xinputButton == BUTTON_DPAD_DOWN) dpadDown = false;
                if (xinputButton == BUTTON_DPAD_LEFT) dpadLeft = false;
                if (xinputButton == BUTTON_DPAD_RIGHT) dpadRight = false;
            }

            // SOCDクリーニング処理
            if (dpadRight && dpadLeft) {
                dpadRight = false;
                dpadLeft = false;
                XInput.setButton(BUTTON_DPAD_RIGHT, false);
                XInput.setButton(BUTTON_DPAD_LEFT, false);
            }

            if (dpadUp && dpadDown) {
                dpadUp = false;
                dpadDown = false;
                XInput.setButton(BUTTON_DPAD_UP, false);
                XInput.setButton(BUTTON_DPAD_DOWN, false);
            }

            // XInputの状態を送信
            XInput.send();
        }
    }
}