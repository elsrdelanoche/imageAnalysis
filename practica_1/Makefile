CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra $(shell pkg-config --cflags gtk+-3.0 gdk-pixbuf-2.0)
LDFLAGS  := $(shell pkg-config --libs gtk+-3.0 gdk-pixbuf-2.0)

INCLUDES := -Iinclude
SRC_DIR  := src
OBJ_DIR  := build

SRCS := \
  $(SRC_DIR)/main.cpp \
  $(SRC_DIR)/ui/view.cpp \
  $(SRC_DIR)/ui/controller.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET := image_channel_extractor

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

