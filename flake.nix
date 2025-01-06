{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            clang
            glibc
            nixpkgs-fmt
            platformio-core
            stdenv
            valgrind
            xorg.libX11
          ];
        };
        apps = {
          unit = {
            type = "app";
            program = "${self.packages.${system}.default}/bin/unit";
          };
        };
      });
}
