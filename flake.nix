{
  description = "Mosquitto plugin for authentication and access control through an HTTP API";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

  outputs = { self, nixpkgs }:
    let
      forAllSystems = nixpkgs.lib.genAttrs [ "x86_64-linux" "aarch64-linux" ];
    in
    {
      packages = forAllSystems (system: rec {
        mosquitto-http-plugin = nixpkgs.legacyPackages.${system}.callPackage ./package.nix { };
        default = mosquitto-http-plugin;
      });
    };
}
