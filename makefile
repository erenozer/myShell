all: clean compile run

compile: main.cpp File.cpp RegularFile.cpp SoftLinkedFile.cpp Directory.cpp
	@echo "-----------------------------------------"
	@echo "Compiling..."
	@g++ -std=c++11 -o output main.cpp File.cpp RegularFile.cpp SoftLinkedFile.cpp Directory.cpp
	@echo "Compilation successful."

run:
	@echo "-----------------------------------------"
	@echo "Running the program..."
	@echo "======================================================================="
	./output
	@echo "======================================================================="
	@echo "Program completed."

clean:
	@echo "-----------------------------------------"
	@echo "Removing compiled files..."
	@rm -f *.o
	@rm -f output
	@echo "Removed compiled files."