{
  description = "An Implementation of Digital Command Control for Model Railroads";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs =
    inputs@{ nixpkgs, flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [ ];
      systems = nixpkgs.lib.systems.flakeExposed;
      perSystem =
        {
          config,
          self',
          inputs',
          pkgs,
          system,
          ...
        }:
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
          ];
        in
        {
          devShells.default = pkgs.mkShell {
            packages =
              (with pkgs; [
                cspell
                nixfmt
                treefmt
                yamlfmt
              ])
              ++ web-packages
              ++ board-packages;
          };
          apps = {
            unit = {
              type = "app";
              program = "${self'.packages.${system}.default}/bin/unit";
            };
          };
          formatter = pkgs.treefmt;
        };
      flake = { };
    };
}
