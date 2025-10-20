# ROOT项目专用Makefile
# 项目路径: /Data/xyzhou/fitSin2ThetaW

# ===================== 项目配置 =====================
PROJECT_ROOT := /Data/xyzhou/fitByResBos
TARGET := fitSin2ThetaW
SRC_DIR := $(PROJECT_ROOT)/src
INC_DIR := $(PROJECT_ROOT)/include
BUILD_DIR := $(PROJECT_ROOT)/build

# ===================== ROOT配置 =====================
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --libs)
ROOTGLIBS := $(shell root-config --glibs)

# ===================== 编译器设置 =====================
CXX := g++
CXXFLAGS := -Wall -Wextra -g -O2 -std=c++17 -I$(INC_DIR) $(ROOTCFLAGS)

# 关键修改：添加 LHAPDF 库路径
LHAPDF_PATH := /opt/LHAPDF/6.5.5
LDFLAGS := $(ROOTLIBS) $(ROOTGLIBS) -L$(LHAPDF_PATH)/lib -lMinuit -lMinuit2 -lRooFit -lRooFitCore -lLHAPDF

# ===================== 文件自动发现 =====================
# 支持多种C++源文件扩展名
SRCS := $(wildcard $(SRC_DIR)/*.cxx) $(wildcard $(SRC_DIR)/*.cpp) \
        $(wildcard $(SRC_DIR)/*.cc) $(wildcard $(SRC_DIR)/*.C)

# 检查是否找到源文件
ifeq ($(SRCS),)
  $(error 错误: 在 $(SRC_DIR) 中没有找到任何源文件! 请检查文件扩展名或路径)
endif

# 生成对象文件列表
OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%.o,$(SRCS))

# ===================== 主构建目标 =====================
$(TARGET): $(OBJECTS)
	@echo "正在链接可执行文件: $@"
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
	@echo "构建成功! 可执行文件: $(TARGET)"

# ===================== 编译规则 =====================
# 为每种可能的源文件扩展名定义规则
$(BUILD_DIR)/%.cxx.o: $(SRC_DIR)/%.cxx | $(BUILD_DIR)
	@echo "正在编译: $< (.cxx)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "正在编译: $< (.cpp)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	@echo "正在编译: $< (.cc)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.C.o: $(SRC_DIR)/%.C | $(BUILD_DIR)
	@echo "正在编译: $< (.C)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===================== 目录创建 =====================
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "创建构建目录: $(BUILD_DIR)"

# ===================== 清理规则 =====================
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(TARGET) *.pcm
	@echo "已清理构建文件和可执行文件"

# 更新distclean目标以删除指定的数据文件
.PHONY: distclean
distclean: clean
	@rm -f $(DICT_SOURCE) $(DICT_SOURCE:.cxx=.h)
	@echo "正在删除FBHist数据文件..."
	@rm -f $(PROJECT_ROOT)/data/FBHist.root
	@rm -f $(PROJECT_ROOT)/data/coefficientHist.root
	@rm -f $(PROJECT_ROOT)/data/FBHist[0-7].root
	@rm -f $(PROJECT_ROOT)/data/errorSet/FBHist{0..58}.root
	@rm -f $(PROJECT_ROOT)/output/*.root
	@echo "已删除所有Hist数据文件"
	@echo "已完全清理"

# ===================== 运行目标 =====================
.PHONY: run
run: $(TARGET)
	@echo "正在运行程序..."
	@LD_LIBRARY_PATH=$(shell root-config --libdir):$$LD_LIBRARY_PATH ./$(TARGET)

# ===================== 信息显示目标 =====================
.PHONY: info
info:
	@echo "===== 项目信息 ====="
	@echo "项目根目录: $(PROJECT_ROOT)"
	@echo "源文件目录: $(SRC_DIR)"
	@echo "头文件目录: $(INC_DIR)"
	@echo "构建目录: $(BUILD_DIR)"
	@echo "ROOT编译选项: $(ROOTCFLAGS)"
	@echo "ROOT链接库: $(ROOTLIBS)"
	@echo "LHAPDF路径: $(LHAPDF_PATH)"
	@echo ""
	@echo "===== 源文件检查 ====="
	@echo "搜索模式: $(SRC_DIR)/*.{cxx,cpp,cc,C}"
	@echo "找到的源文件:"
	@ls -l $(SRC_DIR)/*.cxx $(SRC_DIR)/*.cpp $(SRC_DIR)/*.cc $(SRC_DIR)/*.C 2>/dev/null || echo "未找到任何匹配的源文件!"
	@echo ""
	@echo "===== 构建状态 ====="
	@echo "对象文件列表: $(OBJECTS)"
	@echo "构建目录内容:"
	@ls -l $(BUILD_DIR) 2>/dev/null || echo "构建目录不存在"

# ===================== 调试目标 =====================
.PHONY: debug
debug:
	@echo "===== 调试信息 ====="
	@echo "源文件列表: $(SRCS)"
	@echo "对象文件列表: $(OBJECTS)"
	@echo "编译命令: $(CXX) $(CXXFLAGS) -c <source> -o <object>"
	@echo "链接命令: $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)"
	@echo "===== 目录内容 ====="
	@echo "源文件目录内容:"
	@ls -la $(SRC_DIR)
	@echo "构建目录内容:"
	@ls -la $(BUILD_DIR) 2>/dev/null || echo "构建目录不存在"