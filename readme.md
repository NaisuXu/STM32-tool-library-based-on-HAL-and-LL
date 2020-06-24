# 基于STM32 HAL和LL的工具库

[toc]

## 基础说明

现在的STM32系列微控制器程序开发ST官方主要提供了HAL和LL两种库，HAL库对很多功能进行了抽象封装，使用起来比传统的STD库要方便许多，但和很多偏向于软件开发的Arduino、Micropython等来说使用的时候还不是非常方便。这里对自己常用的一些功能在官方库的基础上进行了进一步的封装，这就是这个库的由来。

该库完全使用ST官方的HAL和LL库，不改变任何官方库代码。该库大部分由C++编写而成，C++项目才能使用（根据情况可能需要手动将main.c和stm32fxxx_it.c重命名为.cpp）。除了这点外该库可以直接在在STM32CubeMX生成的项目上直接使用。



## 库功能与使用说明

该库可以实现的功能和使用说明可以参考 lib 文件夹中的 [readme.md](lib/readme.md) 文档。



## 例程与演示

该库的例程与演示可以在 examples 文件夹中找到。



## 许可 / LICENSE

该库基于MIT许可，具体内容详见 [LICENSE.md](LICENSE.md) 文档。

This library is under the MIT licence, see the [LICENSE.md](LICENSE.md) document.