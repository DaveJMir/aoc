APP = $(notdir $(CURDIR))
all: $(APP)

CFLAGS= -Wall -Werror -O3 -ggdb --std=c++23 -MMD -MP -I$(PWD) -I$(PWD)/..
LDFLAGS=

CXX= $(shell brew --prefix llvm@18)/bin/clang++
CPP=main.cpp
HDR=
SRC=$(CPP) $(HDR)

OUT = ../out/$(APP)/
OBJ = $(addprefix $(OUT)/, $(CPP:.cpp=.o)) ../out/harness.o
DEPS = $(addprefix $(OUT)/, $(CPP:.cpp=.d))

-include $(DEPS)

debug:
	@echo deps: $(DEPS)
	@echo objs: $(OBJ)

../out/harness.o: ../harness.cpp
	@mkdir -p ../out
	$(CXX) $(CFLAGS) $< -c -o$@ -MF$(@:.o=.d)

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
	./$(APP) && echo OK || echo fail
