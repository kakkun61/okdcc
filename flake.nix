{
  description = "Description for the project";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = inputs@{ nixpkgs, flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [];
      systems = nixpkgs.lib.systems.flakeExposed;
      perSystem = { config, self', inputs', pkgs, system, ... }: 
        let
          web-packages = with pkgs; [
            nodejs_24
          ];
          board-packages = with pkgs; [
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
        in{
        devShells.default = pkgs.mkShell {
          packages = web-packages ++ board-packages;
        };
        apps = {
          unit = {
            type = "app";
            program = "${self'.packages.${system}.default}/bin/unit";
          };
        };
        formatter = pkgs.nixfmt;
      };
      flake = {};
    };
}
