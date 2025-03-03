# spell-checker:words numtide pkgs stdenv
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
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
            doxygen
            gettext
            glibc
            nixpkgs-fmt
            platformio-core
            python312
            python312Packages.breathe
            python312Packages.sphinx_rtd_theme
            sphinx
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
