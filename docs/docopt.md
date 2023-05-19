
以下是一些常见的docopt.cpp支持的usage格式定义：

1. 基本格式

```
Usage: program [options]

Options:
  -h, --help     Show this help message and exit.
  -v, --version  Show version number and exit.
```

2. 带有参数的选项

```
Usage: program [options]

Options:
  -f FILE, --file=FILE  Path to input file.
  -o FILE, --output=FILE  Path to output file.
  -n NUM, --number=NUM  Number of iterations.
```

3. 带有默认值的选项

```
Usage: program [options]

Options:
  -f FILE, --file=FILE  Path to input file. [default: input.txt]
  -o FILE, --output=FILE  Path to output file. [default: output.txt]
  -n NUM, --number=NUM  Number of iterations. [default: 10]
```

4. 带有可选参数的选项

```
Usage: program [options]

Options:
  -f [FILE], --file=[FILE]  Path to input file.
  -o [FILE], --output=[FILE]  Path to output file.
  -n [NUM], --number=[NUM]  Number of iterations.
```

5. 带有必选参数的命令

```
Usage: program <input_file> [options]

Options:
  -o FILE, --output=FILE  Path to output file.
  -n NUM, --number=NUM  Number of iterations.
```

6. 带有子命令的程序

```
Usage: program <command> [<args>...]

Commands:
  init  Initialize the program.
  run   Run the program.

Options:
  -h, --help     Show this help message and exit.
  -v, --version  Show version number and exit.
```

7. 带有可选参数的子命令

```
Usage: program [options] <command> [<args>...]

Commands:
  init  Initialize the program.
  run   Run the program.

Options:
  -h, --help     Show this help message and exit.
  -v, --version  Show version number and exit.

Command Options:
  -f [FILE], --file=[FILE]  Path to input file.
  -o [FILE], --output=[FILE]  Path to output file.
  -n [NUM], --number=[NUM]  Number of iterations.
```


你可以使用以下代码定义一个带有'-L'选项的usage：

```
Usage: program [options]

Options:
  -h, --help                Show this help message and exit.
  -v, --version             Show version number and exit.
  -L LEVEL, --log-level=LEVEL
                            Set the logging level. Available options: info, debug, warning, error. [default: info]
```

在这个usage中，'-L'选项后面需要跟一个参数LEVEL，可以使用"--log-level=LEVEL"或者"-L LEVEL"来指定。LEVEL参数的值可以是四个选项中的一个：info, debug, warning, error。如果没有指定LEVEL参数，使用默认值info。
