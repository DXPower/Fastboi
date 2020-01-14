include source.env
include targets.env

.PHONY: all link test clean

#-static-libgcc -static-libstdc++

CC=clang
CXX=clang++
CFLAGS = -std=c11
CXXFLAGS = -std=c++17 -m64 -march=x86-64 -Wfatal-errors -W -Wdelete-abstract-non-virtual-dtor -Wno-unused-parameter -Wno-format -Xclang -flto-visibility-public-std 
CXX_LINKS = -L./lib/ -lSDL2main -lSDL2 -lSDL2_image
# DEBUGFLAGS = -g -D DEBUG
DEFINES = -D SDL_MAIN_HANDLED -D WINDOWS

OBJDIR := out/obj
TESTDIR := out/test
DEPDIR := out/obj
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
TESTDEPFLAGS = -MT $@ -MMD -MP -MF $(TESTDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
POSTCOMPILE_TEST = mv -f $(TESTDIR)/$*.Td $(TESTDIR)/$*.d && touch $@

COMPILE.c = $(CC) $(DEFINES) $(DEPFLAGS) $(CFLAGS) -c
COMPILE.cpp = $(CXX) $(DEFINES) $(DEPFLAGS) $(CXXFLAGS) -c

COMPILE_TEST.c = $(CC) $(DEFINES) $(TESTDEPFLAGS) $(CFLAGS) -c
COMPILE_TEST.cpp = $(CXX) $(DEFINES) $(TESTDEPFLAGS) $(CXXFLAGS) -c

$(DEPDIR): ; @mkdir -p $@

APP_OBJS = \
	$(patsubst src/%.cpp,out/obj/%.o,$(APP_SRC)) \
	$(patsubst src/Slowboi/%.cpp,out/obj/Slowboi/%.o,$(APP_SRC)) \
	$(patsubst src/%.c,out/obj/%.o,$(APP_SRC))

APP_OBJS_TEST = \
	$(patsubst src/%.cpp,out/test/%.o,$(TEST_SRC)) \
	$(patsubst src/Slowboi/%.cpp,out/test/Slowboi/%.o,$(TEST_SRC)) \
	$(patsubst src/%.c,out/test/%.o,$(TEST_SRC))

# APP_OBJS_DEBUG = $(patsubst src/%.cpp,out/obj/%.d.o,$(APP_SRC)) $(patsubst src/Slowboi/%.cpp,out/obj/Slowboi/%.o,$(APP_SRC)) 

DEPENDS := $(patsubst src/%.cpp,out/obj/%.d,$(APP_SRC))
DEPENDS := $(patsubst src/Slowboi/%.cpp,out/obj/Slowboi/%.d,$(DEPENDS))
DEPENDS := $(patsubst src/%.c,out/obj/%.d,$(DEPENDS))

-include $(DEPENDS)

$(OBJDIR)/%.o : src/%.c Makefile
	$(COMPILE.c) $(APP_INC) $(OUTPUT_OPTION) $<

$(OBJDIR)/%.o : src/%.cpp Makefile
	$(COMPILE.cpp) $(APP_INC) $(OUTPUT_OPTION) $<

$(TESTDIR)/%.o : src/%.c Makefile
	$(COMPILE_TEST.c) $(APP_INC) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE_TEST)

$(TESTDIR)/%.o : src/%.cpp Makefile
	$(COMPILE_TEST.cpp) $(APP_INC) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE_TEST)

# $(OBJDIR)/%.d.o : src/%.cpp Makefile
# 	$(COMPILE.cpp) $(DEBUGFLAGS) $(APP_INC) $(OUTPUT_OPTION) $<
# 	$(POSTCOMPILE)

all:
	# @echo $(DEPENDS)
	@$(MAKE) mygame
	@$(MAKE) link

	@printf "\nDone\n"

# debug:
# 	@$(MAKE) mygame_debug
# 	@$(MAKE) link

# 	@printf "\nDebug build Done\n"

mygame: $(APP_OBJS)

# mygame_debug: $(APP_OBJS_DEBUG)

link:
	clang++ -Xclang -flto-visibility-public-std -o $(EXE_NAME).exe out/obj/*.o out/obj/Slowboi/*.o $(CXX_LINKS)

link_test:
	clang++ -Xclang -flto-visibility-public-std -o test.exe out/test/*.o out/test/Slowboi/*.o out/test/test/*.o $(CXX_LINKS)

test_build: $(APP_OBJS_TEST)

test:
	@$(MAKE) test_build
	@$(MAKE) link_test
	test.exe

# DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
# $(DEPFILES):
# include $(wildcard $(DEPFILES))

clean:
	rm -f out/obj/*.*
	rm -f out/obj/Slowboi/*
	rm -f out/test/*.*
	rm -f out/test/Slowboi/*
	rm -f out/test/test/*
	rm -f ${EXE_NAME}.exe
	rm -f ${EXE_NAME}.d.exe
	rm -f test.exe
	rm -f ${EXE_NAME}.out
	rm -f ${EXE_NAME}.d.out
	rm -f makefile_old
	rm -f source_old.env
