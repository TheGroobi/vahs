{
  connnnnfig,
  pkgs,
  lib,
  ...
}:

{
  imports = [ "${pkgs.path}/nixos/modules/installer/cd-dvd/iso-image.nix" ];
  users.users.root.openssh.authorizedKeys.keys = import ./keys.nix;

  networking.hostName = "vahs-iso";
  networking.useDHCP = true;

  services.avahi = {
    enable = true;
    nssmdns = true;
    publish = {
      enable = true;
      addresses = true;
      workstation = true;
    };
  };

  nix.settings.experimental-features = [
    "nix-command"
    "flakes"
  ];
}
