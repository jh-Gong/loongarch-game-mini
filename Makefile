
ARCH = loongarch
CROSS_COMPILE = loongarch64-linux-gnu-
CFLAGS = -Wall -pthread

# Debug
ifeq ($(DEBUG), no)
    DEBUG_FLAGS =
else
    DEBUG_FLAGS = -DUSE_DEBUG
endif

COMPILER = g++
SOURCE_PATH = ./source/
SOURCES   =$(wildcard $(SOURCE_PATH)*.cpp)
INCLUDES   =-I./include
OBJ = $(patsubst %.cpp, %.o, $(SOURCES))
LIB_PATH = ../lib
LIB_NAME_2 = draw
LIB_NAME_3 = ui

PROJECT_NAME = mineral_hunter

APP_TARGET = test_$(PROJECT_NAME)
LIB_TARGET = lib$(PROJECT_NAME)

all: default

default:$(OBJ)
	@echo "Compile: $(ARCH)"
	@mkdir -p bin
	@rm -rf $(LIB_PATH)/lib$(LIB_NAME_2).so $(LIB_PATH)/lib$(LIB_NAME_3).so
	@ln -s $(LIB_PATH)/lib$(LIB_NAME_2)-$(ARCH).so $(LIB_PATH)/lib$(LIB_NAME_2).so
	@ln -s $(LIB_PATH)/lib$(LIB_NAME_3)-$(ARCH).so $(LIB_PATH)/lib$(LIB_NAME_3).so
	$(CROSS_COMPILE)$(COMPILER) $(OBJ) -o bin/$(APP_TARGET)-$(ARCH) -L$(LIB_PATH) -L/home/gjhhh/Projects/dynamic_system/tslib/usr/lib/ \
	-l$(LIB_NAME_2) -l$(LIB_NAME_3) -lts -pthread
	@cp ./bin/$(APP_TARGET)-$(ARCH) /srv/nfs/bin/$(APP_TARGET)-$(ARCH)
	@rm -rf $(OBJ) /srv/nfs/$(APP_TARGET)-x86  $(LIB_PATH)/lib$(LIB_NAME_2).so $(LIB_PATH)/lib$(LIB_NAME_3).so

#compile
$(SOURCE_PATH)%.o: $(SOURCE_PATH)%.cpp
	$(CROSS_COMPILE)$(COMPILER) $(INCLUDES) $(DEBUG_FLAGS) -fPIC -c $(CFLAGS) $< -o $@
	
.PHONY:clean
clean:
	@echo "Remove linked and compiled files......"
	rm -rf $(OBJ) bin/*