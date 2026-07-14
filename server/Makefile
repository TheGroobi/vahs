VM := ./nix\#nixosConfigurations.t630.config.system.build.vm

build:
	git add ./nix
	nix build '$(VM)'

run: build
	./result/bin/run-*-vm

fresh: build
	rm -f *.qcow2
	./result/bin/run-*-vm

clean:
	rm -f result *.qcow2

.PHONY: build run fresh clean
