### FrameBuffer颜色测试程序

### Compile:
```shell
    cmake . && make
```

### Usage:

```shell
./fbtest [option]
```

### Options

```shell
    -h this message
    -m display mode
         0: auto display rgb
         1: press enter key to control increase range one(0x00-->0x01)
         2: press enter key to control increase range double(0x02-->0x04)
    -d switch play on fb1 or fb0
         default /dev/fb0
```
