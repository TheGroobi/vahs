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
    # No password set — login is SSH-key-only (the secure default for the real box).
    openssh.authorizedKeys.keys = [
      "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAILnF8ELhQU2XJE2CvrY+YVpEXvJxglyD6Mb80v7jJAdU groobi@vahs"
    ];
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
