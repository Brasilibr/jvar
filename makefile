##################Pasta:sub-make ## Makefile ###################
CC=g++
CPPFLAGS=-fno-trapping-math -Werror=multichar -Werror=overflow -pedantic -O3 -std=c++17 -D_FORCE_INLINES -I./src/include -I/usr/include/mysql
LIBS =-lm -lmysqlclient -lpthread -lcrypto -lz -lssl #### -lcurl####
EXEC=serv.exe
SRC_DIR := src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
SRC_CFILES := $(wildcard $(SRC_DIR)/*.c)
DEP_FILES := $(wildcard $(SRC_DIR)/*.h)
DEP_FILES2 := $(wildcard $(SRC_DIR)/*.hpp)
_OBJ= $(SRC_FILES:.cpp=.ocpp)
_OBJC= $(SRC_CFILES:.c=.oc)
ODIR=obj
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
OBJC = $(patsubst %,$(ODIR)/%,$(_OBJC))

LIB_SRC_DIR := lib
LIB_SRC_FILES := $(wildcard $(LIB_SRC_DIR)/*.cpp)
LIB_SRC_CFILES := $(wildcard $(LIB_SRC_DIR)/*.c)
LIB_DEP_FILES := $(wildcard $(LIB_SRC_DIR)/*.h)
LIB_DEP_FILES2 := $(wildcard $(LIB_SRC_DIR)/*.hpp)

LIB_ODIR=objlib
LIB_OBJ= $(LIB_SRC_FILES:.cpp=.ocpp)
LIB_OBJC= $(LIB_SRC_CFILES:.c=.oc)
LIB_OBJ_FULL = $(patsubst %,$(LIB_ODIR)/%,$(LIB_OBJ))
LIB_OBJC_FULL = $(patsubst %,$(LIB_ODIR)/%,$(LIB_OBJC))

LEVEL2_SRC_DIR := src
LEVEL2_SRC_FILES := $(wildcard $(LEVEL2_SRC_DIR)/*/*.cpp)
LEVEL2_SRC_CFILES := $(wildcard $(LEVEL2_SRC_DIR)/*/*.c)
LEVEL2_DEP_FILES := $(wildcard $(LEVEL2_SRC_DIR)/*/*.h)
LEVEL2_DEP_FILES2 := $(wildcard $(LEVEL2_SRC_DIR)/*/*.hpp)

LEVEL2_ODIR=objlvl2
LEVEL2_OBJ= $(LEVEL2_SRC_FILES:.cpp=.ocpp)
LEVEL2_OBJC= $(LEVEL2_SRC_CFILES:.c=.oc)
LEVEL2_OBJ_FULL = $(patsubst %,$(LEVEL2_ODIR)/%,$(LEVEL2_OBJ))
LEVEL2_OBJC_FULL = $(patsubst %,$(LEVEL2_ODIR)/%,$(LEVEL2_OBJC))

$(info  Cpp Objects:  $(OBJ))
$(info  C Objects:  $(OBJC))
$(info  Lib Cpp Objects:  $(LIB_OBJ_FULL))
$(info  Lib C Objects:  $(LIB_OBJC_FULL))

all: $(EXEC)
	@echo "Compilation Finished"
	
serv.exe: $(OBJ) $(OBJC) $(LIB_OBJ_FULL) $(LIB_OBJC_FULL) $(LEVEL2_OBJ_FULL) $(LEVEL2_OBJC_FULL)
# $@ = nome do case
# $^ = objects do case
	$(CC) -o $@ $^ $(LIBS) 

$(ODIR)/%.ocpp: %.cpp $(DEP_FILES) $(DEP_FILES2) $(LIB_DEP_FILES) $(LIB_DEP_FILES2) $(LEVEL2_DEP_FILES) $(LEVEL2_DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)

$(ODIR)/%.oc: %.c $(DEP_FILES) $(DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)

$(LIB_ODIR)/%.ocpp: %.cpp $(LIB_DEP_FILES) $(LIB_DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)

$(LIB_ODIR)/%.oc: %.c $(LIB_DEP_FILES) $(LIB_DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)

$(LEVEL2_ODIR)/%.ocpp: %.cpp $(LEVEL2_DEP_FILES) $(LEVEL2_DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)

$(LEVEL2_ODIR)/%.oc: %.c $(LEVEL2_DEP_FILES) $(LEVEL2_DEP_FILES2)
	$(CC) -o $@ -c $< $(CPPFLAGS)



.PHONY: clean cleanall

cleanLib: 
	rm -rf $(LIB_ODIR)/*.o*
	rm -rf $(LIB_ODIR)/$(LIB_SRC_DIR)/*.o*
clean: 
	rm -rf $(ODIR)/*.o*
	rm -rf $(ODIR)/$(SRC_DIR)/*.o*
	rm -rf $(LEVEL2_ODIR)/*.o*
	rm -rf $(LEVEL2_ODIR)/$(SRC_DIR)/*/*.o*
cleanall: clean cleanLib
	rm -rf $(EXEC)
