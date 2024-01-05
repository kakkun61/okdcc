{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs = import nixpkgs { inherit system; };
        name = "okdcc";
        buildInputs = with pkgs; [
          stdenv
          xorg.libX11
        ];
      in
      {
        inherit name;
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            arduino-cli
            clang
            glibc
            nixpkgs-fmt
            python3
            python310Packages.pyserial
            valgrind
          ] ++ buildInputs;
        };
        packages.default = pkgs.stdenv.mkDerivation {
          pname = name;
          version = "0.0.0";
          src = ./.;
          nativeBuildInputs = buildInputs;
        };
        apps = {
          unit = {
            type = "app";
            program = "${self.packages.${system}.default}/bin/unit";
          };
        };
      });
}
