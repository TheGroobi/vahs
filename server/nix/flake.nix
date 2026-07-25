{
  description = "V.A.H.S. - Voluntarily Ass Home Server";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    disko = {
      url = "github:nix-community/disko";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    sops-nix.url = "github:Mic92/sops-nix";
  };

  outputs =
    {
      self,
      nixpkgs,
      disko,
      sops-nix,
      ...
    }:
    {
      nixosConfigurations = {

        t630 = nixpkgs.lib.nixosSystem {
          system = "x86_64-linux";
          modules = [
            disko.nixosModules.disko
            ./disk-config.nix
            ./configuration.nix
            sops-nix.nixosModules.sops
          ];
        };

        iso = nixpkgs.lib.nixosSystem {
          system = "x86_64-linux";
          specialArgs = { inherit nixpkgs; };
          modules = [
            ./installer.nix
          ];
        };
      };
    };
}
