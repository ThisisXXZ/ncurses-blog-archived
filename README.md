# ncurses
博客园链接 https://www.cnblogs.com/VeniVidiVici/p/17318232.html

这次 ENGG1340 课程的 group project 是设计并实现一个 **text-base game**，作为终端上运行的 text-base game，有一个出彩的 GUI 肯定是一个加分项！

在未老师的介绍下，我知道了有 `<ncurses.h>` 库这么一个神奇的东西；最重要的是，虽然它不属于 C++ 标准库，但是在学校 server 里居然默认下载好了 (可见其出名的功能强大)
据说很多热门的终端程序，例如 Vim，SL 都用到了 ncurses

花了一个下午学习了一下用法，在这里简单的总结一下，并且附上一些简单 GUI 组成的实例

## Installation
官方 release 网址在[这里](https://invisible-island.net/ncurses/announce.html)

在终端上输入命令 `sudo apt-get install libncurses5` 进行安装

<br />

## Compilation
所有使用了 `<ncurses.h>` 库的程序，在编译时需要添加参数 `-l ncurses` 使编译器链接到 ncurses 库

例: ``g++ -o test test.cpp -lncurses``

<br />

## 初始化窗口对象

使用 `initscr()` 初始化标准窗口对象 stdscr (WINDOW 类)

初始化过后，我们接下来将与该窗口对象进行交互: std::cin/out, scan/printf 等标准输入输出将失效

<br />

## 返回常规终端模式
当使用完 ncurses，想回到常规终端模式时，使用 `endwin()` 来关闭窗口

通常在程序退出前调用 `endwin()`

<br />

## 输入/输出
在调用 `endwin()` 之前，标准输入输出将失效

想要与初始化后的窗口对象交互，我们需要使用 ncurses 库提供的输入/输出函数

```cpp
#include <ncurses.h>

int scanw(char* format, ...);   // 用法与 scanf 相同
int printw(char* format, ...);  // 用法与 printf 相同
int getch(void);                // 用法同 getchar, 有阻碍程序继续的功能
```

由于 gp 中使用了很多 string，我自定义了两个接口用来处理 string 类型的输入输出

```cpp
#include <string>
#include <ncurses.h>

string instr() {
  char s[1024];
  scanw("%[^\n]", s);   // 这里对标的是 getline, 回车时结束读入
  return string(s);
}
void outstr(string s) {
  printw("%s", s.c_str());
}
```

<br />

## 在指定位置输出
在 ncurses 模式中，我们可以通过 `mv()` 函数轻松控制光标的位置，从而实现在终端的指定位置输出

`mv(x, y)` 将光标移动到当前窗口的第 $x$ 行第 $y$ 列 (**行列均从 $0$ 开始**)，且接下来输出的内容都将从该位置开始

以下实例将在**从屏幕中央开始** (注意，是开始而不是位于) 输出 "Hello World!"

```cpp
#include <ncurses.h>

...
int scrLine, scrCol;
getmaxyx(stdscr, scrLine, scrCol);       // 获取标准屏幕的行/列数
move(scrLine / 2 - 1, scrCol / 2 - 1);   // 将光标移至屏幕中央
printw("Hello World!");
...
```

移动光标和输出可以在同一个函数 `mvprintw()` 中完成: 例如上例程序可写成 `mvprintw(scrLine / 2 - 1, scrCol / 2 - 1, "Hello World!")`

在输出完毕后，使用 `refresh()` 进行刷新，将输出显示到当前屏幕上

<br />

## 新窗口/子窗口

### 创建新窗口/子窗口
在未指定的情况下，`initscr()` 初始化的窗口是标准屏幕 `stdscr`

而有时我们需要多个窗口；通过创建新窗口 (`newwin`) 或子窗口 (`subwin`)

创建新窗口时，其会被分配一个新的内存；而子窗口与其父窗口共用内存

```cpp
WINDOW* newwin(int line, int col, int x, int y);                 // 创建新窗口
WINDOW* submin(WINDOW* parent, int line, int col, int x, int y); // 创建子窗口，其父窗口为 parent
```

### 删除新窗口/子窗口
使用 `del(win)` 删除窗口 win，即，释放其所占用的内存

* 删除窗口 win 并**不代表** 屏幕上 win 输出的内容会消失，因此在删除之前调用 `wclear(win)` 与 `wrefresh(win)` 进行清屏

* 不要删除 ncurses 的默认窗口 `stdscr`, 结束它使用 `endwin()` 即可

### 非标准窗口的交互操作
超级简单易懂，之前我们介绍的所有对 `stdscr` 的函数，加上前缀 **w**，再添加对应窗口的指针作为参数就得到了与非标准窗口交互的函数

例如：`printw()` 对应 `wprintw(win)`, `refresh()` 对应 `wrefresh(win)`, `mvprinrw()` 对应 `mvwprintw(win)` (这个位置稍有不同)

```cpp
WINDOW* win = newwin(30, 30, 0, 0);
wmvprintw(1, 1, "Joker");     // 注意，这里的 (1, 1) 是对于窗口 win 的相对位置，而不是对于 stdscr 的绝对位置
wrefresh(win);                // 进行刷新，使内容显示到屏幕上
or
WINDOW* sub = subwin(stdscr, 30, 30, 0, 0);    // sub 是标准窗口的子窗口
wmvprintw(1, 1, "Skull"); 
touchwin(stdscr);                              // 由于 sub 与标准窗口共用内存，我们用 touchwin() 函数标记标准窗口被修改即可，而非使用 wrefresh()
```

<br />

## 窗口转储
这在实现 text-base game 中是一个很实用的功能，它可以用来实现场景的切换，与**读/存档功能**

例如，"开始" 界面可以进入 "游戏" 界面，但从 "游戏" 界面返回时，由于屏幕已经被 "游戏" 界面所在窗口覆盖，我们需要重新加载 "开始" 界面

此时，利用窗口的转储，我们可以直接恢复之前的界面

### 标准窗口转储
标准窗口 stdscr 可以利用下面的两个函数进行存储与读取:

```cpp
int scr_dump(const char*);    // 参数是当前目录下文件的名称: scr_dump 将会把标准窗口中的内容存储到对应名称的文件中
                              // 若当前目录下没有该名称的文件，将会创建一个
int scr_restore(const char*); // scr_restore 函数将会读取对应文件中存储的内容，并将其载入到标准窗口中
```

以下是一个简单的例程 (gp 中的界面转换可以以此为基础)

```cpp
printw("Main Menu\n");          // 标准窗口作为主界面 main menu
refresh();
scr_dump("a");                  // 储存当前标准窗口的内容

getch();
WINDOW* win = newwin(scrLine, scrCol, 0, 0);
wprintw("Game Started!\n");     // 一个与标准窗口相同大小的新窗口 win 作为游戏界面
wrefresh(win);

getch();
wclear(win);
wrefresh(win);                  // 游戏界面结束，将屏幕清空
delwin(win);                    // 释放游戏界面所在窗口 win 的内存

// getch();
scr_restore("a");               // 返回主界面 main menu, 使用 scr_restore() 恢复
refresh();
```

若有时我们需要暂时退出 ncurses 模式，回到行缓冲模式，我们可以储存当前标准窗口的内容后再退出

(或使用 `def_prog_mode()` 与 `reset_prog_mode()` 函数，由于 gp 中可能不会用到，这里不多介绍)

### 非标准窗口转储
当然，除了标准窗口 stdscr，其他任何我们新创建的窗口都可以进行转储

使用以下的两个函数: 

```cpp
int putwin(WINDOW*, FILE*);     // 存
int getwin(WINDOW*, FILE*);     // 读
                                // 注意，这两个函数的文件参数不是文件名 (字符串)，而是文件指针
```

<br />

## 滚屏操作

在行缓冲模式中，若输出的内容超过了终端的 bottom line，将会自动滚屏 (旧的输出将会向上滚动，为新的输出留位置)

我以为 ncurses 中的窗口默认滚屏，结果当输出超出屏幕时，反而向右溢出了

我翻了好久的库文档，终于找到了对应的函数 ([这里](https://manpages.debian.org/bullseye/ncurses-doc/scrollok.3ncurses.en.html))

```cpp
int scrollok(WIN*, bool);        // 在 WIN 指针指向的窗口中开启 (true)/关闭 (false) 滚屏
setscrreg(int x, int line);      // 在 scrollok 开启后，可以在窗口中设立一个滚屏区，在该区域内有滚屏
                                 // 滚屏区从第 [x] 行开始，共占 [line] 行
```

<br />

## 颜色设置

有时候我们希望改变窗口的背景与文本颜色， ncurses 库对此也提供了支持

在使用之前，我们先初始化颜色设置

```cpp
bool has_color(void);            // 返回该环境是否支持颜色设置
int start_color(void);           // 初始化颜色设置
```

在 `start_color()` 成功调用后，一系列的常量将会产生，例 `COLOR` (支持的颜色数量)，`COLOR_BLACK` (黑色), `COLOR_WHITE`...

我们使用 `init_pair()` 函数来创建 **背景-文本** 的颜色对，并用 `attron()` 函数激活

接下来，直到 `attroff()` 关闭之前，所有输出的 背景-文本 都将是指定的颜色对

```cpp
init_pair(1, COLOR_BLACK, COLOR_WHITE);  // [黑]底[白]字为第 1 个颜色对
                                         // 注意，每一种颜色对的编号必须是不同的！
attron(COLOR_PAIR(1));                   // 传入之前定义的颜色对作为参数 (以编号表示)，开启颜色设置
printw("Black background white text\n");
attroff(COLOR_PAIR(1));                  // 关闭颜色设置，恢复默认
                                         // 此处也可以直接 attron 其他颜色对，而无需先 attroff
```

<br />


## Reference

1. [Debian ncurses manpage](https://manpages.debian.org/bullseye/ncurses-doc/)
    * [滚屏 scrollok](https://manpages.debian.org/bullseye/ncurses-doc/scrollok.3ncurses.en.html) 
    * [窗口转储 scr_dump](https://manpages.debian.org/testing/ncurses-bin/scr_dump.5.en.html)
2. [tiga-Unix/Linux下的Curses库开发指南——第三章curses库窗口](https://blog.csdn.net/tingya/article/details/4800120?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-4800120-blog-80753248.235%5Ev28%5Epc_relevant_t0_download&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-4800120-blog-80753248.235%5Ev28%5Epc_relevant_t0_download&utm_relevant_index=2) (这好像也是转载的，但是翻译的很好，也很全)
3. [KeBlog-ncurses](https://kewth.github.io/2019/10/01/ncurses/) (Kewth NB)
4. [ztq-ncurses库 常用函数及基本使用](https://blog.csdn.net/ztq_12345/article/details/100560314) (有一个通过子窗口实现分屏的程序，gp 的时候可以借鉴，修改一下参数)
