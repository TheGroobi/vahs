PREFIX := ./nix\#nixosConfigurations

VM := $(PREFIX).t630.config.system.build.vm
iso := $(PREFIX).iso.config.system.build.isoImage

.PHONY: build build-iso
build: ATTR = $(VM)
build-iso: ATTR = $(iso)
build build-iso:
	git add ./nix
	nix build '$(ATTR)'

run: build
	./result/bin/run-*-vm

fresh: build
	rm -f *.qcow2
	./result/bin/run-*-vm

clean:
	rm -f result *.qcow2

.PHONY: build run fresh clean
