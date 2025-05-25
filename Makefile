CC = g++
CFLAGS = -IFrontend/include -IBackend/include -D_DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

TARGET = run

SRC_DIR_FRONTEND = ./Frontend/src
SRC_DIR_BACKEND = ./Backend/src

BUILD_DIR_MAIN = ./build
BUILD_DIR_FRONTEND = ./Frontend/build
BUILD_DIR_BACKEND = ./Backend/build

BIN_DIR = ./bin
DUMP_DIR = ./Frontend/dump

SRC_MAIN = ./main.cpp
SRC_FRONTEND = $(SRC_DIR_FRONTEND)/vector.cpp $(SRC_DIR_FRONTEND)/tokenizer.cpp $(SRC_DIR_FRONTEND)/parser.cpp $(SRC_DIR_FRONTEND)/tree.cpp 
SRC_BACKEND = $(SRC_DIR_BACKEND)/nasmGen.cpp

OBJ_MAIN = $(BUILD_DIR_MAIN)/main.o
OBJ_FRONTEND = $(BUILD_DIR_FRONTEND)/vector.o $(BUILD_DIR_FRONTEND)/tokenizer.o $(BUILD_DIR_FRONTEND)/parser.o $(BUILD_DIR_FRONTEND)/tree.o
OBJ_BACKEND = $(BUILD_DIR_BACKEND)/nasmGen.o

$(BIN_DIR)/$(TARGET): $(OBJ_MAIN) $(OBJ_FRONTEND) $(OBJ_BACKEND)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	
$(BUILD_DIR_MAIN)/main.o: $(SRC_MAIN)
	@mkdir -p $(BUILD_DIR_MAIN)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/vector.o: $(SRC_DIR_FRONTEND)/vector.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/tokenizer.o: $(SRC_DIR_FRONTEND)/tokenizer.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/parser.o: $(SRC_DIR_FRONTEND)/parser.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_FRONTEND)/tree.o: $(SRC_DIR_FRONTEND)/tree.cpp
	@mkdir -p $(BUILD_DIR_FRONTEND)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_BACKEND)/nasmGen.o: $(SRC_DIR_BACKEND)/nasmGen.cpp
	@mkdir -p $(BUILD_DIR_BACKEND)
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run

clean:
	@rm -rf $(BIN_DIR) $(BUILD_DIR_MAIN) $(BUILD_DIR_FRONTEND) $(BUILD_DIR_BACKEND) $(DUMP_DIR)

run: $(BIN_DIR)/$(TARGET)
	@mkdir -p $(DUMP_DIR)
	@$(BIN_DIR)/$(TARGET)