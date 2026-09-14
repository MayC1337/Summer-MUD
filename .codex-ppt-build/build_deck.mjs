import fs from "node:fs/promises";
import path from "node:path";
import { pathToFileURL } from "node:url";
import { Presentation, PresentationFile } from "@oai/artifact-tool";

const workspaceDir = "D:/Mud-dev/Summer-MUD";
const SKILL_DIR = "C:/Users/32533/.codex/plugins/cache/openai-primary-runtime/presentations/26.909.11814/skills/presentations";
const TMP_DIR = path.join(workspaceDir, ".codex-ppt-build");
const FINAL_PPTX = path.join(workspaceDir, "deliverables", "铃响之前_课堂展示_终稿.pptx");
const RUNTIME_PYTHON = "C:/Users/32533/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe";
const { makeNativeBulletParagraphs, finalizePresentation } = await import(
  pathToFileURL(path.join(SKILL_DIR, "container_tools/artifact_tool_utils.mjs")).href,
);

await fs.mkdir(TMP_DIR, { recursive: true });
await fs.mkdir(path.dirname(FINAL_PPTX), { recursive: true });

const W = 1280, H = 720;
const C = {
  ink: "#17332D", dark: "#11251F", board: "#193B33", teal: "#2A7668",
  mint: "#9ECBBC", paper: "#F7F1E5", cream: "#FFF9EC", orange: "#E99343",
  gold: "#F0C45C", red: "#BD5B4A", grey: "#5E6B66", pale: "#E7EFEA",
  white: "#FFFFFF", black: "#111111"
};
const FONT = "Microsoft YaHei";
const MONO = "Microsoft YaHei";

const deck = Presentation.create({ slideSize: { width: W, height: H } });

function rect(slide, x, y, w, h, fill, line = "none", radius = 0) {
  return slide.shapes.add({
    geometry: radius ? "roundRect" : "rect",
    position: { left: x, top: y, width: w, height: h },
    fill,
    line: { style: "solid", fill: line, width: line === "none" ? 0 : 1.5 },
    ...(radius ? { borderRadius: radius } : {}),
  });
}

function textBox(slide, text, x, y, w, h, opts = {}) {
  const s = slide.shapes.add({
    geometry: "textbox", position: { left: x, top: y, width: w, height: h },
    fill: opts.fill ?? "none",
    line: { style: "solid", fill: opts.line ?? "none", width: opts.line && opts.line !== "none" ? 1 : 0 },
    ...(opts.radius ? { borderRadius: opts.radius } : {}),
  });
  s.text = text;
  s.text.style = {
    typeface: opts.font ?? FONT,
    fontSize: opts.size ?? 24,
    bold: opts.bold ?? false,
    color: opts.color ?? C.ink,
    alignment: opts.align ?? "left",
    verticalAlignment: opts.valign ?? "middle",
    autoFit: "none",
  };
  return s;
}

function bulletBox(slide, items, x, y, w, h, opts = {}) {
  const s = textBox(slide, "", x, y, w, h, opts);
  s.text = makeNativeBulletParagraphs(items, {
    marginLeftPoints: opts.marginLeft ?? 17,
    hangingPoints: opts.hanging ?? 8,
    spaceAfterPoints: opts.spaceAfter ?? 8,
  });
  s.text.style = {
    typeface: opts.font ?? FONT, fontSize: opts.size ?? 22,
    color: opts.color ?? C.ink, autoFit: "none",
  };
  return s;
}

function addTitle(slide, title, index, subtitle = "") {
  slide.background.fill = C.paper;
  rect(slide, 0, 0, 18, H, C.orange);
  textBox(slide, title, 64, 34, 1030, 58, { size: 34, bold: true, color: C.dark });
  if (subtitle) textBox(slide, subtitle, 66, 91, 1060, 34, { size: 16, color: C.grey });
  rect(slide, 65, 120, 1150, 2, C.mint);
  textBox(slide, String(index).padStart(2, "0"), 1160, 42, 55, 38, { size: 17, bold: true, color: C.teal, align: "right" });
}

function notes(slide, body) {
  slide.speakerNotes.textFrame.setText(body);
}

function label(slide, value, x, y, w, fill = C.teal, color = C.white) {
  return textBox(slide, value, x, y, w, 34, { size: 17, bold: true, color, fill, radius: 10, align: "center" });
}

function node(slide, value, x, y, w, h, opts = {}) {
  return textBox(slide, value, x, y, w, h, {
    size: opts.size ?? 18, bold: opts.bold ?? true, color: opts.color ?? C.ink,
    fill: opts.fill ?? C.cream, line: opts.line ?? C.mint, radius: opts.radius ?? 10,
    align: opts.align ?? "center", valign: "middle"
  });
}

function arrow(slide, a, b, opts = {}) {
  return slide.shapes.connect(a, b, {
    kind: opts.kind ?? "elbow", fromSide: opts.from ?? "right", toSide: opts.to ?? "left",
    line: { style: opts.dashed ? "dashed" : "solid", fill: opts.color ?? C.teal, width: opts.width ?? 2 },
    tail: { type: "triangle", width: "sm", length: "sm" },
  });
}

function classBox(slide, name, members, x, y, w, h, fill = C.cream) {
  const box = rect(slide, x, y, w, h, fill, C.teal, 8);
  rect(slide, x, y, w, 36, C.teal, C.teal, 8);
  textBox(slide, name, x + 6, y + 2, w - 12, 31, { size: 18, bold: true, color: C.white, align: "center" });
  textBox(slide, members, x + 12, y + 43, w - 24, h - 50, { size: 14, color: C.ink, valign: "top" });
  return box;
}

// 1 Cover
{
  const s = deck.slides.add();
  s.background.fill = C.dark;
  rect(s, 0, 0, 22, H, C.orange);
  textBox(s, "铃  响  之  前", 100, 130, 850, 88, { size: 54, bold: true, color: C.cream });
  textBox(s, "高考倒计时 35 天的 C++ 文字 MUD", 104, 230, 760, 48, { size: 27, color: C.mint });
  rect(s, 103, 300, 780, 3, C.orange);
  textBox(s, "课程设计课堂展示", 104, 330, 520, 50, { size: 30, bold: true, color: C.white });
  textBox(s, "故事背景  系统设计  核心算法  STL  程序演示", 104, 392, 760, 36, { size: 18, color: C.gold });
  textBox(s, "35", 925, 112, 230, 180, { size: 116, bold: true, color: C.orange, align: "center" });
  textBox(s, "DAYS", 956, 278, 170, 40, { size: 23, bold: true, color: C.mint, align: "center" });
  textBox(s, "汇报人：________    小组：________", 104, 620, 750, 36, { size: 18, color: C.pale });
  textBox(s, "C++17", 1050, 626, 100, 30, { size: 16, bold: true, color: C.gold, align: "right" });
  notes(s, "开场：我们设计了一款以高考前35天为背景的文字MUD游戏《铃响之前》。这次展示从故事和需求出发，再介绍类设计、核心算法、STL应用、测试与程序演示。上台前请填写汇报人和小组信息。来源：当前Summer-MUD项目源码与V1.0需求文档。");
}

// 2 Background
{
  const s = deck.slides.add(); addTitle(s, "故事背景与设计目标", 2, "把35天复习生活压缩成可选择、可反馈、可回顾的一段校园故事");
  textBox(s, "黑板上的倒计时从 35 开始。玩家要在学习、休息、社交和娱乐之间分配时间，并在每周六通过周测观察变化。", 72, 152, 710, 92, { size: 26, bold: true, color: C.dark });
  const days = ["晨间", "午间", "下午", "晚间"];
  const desc = ["晨读、早餐、梳理弱科", "吃饭、午休、同学交流", "课堂学习、专项练习", "地图探索、购物、休息"];
  days.forEach((d, i) => {
    textBox(s, d, 82 + i * 177, 285, 145, 42, { size: 20, bold: true, color: C.white, fill: i === 3 ? C.orange : C.teal, radius: 8, align: "center" });
    textBox(s, desc[i], 82 + i * 177, 338, 145, 102, { size: 17, color: C.ink, align: "center", valign: "top" });
  });
  rect(s, 835, 157, 345, 410, C.board, C.board, 16);
  textBox(s, "倒计时", 878, 190, 260, 38, { size: 22, bold: true, color: C.mint, align: "center" });
  textBox(s, "35", 870, 225, 275, 125, { size: 88, bold: true, color: C.orange, align: "center" });
  textBox(s, "5 次周测", 875, 362, 265, 42, { size: 25, bold: true, color: C.cream, align: "center" });
  textBox(s, "16 段连续故事\n9 个校园地点\n多种考试与人生结局", 875, 422, 265, 112, { size: 20, color: C.white, align: "center", valign: "top" });
  textBox(s, "核心体验：选择行动，观察数值变化，根据周测调整策略，完成35天后获得最终成绩与结局。", 82, 515, 690, 85, { size: 21, color: C.teal, bold: true });
  notes(s, "故事背景：玩家扮演一名高三学生，在最后35天里安排每天四个时段。游戏不强调战斗，而强调时间分配和状态管理。每周六安排周测，帮助玩家了解自己的复习效果。连续故事涉及同桌、班主任、家人和流浪猫，最终分数与状态共同影响结局。");
}

// 3 Functions
{
  const s = deck.slides.add(); addTitle(s, "功能需求与游戏主线", 3, "课堂演示聚焦能够直接观察和验证的功能");
  const timeline = [
    ["新游戏 / 读档", "创建玩家或恢复V4存档"], ["每日行动", "上午、中午、下午、晚间"],
    ["故事与地图", "事件选择、NPC对话、地点移动"], ["周测反馈", "第6、13、20、27、34天"],
    ["高考与结局", "750分制、大学去向、成长回忆"]
  ];
  const nodes = timeline.map((item, i) => node(s, `${item[0]}\n${item[1]}`, 55 + i * 242, 205, 190, 108, { size: i === 3 ? 17 : 18, fill: i === 3 ? "#FFF0D8" : C.cream, line: i === 3 ? C.orange : C.mint }));
  for (let i = 0; i < nodes.length - 1; i++) arrow(s, nodes[i], nodes[i + 1], { color: C.orange });
  label(s, "玩家可见功能", 78, 380, 180);
  bulletBox(s, ["菜单数字选择，同时支持中文和英文命令", "进入地点自动提示NPC，交谈不消耗行动", "行动后立即显示属性变化，每日显示简短状态", "自动存档，可从未完成阶段继续"], 78, 425, 520, 198, { size: 20, spaceAfter: 7 });
  label(s, "系统约束", 690, 380, 180, C.orange);
  bulletBox(s, ["游戏固定35天，星期六周测，星期日休息", "属性范围0到100，考试按750分制换算", "随机事件每次最多触发一个，已触发事件不重复", "损坏或版本不匹配的存档拒绝读取"], 690, 425, 500, 198, { size: 20, spaceAfter: 7 });
  notes(s, "功能介绍可按这条主线讲。程序启动后选择新游戏或继续游戏。普通学习日包含四个时段，第六天开始出现周测。地图与NPC属于晚间扩展，但不改变核心35天调度。SaveManager负责自动保存状态，Ending根据最终成绩和状态给出结局。");
}

// 4 WBS
{
  const s = deck.slides.add(); addTitle(s, "WBS 工作分解结构", 4, "需求分析把项目拆成5个可交付阶段，每个阶段都有明确产物");
  const root = node(s, "《铃响之前》\n课程设计项目", 485, 142, 310, 64, { fill: C.board, color: C.white, line: C.board, size: 21 });
  const phases = [
    ["1 需求与设计", "故事规则\n用例与WBS\n类与接口"],
    ["2 核心框架", "35天调度\n玩家属性\n命令输入"],
    ["3 内容模块", "行动事件\n地图NPC\n考试结局"],
    ["4 整合测试", "存档恢复\n模块联调\n缺陷修复"],
    ["5 交付展示", "发布程序\n设计报告\n课堂演示"],
  ];
  phases.forEach((p, i) => {
    const x = 38 + i * 248;
    const head = node(s, p[0], x, 290, 210, 54, { fill: i === 4 ? C.orange : C.teal, color: C.white, line: i === 4 ? C.orange : C.teal, size: 18 });
    const body = node(s, p[1], x, 370, 210, 145, { fill: C.cream, line: C.mint, size: 18, bold: false });
    arrow(s, root, head, { from: "bottom", to: "top", color: C.mint });
    arrow(s, head, body, { from: "bottom", to: "top", color: C.teal });
  });
  textBox(s, "WBS用于回答“项目需要完成哪些工作”，后续类图和流程图再回答“程序怎样实现”。", 150, 590, 980, 42, { size: 21, color: C.dark, bold: true, align: "center" });
  notes(s, "WBS从项目交付角度拆解工作。第一阶段确定需求和接口。第二阶段建立核心循环。第三阶段完成独立功能模块。第四阶段做整合与回归测试。最后形成可执行程序、课程设计报告和课堂展示。正式答辩时可以结合本组真实分工补充每位成员的责任。");
}

// 5 Use case
{
  const s = deck.slides.add(); addTitle(s, "Use-Case 用例图", 5, "玩家通过一组可观察用例完成从创建角色到查看结局的全过程");
  const actor = node(s, "玩家", 45, 295, 130, 70, { fill: C.board, color: C.white, line: C.board, size: 22 });
  rect(s, 225, 145, 965, 460, "#FBF7EE", C.mint, 12);
  textBox(s, "《铃响之前》系统边界", 250, 158, 300, 34, { size: 20, bold: true, color: C.teal });
  const uses = [
    ["开始新游戏", 270, 215], ["继续游戏", 490, 215], ["选择时段行动", 710, 215], ["查看状态与地图", 930, 215],
    ["触发事件并选择", 270, 365], ["与NPC交谈", 490, 365], ["参加每周周测", 710, 365], ["保存并退出", 930, 365],
    ["参加最终高考并查看结局", 595, 510]
  ].map(([t, x, y]) => {
    const e = s.shapes.add({ geometry: "ellipse", position: { left: x, top: y, width: t.length > 12 ? 300 : 180, height: 58 }, fill: C.cream, line: { style: "solid", fill: C.teal, width: 1.5 } });
    e.text = t; e.text.style = { typeface: FONT, fontSize: 17, bold: true, color: C.ink, alignment: "center", verticalAlignment: "middle", autoFit: "none" };
    return e;
  });
  uses.forEach((u, i) => arrow(s, actor, u, { kind: "straight", color: i >= 6 ? C.orange : C.mint, width: 1.3 }));
  textBox(s, "系统自动执行：推进时间、计算属性、触发周测和事件、自动保存进度", 245, 624, 930, 35, { size: 19, color: C.grey, align: "center" });
  notes(s, "用例图只描述玩家能够完成的目标，不展示类和函数。玩家可以开始或继续游戏，选择行动，查看信息，探索地图和NPC，参加每周周测，保存退出，并最终查看高考成绩与结局。时间推进和分数计算由系统自动完成。");
}

// 6 Architecture
{
  const s = deck.slides.add(); addTitle(s, "系统总体架构", 6, "GameManager统一调度，各模块通过公开接口协作");
  const layers = [
    ["交互层", "ConsoleUI    CommandParser", C.board, C.white],
    ["调度层", "GameManager    TimeManager    GameProgress", C.teal, C.white],
    ["业务层", "Action    EventManager    Exam    Ending    SaveManager", "#DDEBE5", C.ink],
    ["领域对象", "Player    Stats    Inventory    Item    CampusMap    NPC", C.cream, C.ink],
    ["基础设施", "C++17    STL    文件流    Windows控制台", "#EEE6D7", C.grey],
  ];
  layers.forEach((l, i) => {
    const y = 158 + i * 92;
    textBox(s, l[0], 82, y, 165, 65, { size: 20, bold: true, color: l[3], fill: l[2], radius: 8, align: "center" });
    textBox(s, l[1], 265, y, 915, 65, { size: i === 2 ? 20 : 22, bold: i < 2, color: l[3], fill: l[2], radius: 8, align: "center" });
  });
  textBox(s, "调用方向", 45, 612, 120, 32, { size: 16, color: C.grey });
  textBox(s, "输入进入调度层，调度层调用业务模块，业务模块修改领域对象；SaveManager统一负责文件读写。", 180, 604, 990, 48, { size: 20, bold: true, color: C.dark });
  notes(s, "系统采用分层和模块化设计。ConsoleUI与CommandParser处理输出和输入。GameManager处在中间，决定当前执行哪个阶段。Action、EventManager、Exam等负责具体业务。Player和地图对象保存数据。SaveManager是唯一集中处理存档文件的模块，避免把文件读写散落在各个类中。");
}

// 7 Core UML
{
  const s = deck.slides.add(); addTitle(s, "UML 类图：核心调度", 7, "类图保留关键成员和公开操作，重点说明职责与依赖关系");
  const gm = classBox(s, "GameManager", "- player: unique_ptr<Player>\n- timeManager: TimeManager\n- progress: GameProgress\n- eventManager: EventManager\n+ startGame()\n+ run()\n- processCurrentDay()", 435, 145, 330, 218, "#FFF2DC");
  const tm = classBox(s, "TimeManager", "- totalDays: int\n- elapsedDays: int\n+ advanceDay(): bool\n+ getCurrentDay(): int\n+ getCurrentDayType(): DayType", 50, 170, 295, 177);
  const gp = classBox(s, "GameProgress", "+ stage: Stage\n+ period: int\n+ location: string\n+ lastWeeklyScore: int\n+ pendingChoices: vector<int>", 50, 440, 295, 165);
  const ac = classBox(s, "Action", "- currentTime: ActionTime\n- world: CampusMap*\n+ executeDailyAction(Player&)\n+ setWorld(CampusMap*)", 880, 145, 310, 150);
  const em = classBox(s, "EventManager", "- events: vector<Event>\n- triggeredEvents: set<string>\n- eventChoices: map<string,int>\n+ triggerEvent(Player&, int)\n+ triggerStory(Player&, int, int)", 880, 345, 310, 183);
  const sm = classBox(s, "SaveManager", "- saveFile: string\n+ saveGame(...)\n+ loadGame(...)\n+ hasSave(): bool", 480, 485, 260, 125);
  arrow(s, gm, tm, { from: "left", to: "right" });
  arrow(s, gm, gp, { from: "left", to: "right" });
  arrow(s, gm, ac, { from: "right", to: "left" });
  arrow(s, gm, em, { from: "right", to: "left" });
  arrow(s, gm, sm, { from: "bottom", to: "top", color: C.orange });
  textBox(s, "组合/持有", 350, 284, 85, 25, { size: 13, color: C.grey, align: "center" });
  textBox(s, "调用", 775, 250, 70, 25, { size: 13, color: C.grey, align: "center" });
  notes(s, "核心类图以GameManager为中心。它持有时间、进度、事件、考试、存档等对象，并使用unique_ptr管理玩家。TimeManager只管理日期。GameProgress记录当天阶段，用于中途恢复。Action不拥有地图，只保存一个非拥有指针。SaveManager通过参数读取各对象数据，不接管它们的生命周期。");
}

// 8 Domain UML
{
  const s = deck.slides.add(); addTitle(s, "UML 类图：玩家、事件与校园", 8, "领域对象保存游戏状态，管理类负责集合与规则");
  const p = classBox(s, "Player", "- name_: string\n- money_: int\n- stats_: unique_ptr<Stats>\n- inventory_: unique_ptr<Inventory>\n+ getStats(): Stats&\n+ changeMoney(int)", 55, 160, 280, 190, "#FFF2DC");
  const st = classBox(s, "Stats", "- 9项属性: int\n+ modify(StatType,int)\n+ set(StatType,int)\n+ get(StatType): int", 70, 450, 245, 132);
  const inv = classBox(s, "Inventory", "- items_: vector<unique_ptr<Item>>\n+ addItem(...)\n+ removeItem(string)\n+ hasItem(string): bool", 390, 165, 300, 145);
  const it = classBox(s, "Item", "- id_: string\n- price_: int\n- effects_: vector<pair<...>>\n+ use(Player&)", 410, 440, 260, 142);
  const map = classBox(s, "CampusMap", "- rooms: map<string,Room>\n- current: string\n+ move(direction)\n+ routeTo(destination)", 780, 145, 280, 145);
  const npc = classBox(s, "NPC", "- id/name/room\n- period: int\n+ isPresent(...)\n+ talk(Player&,EventManager&,bool)", 930, 415, 280, 150);
  const ev = classBox(s, "Event", "- id/title/description\n- choices: vector<string>\n+ canTrigger(Player&)\n+ applyChoice(Player&,int)", 680, 445, 220, 150);
  arrow(s, p, st, { from: "bottom", to: "top" });
  arrow(s, p, inv, { from: "right", to: "left" });
  arrow(s, inv, it, { from: "bottom", to: "top" });
  arrow(s, map, npc, { from: "bottom", to: "top", dashed: true });
  arrow(s, npc, p, { from: "left", to: "right", dashed: true, color: C.orange });
  arrow(s, ev, p, { from: "left", to: "right", dashed: true, color: C.orange });
  textBox(s, "组合", 165, 386, 65, 22, { size: 13, color: C.grey, align: "center" });
  textBox(s, "使用", 770, 620, 65, 22, { size: 13, color: C.grey, align: "center" });
  notes(s, "Player组合Stats和Inventory，Inventory再组合多个Item，符合Player、Stats、Inventory、Item的既定关系。CampusMap使用map保存Room，NPC根据地点和时段判断是否出现。Event和NPC通过Player公开接口修改状态，不直接访问Player私有成员。");
}

// 9 Flowchart
{
  const s = deck.slides.add(); addTitle(s, "核心算法流程：35天状态机", 9, "循环每次推进一个可保存阶段，只有FinishDay真正增加天数");
  const a = node(s, "DayStart\n显示日期与旁白", 65, 165, 180, 74, { fill: C.board, color: C.white, line: C.board });
  const d = s.shapes.add({ geometry: "diamond", position: { left: 310, top: 160, width: 170, height: 90 }, fill: C.cream, line: { style: "solid", fill: C.teal, width: 1.5 } });
  d.text = "当天类型？"; d.text.style = { typeface: FONT, fontSize: 18, bold: true, color: C.ink, alignment: "center", verticalAlignment: "middle", autoFit: "none" };
  const act = node(s, "Routine / Action\n四时段行动", 550, 150, 190, 72);
  const story = node(s, "Story\n检查连续剧情", 810, 150, 190, 72);
  const daily = node(s, "DailyEvent\n随机事件", 1045, 150, 170, 72);
  const exam = node(s, "SpecialDay\n星期六周测", 425, 365, 190, 72, { fill: "#FFF0D8", line: C.orange });
  const rest = node(s, "SpecialDay\n星期日休息", 680, 365, 190, 72);
  const finish = node(s, "FinishDay\n状态小结并推进1天", 930, 510, 230, 78, { fill: C.board, color: C.white, line: C.board });
  arrow(s, a, d);
  arrow(s, d, act); textBox(s, "学习日", 475, 173, 65, 22, { size: 13, color: C.teal, align: "center" });
  arrow(s, act, story); arrow(s, story, daily);
  arrow(s, story, act, { from: "bottom", to: "bottom", dashed: true, color: C.mint });
  textBox(s, "还有时段", 735, 257, 92, 23, { size: 13, color: C.grey, align: "center" });
  arrow(s, d, exam, { from: "bottom", to: "top", color: C.orange });
  arrow(s, d, rest, { from: "bottom", to: "top" });
  textBox(s, "星期六", 360, 298, 70, 22, { size: 13, color: C.orange, align: "center" });
  textBox(s, "星期日", 590, 318, 70, 22, { size: 13, color: C.teal, align: "center" });
  arrow(s, daily, finish, { from: "bottom", to: "top" });
  arrow(s, exam, finish); arrow(s, rest, daily, { from: "right", to: "bottom" });
  arrow(s, finish, a, { from: "bottom", to: "bottom", dashed: true, color: C.orange });
  textBox(s, "自动保存点：每个阶段完成后保存下一阶段，退出后不会重复结算行动或周测。", 145, 625, 990, 35, { size: 19, bold: true, color: C.dark, align: "center" });
  notes(s, "主循环使用状态机。学习日依次进行四个时段，每个时段结束后检查故事。星期六直接进入周测，星期日执行休息。所有分支最终进入FinishDay，只有这里调用advanceDay。每个阶段完成后自动保存下一阶段，因此读档不会重复扣钱、加属性或重复计算周测。");
}

// 10 sequence
{
  const s = deck.slides.add(); addTitle(s, "UML 序列图：保存、退出与继续", 10, "SaveManager集中读写，GameProgress保证从未完成阶段恢复");
  const xs = [105, 330, 570, 810, 1050];
  const names = ["玩家", "CommandParser", "GameManager", "SaveManager", "save.txt"];
  const heads = names.map((n, i) => node(s, n, xs[i], 150, 135, 45, { fill: i === 2 ? C.orange : C.teal, color: C.white, line: i === 2 ? C.orange : C.teal, size: 16 }));
  heads.forEach((h, i) => {
    s.shapes.add({ geometry: "line", position: { left: xs[i] + 67, top: 198, width: 0, height: 410 }, fill: "none", line: { style: "dashed", fill: C.mint, width: 1.2 } });
  });
  const messages = [
    [0,1,220,"输入 quit"], [1,2,275,"handleCommand(quit)"], [2,3,330,"saveGame(player,time,event,progress)"],
    [3,4,385,"写临时文件并替换"], [4,3,440,"成功"], [3,2,485,"true"], [2,0,535,"提示已保存并退出"],
  ];
  messages.forEach(([from,to,y,msg]) => {
    const lineLeft = Math.min(xs[from], xs[to]) + 67;
    const lineWidth = Math.abs(xs[to] - xs[from]);
    s.shapes.add({ geometry: "line", position: { left: lineLeft, top: y, width: lineWidth, height: 0 }, fill: "none", line: { style: "solid", fill: from > to ? C.orange : C.teal, width: 1.8 } });
    textBox(s, msg, Math.min(xs[from], xs[to]) + 76, y - 25, Math.abs(xs[to]-xs[from]) - 18, 24, { size: 13, color: C.ink, align: "center" });
  });
  textBox(s, "继续游戏时执行相反过程：读取版本标记和各项数据，验证成功后一次性恢复对象状态。", 215, 632, 850, 34, { size: 18, bold: true, color: C.dark, align: "center" });
  notes(s, "玩家在任意输入位置输入quit。CommandParser把命令交给GameManager。GameManager先同步当前位置、阶段和未完成选项，再调用SaveManager。SaveManager先写临时文件，检查成功后替换save.txt。只有保存成功才退出。继续游戏时先解析并验证到局部变量，全部合法后再恢复玩家和各管理器。");
}

// 11 STL table
{
  const s = deck.slides.add(); addTitle(s, "STL 标准模板库的应用", 11, "容器保存状态，算法负责查找与变换，迭代器统一遍历方式");
  const values = [
    ["组件", "代码中的用途", "典型位置", "选择原因"],
    ["vector", "物品、事件、NPC、路径、输入记录", "Inventory / EventManager / GameProgress", "连续存储，便于按顺序遍历"],
    ["map", "房间索引、出口、事件选择", "CampusMap / Room / EventManager", "通过键快速查找对象"],
    ["set", "已触发事件、NPC每日交谈记录", "EventManager / GameProgress", "元素不重复，适合判重"],
    ["array", "4个时段行动、16段固定故事", "GameProgress / StoryData", "数量固定，语义清楚"],
    ["queue", "地图BFS待访问房间", "CampusMap::routeTo", "先进先出，按层搜索"],
    ["算法", "find_if、any_of、reverse、clamp", "背包、路径、属性与考试", "复用标准实现，减少循环代码"],
    ["迭代器", "begin/end、范围for、erase位置", "各容器遍历与删除", "同一方式操作不同容器"],
  ];
  const table = s.tables.add({ rows: values.length, columns: 4, left: 65, top: 155, width: 1150, height: 450, values, columnWidths: [155, 350, 330, 315] });
  table.borders.assign({ style: "solid", fill: C.mint, width: 1 });
  table.cells.block({ row: 0, column: 0, rowCount: 1, columnCount: 4 }).assign({ fill: C.board, textStyle: { typeface: FONT, fontSize: 17, bold: true, color: C.white }, margins: { left: 8, right: 8, top: 5, bottom: 5 } });
  table.cells.block({ row: 1, column: 0, rowCount: 7, columnCount: 4 }).assign({ textStyle: { typeface: FONT, fontSize: 15, color: C.ink }, margins: { left: 8, right: 8, top: 4, bottom: 4 } });
  [2,4,6].forEach(r => table.cells.block({ row: r, column: 0, rowCount: 1, columnCount: 4 }).fill = "#EEF3ED");
  textBox(s, "智能指针 unique_ptr 配合 vector 管理 Player 和 Item 的生命周期，避免手动 delete。", 145, 627, 990, 34, { size: 18, bold: true, color: C.teal, align: "center" });
  notes(s, "STL部分建议结合源码举例。vector保存顺序集合。map通过字符串ID查房间。set天然去重，适合已触发事件。array用于固定数量的数据。queue支持BFS。find_if和any_of用于物品查找，reverse恢复路径顺序，clamp限制属性范围。范围for和迭代器让遍历方式统一。没有实际使用的sort不列为成果。");
}

// 12 Algorithms
{
  const s = deck.slides.add(); addTitle(s, "核心算法：地图寻路与考试计算", 12, "两个算法分别解决“如何到达”和“学习成果怎样反馈”");
  label(s, "广度优先搜索 BFS", 78, 155, 245);
  const bfs = ["当前地点入队", "取出队首房间", "访问未到达的相邻房间", "记录前驱并继续入队", "到达目标后反向回溯路径"];
  const bfsNodes = bfs.map((t,i)=>node(s, `${i+1}. ${t}`, 72 + (i%2)*275, 215 + Math.floor(i/2)*105, 235, 58, { size: 16, fill: i===4 ? "#FFF0D8" : C.cream, line: i===4 ? C.orange : C.mint }));
  arrow(s,bfsNodes[0],bfsNodes[1]); arrow(s,bfsNodes[1],bfsNodes[2],{from:"bottom",to:"top"}); arrow(s,bfsNodes[2],bfsNodes[3]); arrow(s,bfsNodes[3],bfsNodes[4],{from:"bottom",to:"top",color:C.orange});
  textBox(s, "时间复杂度 O(V + E)", 145, 540, 330, 34, { size: 20, bold: true, color: C.teal, align: "center" });
  label(s, "周测与高考分数", 700, 155, 245, C.orange);
  textBox(s, "得分率 =", 715, 235, 130, 34, { size: 23, bold: true, color: C.dark });
  textBox(s, "0.77 × 学科能力\n+ 0.12 × 智力\n+ 0.08 × 健康\n+ 0.03 × 体力\n− 0.08 × 压力\n+ 周测随机波动", 845, 205, 310, 205, { size: 22, bold: true, color: C.ink, fill: C.cream, line: C.mint, radius: 10, align: "center" });
  textBox(s, "单科分数 = 得分率 × 单科满分", 735, 445, 410, 42, { size: 22, bold: true, color: C.orange, align: "center" });
  textBox(s, "语文150  数学150  英语150  理综300\n总分750；最终高考不加入随机波动", 735, 505, 410, 70, { size: 19, color: C.grey, align: "center" });
  notes(s, "BFS从当前地点开始按层搜索，用queue维护待访问房间，用map记录每个房间的前驱，到达目标后回溯并reverse得到正向路线。在9个地点的小图中开销很小。考试算法综合学科能力和状态。周测加入小幅随机波动，智力较高时波动缩小。最终高考不加入随机波动，便于稳定复现结局。");
}

// 13 patterns
{
  const s = deck.slides.add(); addTitle(s, "设计模式与关键设计选择", 13, "只说明当前代码中能够对应和验证的设计，不把计划中的模式当作成果");
  const items = [
    ["Singleton", "GameManager::getInstance()", "保证总调度器在程序中只有一个实例"],
    ["Simple Factory", "EventFactory::createEvent(id)", "把事件对象的创建集中到工厂"],
    ["State Machine", "GameProgress::Stage", "把一天拆成可保存、可恢复的阶段"],
    ["Callback", "CommandParser commandHandler", "输入模块通过函数对象把命令交给GameManager"],
    ["RAII", "unique_ptr 与 TemporaryFile", "对象离开作用域时自动释放资源或清理临时文件"],
  ];
  items.forEach((it,i)=>{
    const y=150+i*95;
    textBox(s,it[0],75,y,180,54,{size:19,bold:true,color:C.white,fill:i===2?C.orange:C.teal,radius:8,align:"center"});
    textBox(s,it[1],280,y,360,54,{size:18,bold:true,color:C.dark,fill:C.cream,line:C.mint,radius:8,align:"center"});
    textBox(s,it[2],670,y,500,54,{size:18,color:C.ink});
  });
  textBox(s, "未采用：早期文档计划中的 Observer 没有形成完整实现，因此不作为本项目设计模式成果。", 140, 636, 1000, 34, { size: 18, color: C.red, bold: true, align: "center" });
  notes(s, "这里强调诚实对应源码。GameManager使用单例。EventFactory是简单工厂。GameProgress的Stage构成状态机。CommandParser通过std::function注册回调。unique_ptr和临时文件清理体现RAII。早期文档提到Observer，但当前项目没有完整观察者接口，所以答辩中不应声称已经使用。");
}

// 14 testing
{
  const s = deck.slides.add(); addTitle(s, "测试与质量保证", 14, "三层测试覆盖类行为、模块整合和真实可执行程序流程");
  const t1=node(s,"CoreRegression\n输入、地图、NPC\nV1—V4存档与原子替换",65,175,310,140,{fill:C.cream,size:20});
  const t2=node(s,"StoryIntegration\n连续故事、行动重放\n考试范围与界面对齐",485,175,310,140,{fill:C.cream,size:20});
  const t3=node(s,"runtime_regression.py\n驱动实际EXE\n完整35天与中途恢复",905,175,310,140,{fill:"#FFF0D8",line:C.orange,size:20});
  arrow(s,t1,t2);arrow(s,t2,t3,{color:C.orange});
  const metrics=[
    ["35", "天完整通关"], ["5", "次周测出现"], ["16", "段连续故事"], ["9", "个地点连通"], ["V1—V4", "存档兼容"]
  ];
  metrics.forEach((m,i)=>{
    textBox(s,m[0],65+i*235,390,200,65,{size:i===4?32:42,bold:true,color:i===4?C.orange:C.teal,align:"center"});
    textBox(s,m[1],65+i*235,456,200,34,{size:17,color:C.grey,align:"center"});
  });
  bulletBox(s,["非法输入、输入结束和保存失败不会让游戏空跑或错误跳天", "读档不会重复扣钱、重复学习、重复周测或重复领取NPC奖励", "Windows版本静态链接C++运行库，降低其他电脑缺少DLL的风险"], 150,535,980,110,{size:18,spaceAfter:5});
  notes(s, "测试分三层。CoreRegression验证基础类和边界。StoryIntegration验证多个模块组合。runtime_regression脚本直接运行最终EXE并输入命令，检查完整35天。自动测试验证了5次周测、16段故事和9个地点连通。自动测试不能替代小组互测，报告中还应记录同学实际试玩发现的问题。");
}

// 15 demo
{
  const s = deck.slides.add(); addTitle(s, "程序演示路线与总结", 15, "课堂上用一条短路线展示核心功能，再用预备存档跳到周测和结局");
  rect(s,70,155,660,430,C.black,C.black,10);
  textBox(s,"《铃响之前》 · 演示命令",95,175,600,34,{size:21,bold:true,color:C.gold,font:MONO});
  textBox(s,
    "> 1  新游戏\n> 输入姓名\n> 选择晨间行动，观察属性变化\n> 地图\n> 北\n> 人物\n> 交谈 班主任\n> 存档 / 退出\n> 2  继续游戏",
    98,225,585,290,{size:22,color:"#D7F2E8",font:MONO,valign:"top"});
  textBox(s,"演示重点",790,165,340,44,{size:27,bold:true,color:C.dark});
  bulletBox(s,["行动产生明确数值反馈", "地点移动后自动显示NPC", "周测显示750分成绩与薄弱科目", "继续游戏恢复日期、地点和未完成阶段", "第35天生成大学去向与叙事结局"],790,225,390,245,{size:20,spaceAfter:8});
  textBox(s,"项目结果",790,492,340,40,{size:25,bold:true,color:C.orange});
  textBox(s,"完成了一个可编译、可游玩、可保存、可测试的C++17文字MUD，并通过模块化设计把核心调度与具体功能分离。",790,535,390,100,{size:20,bold:true,color:C.teal,valign:"top"});
  notes(s, "程序演示建议控制在三分钟。先新建玩家并执行一个行动，然后输入地图和方向命令，到达地点后与NPC交谈。现场完整玩到周测需要太久，所以提前准备一个第5天或第34天的存档，演示继续游戏、周测和结局。结束时总结：本项目把面向对象、STL、文件读写、算法与测试落实到一个完整可运行的程序中。");
}

const stagingDir = path.join(workspaceDir, ".codex-finalizer");
await fs.mkdir(stagingDir, { recursive: true });
const candidatePath = path.join(stagingDir, "铃响之前_课堂展示_candidate.pptx");
await (await PresentationFile.exportPptx(deck)).save(candidatePath);

const result = await finalizePresentation({
  explicitTotalSlideCount: 15,
  requiredNativeTableOwnerSlides: [11],
  requiredNativeChartOwnerSlides: [],
  workspaceDir,
  candidatePath,
  finalPath: FINAL_PPTX,
  pythonExecutable: RUNTIME_PYTHON,
  integrityValidatorPath: path.join(SKILL_DIR, "container_tools/inspect_presentation_package_integrity.py"),
  layoutValidatorPath: path.join(SKILL_DIR, "container_tools/inspect_presentation_layout_geometry.py"),
  layoutArgs: ["--expected-slide-size-emu", "12192000,6858000", "--validate-bullet-geometry", "--validate-heading-fit", "--require-native-table-slide", "11"],
  fontPolicy: { basis: "design", families: [FONT] },
  verifyArtifactToolImport: true,
  receiptPath: path.join(stagingDir, "铃响之前_课堂展示_终稿.validation.json"),
});

for (let i = 0; i < deck.slides.items.length; i++) {
  const blob = await deck.export({ slide: deck.slides.items[i], format: "png", scale: 1 });
  await fs.writeFile(path.join(TMP_DIR, `slide-${String(i+1).padStart(2,"0")}.png`), new Uint8Array(await blob.arrayBuffer()));
}
const montage = await deck.export({ format: "webp", montage: true, scale: 0.5 });
await fs.writeFile(path.join(TMP_DIR, "montage.webp"), new Uint8Array(await montage.arrayBuffer()));
console.log(JSON.stringify({ final: FINAL_PPTX, validation: result }, null, 2));
