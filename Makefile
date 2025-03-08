GTEST_DIR ?= googletest/googletest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h $(GTEST_DIR)/include/gtest/internal/*.h

CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c11 -lm
LDFLAGS = -lm

$(shell mkdir -p build/gtest)

all: build/app.exe build/unit-tests.exe

clean:
	rm -rf build
	rm -rf venv
	rm -rf tests/integration/__pycache__
	rm -rf .pytest_cache

run-int: build/app.exe
	build/app.exe

run-float: build/app.exe
	build/app.exe --float

build/gtest/gtest-all.o: $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

build/gtest/gtest_main.o: $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

build/gtest/gtest_main.a: build/gtest/gtest-all.o build/gtest/gtest_main.o
	ar rv $@ $^ -o $@

run-unit-test: build/unit-tests.exe
	build/unit-tests.exe

build/unit-tests.exe: build/gtest/gtest_main.a
	g++ -isystem $(GTEST_DIR)/include -pthread \
		tests/unit/tests.cpp \
		build/gtest/gtest_main.a \
		-o build/unit-tests.exe

venv/bin/activate:
	python3 -m venv venv

venv: venv/bin/activate
	venv/bin/pip install pytest

run-integration-tests: build/app.exe venv
	venv/bin/pytest tests/integration/test_integration.py

build/app.exe:
	gcc $(CFLAGS) src/calculator.c -o build/app.exe $(LDFLAGS)

