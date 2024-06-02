from inputs import get_gamepad
import time

# 前回の時間をマイクロ秒単位で取得
prev_time = time.perf_counter() * 1000000
last_event_code = None

while True:
    # ゲームパッドの入力イベントを取得
    events = get_gamepad()
    if events:
        event = events[0]
        event_code = (event.ev_type, event.code)
        
        # 同期イベントをチェック
        if event_code == ('Sync', 'SYN_REPORT'):
            last_event_code = event_code
        
        # 前回のイベントコードと異なる場合、時間差を計算して表示
        if event_code != last_event_code:
            current_time = time.perf_counter() * 1000000
            time_diff = current_time - prev_time
            prev_time = current_time
            print("Time difference:", time_diff, "μs", event_code)
            last_event_code = event_code