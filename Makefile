# Source directory
SOURCE_DIR:=./src
# Object directory
OBJECT_DIR:=./obj

# Linker flags
LDFLAGS =-lSDL -lSDLmain -lm -lpthread
# Compiler flags
CXXFLAGS +=-Wall -Wno-strict-aliasing -std=c++11 -I$(SOURCE_DIR)

# Enable debug with make DEBUG=1, default: disabled
DEBUG ?= 0

# Object directory and debug/release flags
ifeq ($(DEBUG), 1)
	OBJECT_DIR:=$(OBJECT_DIR)/debug
	CXXFLAGS+= -g -D__DEBUG__ -O0
else
	OBJECT_DIR:=$(OBJECT_DIR)/release
	CXXFLAGS+= -O3 
endif

# binary name and location
TARGET:=proXo

# Find all .cpp files in source directory
SOURCES:=$(shell find $(SOURCE_DIR) -type f -name "*.cpp")

# Generate .o paths from source paths
OBJECTS:=$(patsubst $(SOURCE_DIR)/%, $(OBJECT_DIR)/%, $(SOURCES:.cpp=.o))

all: $(TARGET)

# Link objects and build binary
$(TARGET): $(OBJECTS)
	@echo -e '\e[1;34mLink and build "$@"\e[0m'
	@g++ $+ $(LDFLAGS) -o $@

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

# Compile sources
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@# make object directory
	@mkdir -p $(@D)
	@echo -e '\e[1;34mCompile "$<"\e[0m'
	@# generate .o
	@g++ -c $(CXXFLAGS) $< -o $@
	@# generate .d
	@g++ -MM $(CXXFLAGS) $< > $(OBJECT_DIR)/$*.d
	@# rename .d to .d.tmp
	@mv -f $(OBJECT_DIR)/$*.d $(OBJECT_DIR)/$*.d.tmp
	@# in .d.tmp, replace recipe name to match obj directory, save in .d
	@sed -e 's|.*:|$(OBJECT_DIR)/$*.o:|' < $(OBJECT_DIR)/$*.d.tmp > $(OBJECT_DIR)/$*.d
	@# in .d.tmp remove recipe name, put all dependencies in multiple lines, append to .d
	@sed -e 's/.*://' -e 's/\\$$//' < $(OBJECT_DIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(OBJECT_DIR)/$*.d
	@# remove .d.tmp
	@rm -f $(OBJECT_DIR)/$*.d.tmp

clean:
	@echo -e '\e[1;34mRemove all *.o and *.d files\e[1;34m'
	@rm -f $(OBJECTS) $(OBJECTS:.o=.d)
