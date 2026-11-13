# 1. コンパイラの自動選択（環境変数 CC が未指定の場合、利用可能なものを自動検出）
ifeq ($(origin CC), default)
    CC := $(shell \
        command -v clang 2>/dev/null || \
        command -v gcc 2>/dev/null || \
        command -v x86_64-w64-mingw32-gcc 2>/dev/null || \
        command -v i686-w64-mingw32-gcc 2>/dev/null || \
        echo cc)
endif

# 2. ターゲット名と拡張子の自動判定（Windows または MinGW 使用時は .exe を付与）
TARGET_BASE = kunten
ifeq ($(OS),Windows_NT)
    TARGET = $(TARGET_BASE).exe
else ifneq (,$(findstring mingw,$(CC)))
    TARGET = $(TARGET_BASE).exe
else
    TARGET = $(TARGET_BASE)
endif

# 3. コンパイル・リンクオプション
CPPFLAGS ?=

# CCにmingwが含まれている場合はWindows環境と判定
ifneq ($(findstring mingw,$(CC)),)
    CPPFLAGS += -DWINDOWSENV
endif
# -MMD -MP により、ヘッダーファイルの依存関係（.d）を自動生成・追跡します
CFLAGS  = -Wall -Wextra -O2 -MMD -MP
% 上記では警告メッセージ多数表示されるが make CFLAGS='-O2 -MMD -MP' とすると当座の警告は非表示
LDFLAGS =
# もし my_math.c 等で <math.h> の関数を使っていてリンクエラーが出る場合は下のコメントを外してください
# LDFLAGS += -lm

# 4. ソース・オブジェクト・依存関係ファイルの定義
SRCS = kunten_main.c kunten.c my_math.c tex_command.c utf_string.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

# デフォルトターゲット
.PHONY: all
all: $(TARGET)

# リンク処理（更新された .o ファイルのみ結合）
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 分割コンパイル処理（変更された .c または関連する .h のみを再コンパイル）
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# 自動生成されたヘッダー依存関係ファイルの読み込み
-include $(DEPS)

# 静的解析・厳格チェック用（.o を生成せずにチェックのみ実行）
LINT_FLAGS = -fsyntax-only -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion
# -fsyntax-only ：チェックだけを行い、実行ファイルや .o ファイルを生成せず
# -Wshadow      ：変数のシャドーイング（外側と同じ名前の変数を内側で宣言するバグの元）を検知
# -Wformat=2    ：printf などのフォーマット指定子の不整合を厳しくチェック
# -Wconversion  ：暗黙の型変換の警告

.PHONY: lint
lint:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --suppress=missingIncludeSystem $(SRCS); \
	else \
		$(CC) $(LINT_FLAGS) $(SRCS); \
	fi

# クリーンアップ処理（make clean）
.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET_BASE) $(TARGET_BASE).exe

# MinGW クロスコンパイル用の便利なショートカット
.PHONY: win64 win32
win64:
	$(MAKE) CC=x86_64-w64-mingw32-gcc

win32:
	$(MAKE) CC=i686-w64-mingw32-ecc
