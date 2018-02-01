all:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build; make
clean:
	rm -rf build
.PHONY: all clean
