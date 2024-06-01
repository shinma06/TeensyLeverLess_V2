#include <XInput.h>

boolean buttonAState = false;

void setup()
{
    // XInputの自動送信を無効にする
    XInput.setAutoSend(false);  
}

void loop()
{
    // ボタンAの状態をトグル
    buttonAState = !buttonAState;
    
    // ボタンAの状態を設定
    XInput.setButton(BUTTON_A, buttonAState);
    
    // XInputの状態を送信
    XInput.send();
    
    // 125マイクロ秒の遅延（8000分の1秒）
    delayMicroseconds(125);
}
