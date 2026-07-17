MODULES := $(patsubst %/CMakeLists.txt,%,$(wildcard */CMakeLists.txt))

ifdef M
  MODULE := $(M)
else ifneq (words $(MODULES),1)
  $(error Multiple modules found: $(MODULES). Use M=<name>)
else
  MODULE := $(MODULES)
endif

.PHONY: build flash monitor clean list

build:
	idf.py -C $(MODULE) build

flash: build
	idf.py -C $(MODULE) flash

monitor:
	idf.py -C $(MODULE) monitor

clean:
	idf.py -C $(MODULE) fullclean

list:
	@echo $(MODULES)
