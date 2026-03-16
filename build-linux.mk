all: build-keyindex build-hash_index build-bindatfile

build-keyindex:
	cd keyindex && make -f build-linux && cd ..

build-hash_index:
	cd hash_index && make -f build-linux && cd ..

build-bindatfile:
	cd bindatfile && make -f build-linux && cd ..

clean:
	rm build/*