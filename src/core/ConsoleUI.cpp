#include "ConsoleUI.h"

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
    const char *const logo[] = {
        "  ███████╗██╗   ██╗███╗   ███╗███╗   ███╗███████╗██████╗ ",
        "  ██╔════╝██║   ██║████╗ ████║████╗ ████║██╔════╝██╔══██╗",
        "  ███████╗██║   ██║██╔████╔██║██╔████╔██║█████╗  ██████╔╝",
        "  ╚════██║██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══╝  ██╔══██╗",
        "  ███████║╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║███████╗██║  ██║",
        "  ╚══════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝",
        "                         M  U  D                         "};

    for (const char *line : logo)
    {
        typeLine(line, 1);
        if (isInteractive())
            std::this_thread::sleep_for(std::chrono::milliseconds(45));
    }

    std::cout << '\n';
    typeLine("              中国式高三生活模拟", 18);
    typeLine("        你还有 35 天，写下自己的答案。", 18);

    if (isInteractive())
    {
        std::cout << "\n  正在翻开日历 ";
        for (int i = 0; i < 12; ++i)
        {
            std::cout << "■" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(45));
        }
        std::cout << "\n\n";
    }
    else
    {
        std::cout << '\n';
    }
}

void ConsoleUI::boxTop(const std::string &title, int width)
{
    printRule("╔", "╗", title, width);
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
