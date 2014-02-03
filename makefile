all: clean build

bin/multmatrix:
		gcc src/multmatrix.c -pthread -std=c99 -lm -o3 -o bin/multmatrix

build: bin/multmatrix

build-debug: clean
		gcc src/multmatrix.c -DDEBUG -pthread -fopenmp -std=c99 -lm -g -o bin/multmatrix

build-omp: clean
		gcc src/multmatrix.c -pthread -fopenmp -std=c99 -lm -o3 -o bin/multmatrix

clean:
		rm -f bin/multmatrix

run: build
		bin/multmatrix
		
run-omp: clean build-omp
		bin/multmatrix

generate-files:
		python src/createTestMatrix.py

test: clean build generate-files
		bin/multmatrix

test-omp: build-omp generate-files
		bin/multmatrix

debug-test: build-debug generate-files
		gdb bin/multmatrix
