all: bsd test_bsd test_radixter
.PHONY : all

init_radixter: radixter
	@echo ""
	@echo "radixter is cloned repository by https://github.com/ytakano/radix_tree.git"
	git submodule init
	git submodule update

bsd:
	make -C bsd_radix

test_radixter: test_radixter.cpp
	c++ -O3 -pipe -o test_radixter test_radixter.cpp

test_bsd: test_bsd.cpp
	c++ -O3 -pipe -o test_bsd test_bsd.cpp bsd_radix/radix.o

clean:
	rm -rf a.out test_bsd test_radixter
.PHONY : clean
