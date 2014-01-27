all: clean build

bin/countPrimos-pt-DiegoRocha-LuanAzevedo:
		gcc src/countPrimos-pt-DiegoRocha-LuanAzevedo.c -std=c99 -lm -o3 -o bin/countPrimos-pt-DiegoRocha-LuanAzevedo

build: bin/countPrimos-pt-DiegoRocha-LuanAzevedo

build-debug: clean
		gcc src/countPrimos-pt-DiegoRocha-LuanAzevedo.c -DDEBUG -std=c99 -lm -g -o bin/countPrimos-pt-DiegoRocha-LuanAzevedo

clean:
		rm -f bin/countPrimos-pt-DiegoRocha-LuanAzevedo
		
test: build
		bin/countPrimos-pt-DiegoRocha-LuanAzevedo 1 10000000 10

debug-test: build-debug
		gdb --args bin/countPrimos-pt-DiegoRocha-LuanAzevedo 1 10000000 10
