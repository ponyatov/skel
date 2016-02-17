CORE = .
CORE = ../Y
log.log: ../Y/skel/skel.bI ./exe.exe
	./exe.exe < $< > $@ && tail $(TAIL) log.log
C = $(CORE)/cpp.cpp ypp.tab.cpp lex.yy.c
H = $(CORE)/hpp.hpp ypp.tab.hpp
CXXFLAGS = -I$(CORE) -I. -std=gnu++11 \
		   -DMODULE=\"$(notdir $(CURDIR))\" -DOS=\"$(OS)\" -DEXE=\"$(EXE)\"
./exe.exe: $(C) $(H) Makefile
	$(CXX) $(CXXFLAGS) -o $@ $(C)
ypp.tab.cpp: ../Y/ypp.ypp
	bison $<
lex.yy.c: ../Y/lpp.lpp
	flex $<
