{ config, pkgs, ... }:

{
  # --- Boot ---
  # UEFI bootloader. The VM build overrides this with a direct kernel boot so it's harmless when testing in qemu
  boot.loader.systemd-boot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;

  networking.hostName = "vahs";
  time.timeZone = "Europe/Warsaw";

  nix.settings.experimental-features = [
    "nix-command"
    "flakes"
  ];

  users.users.groobi = {
    isNormalUser = true;
    extraGroups = [ "wheel" ]; # 'wheel' = sudo

    # login is key-only
    openssh.authorizedKeys = {
      keys = import ./keys.nix;
    };
  };

  services.openssh = {
    enable = true;
    settings.PasswordAuthentication = false;
  };

  networking.firewall.allowedTCPPorts = [ 22 ];

  virtualisation.vmVariant.users.users.groobi.initialPassword = "test";
  # pins backwards-compatible defaults for stateful things.
  system.stateVersion = "26.05";
}
