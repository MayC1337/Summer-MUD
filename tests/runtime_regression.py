"""在独立临时目录测试实际EXE，不使用玩家存档。"""
import pathlib
import subprocess
import sys
import tempfile

exe = pathlib.Path(sys.argv[1]).resolve()


def run(folder, commands):
    result = subprocess.run([str(exe)], input="\n".join(commands) + "\n",
                            text=True, encoding="utf-8", errors="replace",
                            capture_output=True, cwd=folder, timeout=30)
    assert result.returncode == 0, result.stderr
    return result.stdout


def snapshot(folder):
    rows = (folder / "save.txt").read_text(encoding="utf-8").splitlines()
    assert rows[0] == "SummerMUDSaveV4"
    i = 5
    events = rows[i + 1:i + 1 + int(rows[i])]
    i += 1 + int(rows[i])
    i += 1 + int(rows[i])  # items
    i += 1 + int(rows[i])  # event choices
    return rows, events, list(map(int, rows[i].split())), i


with tempfile.TemporaryDirectory(prefix="before-bell-runtime-") as root:
    folder = pathlib.Path(root)
    failure_folder = folder / "write-failure"
    failure_folder.mkdir()
    (failure_folder / "save.txt").mkdir()
    out = run(failure_folder, ["1", "保存失败测试", "quit", "1"])
    assert "保存失败" in out and "进度已保存" not in out
    assert "仍留在当前菜单" in out
    # 非法输入不会选择新游戏或消耗行动；输入结束不会空跑35天。
    out = run(folder, ["1", "断流测试"])
    rows, _, stage, _ = snapshot(folder)
    assert rows[4] == "0" and stage[0] == 0
    assert "输入已结束，停止推进时间" in out
    assert "最终结局" not in out

    # 第一天晚间移动到图书馆，交谈后在学习方法子菜单保存退出。
    out = run(folder, ["1", "场景测试", "1", "1", "1abc", "1.5", "1", "1", "1", "1",
                       "map", "north", "east", "who", "talk linxiao", "talk linxiao",
                       "6", "1", "2", "save", "quit"])
    rows, _, stage, index = snapshot(folder)
    assert rows[4] == "0" and stage[:2] == [3, 3]
    assert '"library"' in rows[index + 1]
    assert rows[index + 6] == "3 6 1 2"
    assert "今天已经聊过了" in out and "林晓" in out
    stats = list(map(int, rows[3].split()))
    assert stats[6] == 45  # 数学练习尚未结算
    out = run(folder, ["2", "time", "who", "talk linxiao", "1"])
    after, _, _, _ = snapshot(folder)
    after_stats = list(map(int, after[3].split()))
    assert "恢复地点：图书馆" in out
    assert "今天已经聊过了" in out
    assert after_stats[6] == 50  # 只结算一次数学练习
    assert "正在翻开" not in out  # 重定向输入输出不播放动画

    # 沿用在当前位置活动时，同时恢复上次晚间地点。
    rows, _, stage, index = snapshot(folder)
    stage[0:2] = [3, 3]
    stage[6] = 1
    rows[4] = "1"
    rows[index] = " ".join(map(str, stage))
    rows[index + 1] = '"gate" "" "library"'
    rows[index + 6] = "0"
    (folder / "save.txt").write_text("\n".join(rows) + "\n", encoding="utf-8")
    out = run(folder, ["2"])
    assert any(line.startswith("路线：校门 → ") and line.endswith(" → 图书馆")
               for line in out.splitlines()), out
    assert "图书馆" in out and "沿用安排" in out

    # 晚间数字0退出不跳天，也不会在读档时再次自动退出。
    out = run(folder, ["1", "退出测试", "1", "1", "1", "1", "1", "1", "0"])
    rows, _, stage, index = snapshot(folder)
    assert rows[4] == "0" and stage[:2] == [3, 3]
    assert rows[index + 6] == "0"

    # 固定一场待选择的随机事件；退出不能重抽事件或重复结算。
    stage[0] = 6
    rows[index] = " ".join(map(str, stage))
    rows[index + 1] = '"gate" "rainy_day" "gate"'
    (folder / "save.txt").write_text("\n".join(rows) + "\n", encoding="utf-8")
    before = list(map(int, rows[3].split()))
    out = run(folder, ["2", "quit"])
    rows, _, stage, index = snapshot(folder)
    assert '"rainy_day"' in rows[index + 1] and stage[0] == 6
    assert list(map(int, rows[3].split())) == before
    out = run(folder, ["2", "2"])
    rows, events, stage, index = snapshot(folder)
    assert rows[4] == "1" and "rainy_day" in events
    assert list(map(int, rows[3].split()))[3] == before[3] - 5

    # 一条完整35天路线，所有连续故事与周考仍在。
    out = run(folder, ["1", "完整通关"] + ["1"] * 2000)
    rows, events, stage, _ = snapshot(folder)
    assert rows[4] == "35" and stage[0] == 0
    assert out.count("周周考") == 5
    for prefix in ("desk", "teacher", "family", "cat"):
        for step in range(1, 5):
            assert f"{prefix}_{step}" in events
    assert "最终结局" in out and "录取通知书" in out
    print("Runtime EOF, strict-input, mid-menu resume, NPC deduplication and 35-day checks passed.")
