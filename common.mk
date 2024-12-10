APP = $(notdir $(CURDIR))
all: $(APP)

CFLAGS= -Wall -Werror -O3 -ggdb --std=c++2b -MMD -MP -I$(PWD) -I$(PWD)/..
LDFLAGS=

CXX=clang++
CPP=main.cpp
HDR=
SRC=$(CPP) $(HDR)

OUT = out
OBJ = $(addprefix $(OUT)/, $(CPP:.cpp=.o))
DEPS = $(addprefix $(OUT)/, $(CPP:.cpp=.d))

-include $(DEPS)

debug:
	@echo deps: $(DEPS)
	@echo objs: $(OBJ)

$(OUT)/%.o: %.cpp
	@mkdir -p $(dir $(OUT)/$<)
	$(CXX) $(CFLAGS) $< -c -o$@ -MF$(@:.o=.d)

$(APP): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(OBJ) $(APP)
	rm -rf $(OUT)

.PHONY: watch
watch:
	while true; do \
		inotifywait -qr -e modify -e create -e delete -e move Makefile $(SRC); \
		$(MAKE); \
	done

.PHONY: run
run: $(APP)
	./$(APP) "$(ARGS)"

.PHONY: test
test: $(APP)
	@./$(APP) > output.txt
	@diff  output.txt expected.txt || exit 1
