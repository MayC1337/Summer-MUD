#include "ConsoleUI.h"
#include "TitleArt.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace
{
bool isWideCodePoint(unsigned int codePoint)
{
    return (codePoint >= 0x1100 && codePoint <= 0x115F) ||
           (codePoint >= 0x2E80 && codePoint <= 0xA4CF) ||
           (codePoint >= 0xAC00 && codePoint <= 0xD7A3) ||
           (codePoint >= 0xF900 && codePoint <= 0xFAFF) ||
           (codePoint >= 0xFE10 && codePoint <= 0xFE6F) ||
           (codePoint >= 0xFF01 && codePoint <= 0xFF60) ||
           (codePoint >= 0xFFE0 && codePoint <= 0xFFE6);
}

void printRule(
    const char *left,
    const char *right,
    const std::string &title,
    int width)
{
    const std::string decorated = title.empty() ? "" : " " + title + " ";
    const int titleWidth = ConsoleUI::displayWidth(decorated);
    const int available = std::max(0, width - titleWidth);
    const int before = available / 2;
    const int after = available - before;

    std::cout << left;
    for (int i = 0; i < before; ++i) std::cout << "═";
    std::cout << decorated;
    for (int i = 0; i < after; ++i) std::cout << "═";
    std::cout << right << '\n';
}
}

int ConsoleUI::displayWidth(const std::string &text)
{
    int width = 0;
    for (std::size_t i = 0; i < text.size();)
    {
        const unsigned char first = static_cast<unsigned char>(text[i]);
        unsigned int codePoint = first;
        std::size_t length = 1;

        if ((first & 0xE0) == 0xC0 && i + 1 < text.size())
        {
            codePoint = ((first & 0x1F) << 6) |
                        (static_cast<unsigned char>(text[i + 1]) & 0x3F);
            length = 2;
        }
        else if ((first & 0xF0) == 0xE0 && i + 2 < text.size())
        {
            codePoint = ((first & 0x0F) << 12) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 2]) & 0x3F);
            length = 3;
        }
        else if ((first & 0xF8) == 0xF0 && i + 3 < text.size())
        {
            codePoint = ((first & 0x07) << 18) |
                        ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(text[i + 3]) & 0x3F);
            length = 4;
        }

        width += isWideCodePoint(codePoint) ? 2 : 1;
        i += length;
    }
    return width;
}

bool ConsoleUI::isInteractive()
{
#ifdef _WIN32
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(fileno(stdout)) != 0;
#endif
}

void ConsoleUI::clearScreen()
{
    if (!isInteractive()) return;
#ifdef _WIN32
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode))
    {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    std::cout << "\x1b[2J\x1b[H";
}

void ConsoleUI::typeLine(const std::string &text, int delayMilliseconds)
{
    if (!isInteractive() || delayMilliseconds <= 0)
    {
        std::cout << text << '\n';
        return;
    }

    for (std::size_t i = 0; i < text.size();)
    {
        const unsigned char first = static_cast<unsigned char>(text[i]);
        std::size_t length = 1;
        if ((first & 0xE0) == 0xC0) length = 2;
        else if ((first & 0xF0) == 0xE0) length = 3;
        else if ((first & 0xF8) == 0xF0) length = 4;
        length = std::min(length, text.size() - i);
        std::cout.write(text.data() + i, static_cast<std::streamsize>(length));
        std::cout.flush();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(delayMilliseconds));
        i += length;
    }
    std::cout << '\n';
}

void ConsoleUI::showSplash()
{
    clearScreen();
    int terminalWidth = 80;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        terminalWidth = info.srWindow.Right - info.srWindow.Left + 1;
#endif
    // 宽窗口显示双列笔画，窄窗口自动缩小，避免长行折断标题。
    const int scale = terminalWidth >= 112 ? 2 : 1;
    const int titleWidth = TitleArt::SIZE * 4 * scale + 6;
    const auto centered = [titleWidth](const std::string& text)
    {
        return std::string(static_cast<std::size_t>(
            std::max(0, (titleWidth - displayWidth(text)) / 2)), ' ') + text;
    };
    std::cout << '\n';
    if (terminalWidth >= titleWidth + 1)
    {
        for (int y = 0; y < TitleArt::SIZE; ++y)
        {
            std::string row;
            for (int letter = 0; letter < 4; ++letter)
            {
                if (letter != 0) row += "  ";
                for (int x = 0; x < TitleArt::SIZE; ++x)
                    for (int repeat = 0; repeat < scale; ++repeat)
                        row += TitleArt::GLYPHS[letter][y][x] == '#' ? "█" : " ";
            }
            std::cout << row << '\n' << std::flush;
            if (isInteractive())
                std::this_thread::sleep_for(std::chrono::milliseconds(45));
        }
    }
    else
    {
        std::cout << "  铃  响  之  前\n";
    }
    std::cout << '\n';
    typeLine(centered("铃  响  之  前"), 30);
    typeLine(centered("—— 高考倒计时 35 天 ——"), 18);
    std::cout << '\n';
    typeLine(centered("你还有 35 天，写下自己的答案。"), 20);
    if (isInteractive())
    {
        std::cout << centered("正在翻开日历  [") << std::flush;
        for (int i = 0; i < 12; ++i)
        {
            std::cout << "=" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(35));
        }
        std::cout << "]\n";
    }
    std::cout << '\n';
}

void ConsoleUI::showPeriod(int period, int stamina, int stress)
{
    const char* titles[] = {"晨间 · 校门与晨光", "午间 · 饭盒与树荫",
        "下午 · 黑板与试卷", "夜晚 · 台灯与月亮"};
    const char* scenery[] = {"    \\ | /          .---- 校门 ----.",
        "       /\\_/\\          .-------.", "   .------- 今日课程 -------.",
        "       *       )        .---.    *"};
    const char* details[] = {"  -- ( ) --        |              |",
        "      ( o.o )         | 饭盒  |", "   |   阅读 / 思考 / 提问    |",
        "                       /___/   台灯"};
    const int index = std::clamp(period, 0, 3);
    const char* transitions[] = {"晨光亮起，校门前响起脚步声。", "午饭铃响，饭香从走廊飘来。",
        "翻开试卷，窗外树影轻轻摇晃。", "台灯亮起，月光落在窗台上。"};
    typeLine(transitions[index], 8);
    boxTop(titles[index]);
    boxLine(scenery[index]);
    boxLine(details[index]);
    boxLine(stamina <= 20 ? "          ( -.- ) z Z" : stress >= 70 ?
        "          ( >_< ) ;" : "          ( ^.^ )");
    boxLine(index == 0 ? "         /| []|>       背上书包，今天也出发吧。" :
        index == 1 ? "         /|___|\\       午间留一点时间给自己。" :
        index == 2 ? "       ___/___/___     把问题一步一步解开。" :
        "       ___/___/___     世界安静下来，选择属于你。");
    boxBottom();
}

void ConsoleUI::waitForExit(const std::string &message)
{
    if (!isInteractive()) return;

    std::cin.clear();
    std::string line;
    while (true)
    {
        std::cout << "\n" << message << std::flush;
        if (!std::getline(std::cin, line) || line == "0")
            return;
        std::cout << "请输入数字 0 后再退出。\n";
    }
}

void ConsoleUI::boxTop(const std::string &title, int width)
{
    if (isInteractive()) std::cout << "\x1b[36m";
    printRule("╔", "╗", title, width);
    if (isInteractive()) std::cout << "\x1b[0m";
}

void ConsoleUI::message(const std::string& text, bool error)
{
    if (isInteractive()) std::cout << (error ? "\x1b[33m" : "\x1b[32m");
    std::cout << text;
    if (isInteractive()) std::cout << "\x1b[0m";
    std::cout << '\n';
}

void ConsoleUI::boxDivider(const std::string &title, int width)
{
    printRule("╠", "╣", title, width);
}

void ConsoleUI::boxLine(const std::string &text, int width)
{
    const int maximumWidth = std::max(0, width - 2);
    std::string visible = text;
    if (displayWidth(visible) > maximumWidth)
    {
        visible.clear();
        int used = 0;
        for (std::size_t i = 0; i < text.size();)
        {
            const unsigned char first = static_cast<unsigned char>(text[i]);
            unsigned int codePoint = first;
            std::size_t length = 1;
            if ((first & 0xE0) == 0xC0 && i + 1 < text.size())
            {
                codePoint = ((first & 0x1F) << 6) |
                            (static_cast<unsigned char>(text[i + 1]) & 0x3F);
                length = 2;
            }
            else if ((first & 0xF0) == 0xE0 && i + 2 < text.size())
            {
                codePoint = ((first & 0x0F) << 12) |
                            ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(text[i + 2]) & 0x3F);
                length = 3;
            }
            const int characterWidth = isWideCodePoint(codePoint) ? 2 : 1;
            if (used + characterWidth > maximumWidth - 3) break;
            visible.append(text, i, length);
            used += characterWidth;
            i += length;
        }
        visible += "...";
    }

    const int contentWidth = displayWidth(visible);
    std::cout << "║ " << visible;
    for (int i = contentWidth; i < width - 2; ++i) std::cout << ' ';
    std::cout << " ║\n";
}

void ConsoleUI::boxBottom(int width)
{
    printRule("╚", "╝", "", width);
}

std::string ConsoleUI::makeBar(int value, int maximum, int cells)
{
    value = std::clamp(value, 0, maximum);
    const int filled = maximum == 0 ? 0 : (value * cells + maximum - 1) / maximum;
    std::string result;
    for (int i = 0; i < cells; ++i)
        result += i < filled ? "■" : "·";
    return result;
}
