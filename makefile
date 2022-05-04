include source.env
include targets.env

.PHONY: all link test clean debug

CC=clang
CXX=clang++
CFLAGS = -std=c11
OPTIM = -O2
CXXFLAGS = -std=c++20 -D _SILENCE_CLANG_CONCEPTS_MESSAGE -m64 -march=x86-64 -Wfatal-errors -W -Wdelete-abstract-non-virtual-dtor -Wno-unused-parameter -Wno-format -Wno-parentheses-equality -Xclang -flto-visibility-public-std -frelaxed-template-template-args
CXXFLAGS_PROF = -std=c++20 -O2 -D TBB_USE_THREADING_TOOLS -g -m64 -march=x86-64 -Wfatal-errors -W -Wdelete-abstract-non-virtual-dtor -Wno-unused-parameter -Wno-format -Wno-parentheses-equality -Xclang -flto-visibility-public-std -frelaxed-template-template-args 
CXX_LINKS = -L./lib/ -lSDL2main -lSDL2 -lSDL2_image -llibsoloud_static
DEBUGFLAGS = -ggdb -D DEBUG
DEFINES = -D SDL_MAIN_HANDLED -D WINDOWS

OBJDIR := out/obj
TESTDIR := out/test
DEPDIR := out/obj
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
TESTDEPFLAGS = -MT $@ -MMD -MP -MF $(TESTDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
POSTCOMPILE_TEST = mv -f $(TESTDIR)/$*.Td $(TESTDIR)/$*.d && touch $@

COMPILE.c = $(CC) $(DEFINES) $(DEPFLAGS) $(CFLAGS) -c
COMPILE.cpp = $(CXX) $(DEFINES) $(DEPFLAGS) $(OPTIM) $(CXXFLAGS) -c
COMPILE.cpp.prof = $(CXX) $(DEFINES) $(CXXFLAGS)
COMPILE.d.cpp = $(CXX) $(DEFINES) $(DEBUGFLAGS)

COMPILE_TEST.c = $(CC) $(DEFINES) $(TESTDEPFLAGS) $(CFLAGS) -c -g
COMPILE_TEST.cpp = $(CXX) $(DEFINES) $(TESTDEPFLAGS) $(CXXFLAGS) -c -g

$(DEPDIR): ; @mkdir -p $@

GAME_SRC = $(MINECRAFT_SRC)
GAME_INC = $(MINECRAFT_INC)
EXE_NAME = $(EXE_NAME_MINECRAFT)

APP_OBJS = \
	$(patsubst src/$(EXE_NAME)/%.cpp,out/obj/$(EXE_NAME)/%.o,$(GAME_SRC)) \
	$(patsubst src/%.cpp,out/obj/%.o,$(ENGINE_SRC)) \
	$(patsubst src/%.c,out/obj/%.o,$(ENGINE_SRC))

APP_OBJS_TEST = \
	$(patsubst src/%.cpp,out/test/%.o,$(TEST_SRC)) \
	$(patsubst src/Slowboi/%.cpp,out/test/Slowboi/%.o,$(TEST_SRC)) \
	$(patsubst src/%.c,out/test/%.o,$(TEST_SRC))

DEPENDS := $(patsubst src/%.cpp,out/obj/%.d,$(ENGINE_SRC) $(GAME_SRC))
DEPENDS := $(patsubst src/$(EXE_NAME)/%.cpp,out/obj/$(EXE_NAME)/%.d,$(DEPENDS))
DEPENDS := $(patsubst src/%.c,out/obj/%.d,$(DEPENDS))

-include $(DEPENDS)

$(OBJDIR)/%.o : src/%.c
	@echo [BUILD C]   $<
	@$(COMPILE.c) $(APP_INC) $(GAME_INC) $(OUTPUT_OPTION) $<

$(OBJDIR)/%.o : src/%.cpp
	@echo [BUILD CXX] $<
	@$(COMPILE.cpp) $(APP_INC) $(GAME_INC) $(GAME_HEADERS) $(OUTPUT_OPTION) $<

$(TESTDIR)/%.o : src/%.c
	$(COMPILE_TEST.c) $(APP_INC) $(OUTPUT_OPTION) $<

$(TESTDIR)/%.o : src/%.cpp
	$(COMPILE_TEST.cpp) $(APP_INC) $(OUTPUT_OPTION) $<

all:
	compiledb make profile -n

	@$(MAKE) mygame
	@$(MAKE) link

	@echo Done

profile:
	$(COMPILE.cpp.prof) $(CXXFLAGS) $(GAME_SRC) $(ENGINE_SRC) -Isrc/ $(GAME_INC) -Iinc/ -o $(EXE_NAME).exe $(CXX_LINKS) -Wno-deprecated
	@echo Done

mygame: $(APP_OBJS)

debug:
	$(COMPILE.d.cpp) -O0 $(CXXFLAGS) $(GAME_SRC) $(ENGINE_SRC) -Isrc/ $(GAME_INC) -Iinc/ -o $(EXE_NAME).d.exe $(CXX_LINKS) -Wno-deprecated

link:
	clang++ -Xclang -flto-visibility-public-std -o $(EXE_NAME).exe out/obj/*.o out/obj/UI/*.o out/obj/$(EXE_NAME)/*.o $(CXX_LINKS)
 
link_test:
	clang++ -Xclang -flto-visibility-public-std -g -o test.exe out/test/*.o out/test/test/*.o $(CXX_LINKS)

test_build: $(APP_OBJS_TEST)

test:
	@$(MAKE) test_build
	@$(MAKE) link_test
	test.exe

# DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
# $(DEPFILES):
# include $(wildcard $(DEPFILES))

clean:
	del /f /q /s out\obj\*.*
	del /f /q /s out\test\*.*
	del /f /q /s ${EXE_NAME_SLOWBOI}.exe 2>nul
	del /f /q /s ${EXE_NAME_SLOWBOI}.d.exe 2>nul
	del /f /q /s ${EXE_NAME_SLOWBOI}.d.exe 2>nul
	del /f /q /s ${EXE_NAME_ADVENTURE}.exe 2>nul
	del /f /q /s ${EXE_NAME_ADVENTURE}.d.exe 2>nul
	del /f /q /s ${EXE_NAME_ADVENTURE}.d.exe 2>nul
	del /f /q /s test.exe 2>nul
	

