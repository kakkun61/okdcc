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
        name = "okdcc";
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            arduino-cli
            clang
            glibc
            nixpkgs-fmt
            python3
            python310Packages.pyserial
            stdenv
          ];
        };
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "okdcc";
          version = "0.0.0";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            stdenv
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
