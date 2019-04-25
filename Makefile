GPP=g++
GPP_FLAGS=-g -Wall
SRC_FOLDER=src
BUILD_FOLDER=build
ENTRY=main
FLAGS = `pkg-config --libs glfw3` -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

SOURCES=$(shell find $(SRC_FOLDER) -type f -name *.cpp)
OBJECTS=$(patsubst $(SRC_FOLDER)/%.cpp, $(BUILD_FOLDER)/%.o, $(SOURCES))

main: build
	./$(BUILD_FOLDER)/$(ENTRY)

build: $(OBJECTS) build_folder
	$(GPP) $(GPP_FLAGS) glad.c -o $(BUILD_FOLDER)/$(ENTRY) $(OBJECTS) $(FLAGS)

$(BUILD_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp build_folder
	$(GPP) $(GPP_FLAGS) -o $@ -c $<

build_folder:
	mkdir -p $(BUILD_FOLDER)


.PHONY: clean
clean:
	rm -rf $(BUILD_FOLDER)
