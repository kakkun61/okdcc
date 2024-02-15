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
          buildInputs = with pkgs; [
            arduino-cli
            clang
            glibc
            nixpkgs-fmt
            python3
            python310Packages.pyserial
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
