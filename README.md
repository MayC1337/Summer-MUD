# 铃响之前

一款以高考倒计时 35 天为背景的 C++ 文字 MUD 游戏。

## 编译

项目要求支持 C++17。使用 CMake 时可执行：

```text
cmake -S . -B build/cmake
cmake --build build/cmake
```

Windows生成的程序名为 `铃响之前.exe`。直接试玩请使用 `build/铃响之前.exe`。

## 发给其他电脑

Windows版本使用静态C++运行库，避免缺少或误加载旧版MinGW DLL。建议发放 `铃响之前-发布包.zip`，接收者先完整解压，再运行文件夹里的 `铃响之前.exe`，不要在压缩软件预览窗口里直接运行。发布包不包含开发者存档。

当前发布程序是Windows x64版本，已在本机移除开发工具PATH的环境下验证启动和完整运行；未对所有Windows版本逐一验证。接收者需要64位Windows，建议Windows 10/11。

## 操作与存档

保留数字菜单，游戏中的选择处也支持：

- `help/帮助`、`status/状态`、`inventory/背包`、`time/时间`
- `map/地图`、`look/观察`、`who/人物`、`talk 林晓`（或 `交谈 林晓`）
- `save/存档`、`quit/退出`（保存成功后退出）
- 晚间主菜单支持 `north/south/east/west` 或 `北/南/东/西`，到达后选择6进行当地活动。

存档文件为当前工作目录下的 `save.txt`。建议始终从同一目录启动程序。V4记录日期、时段、地点、阶段、未完成选择、NPC每日交谈记录和上次行动安排；仍可读取V1—V3存档。旧存档按它原先记录的天数从当天开始，新存档可恢复未完成的阶段。

## 测试

```text
cmake -S . -B build/cmake -DBUILD_TESTING=ON
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure -C Debug
```

测试程序有独立工作目录。有Python3时会额外注册实际EXE的运行测试，测试存档不会使用玩家的正式存档。只构建游戏可设置 `-DBUILD_TESTING=OFF`。

玩法见 `docs/铃响之前-玩法说明.md`，本次改动与验证记录见 `docs/核心修复与地图NPC.md`。
