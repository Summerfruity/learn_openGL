# 编译器
CXX = g++

# 编译选项 (包含头文件目录)
CXXFLAGS = -I./include -g

# 链接库 (SDL2, dl等)
LDFLAGS = -lSDL2 -ldl

# 源文件
SRC = src/main.cpp src/glad.c

# 输出目标
TARGET = build/prog

# 默认动作：输入 make 时执行
all: $(TARGET)

# 编译规则
$(TARGET): $(SRC)
	@mkdir -p build
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

# 清理规则：输入 make clean 时执行
clean:
	rm -f $(TARGET)