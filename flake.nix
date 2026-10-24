{
  description = "An Implementation of Digital Command Control for Model Railroads";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    esp-dev.url = "github:mirrexagon/nixpkgs-esp-dev";
    m5unified = {
      url = "github:m5stack/M5Unified/0.2.27";
      flake = false;
    };
    m5gfx = {
      url = "github:m5stack/M5GFX/0.2.34";
      flake = false;
    };
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
          esp-components = pkgs.linkFarm "okdcc-esp-components" [
            {
              name = "M5Unified";
              path = inputs.m5unified;
            }
            {
              name = "M5GFX";
              path = inputs.m5gfx;
            }
          ];
          web-packages = with pkgs; [
            nodejs_24
          ];
          board-packages = with pkgs; [
            clang
            doxygen
            inputs'.esp-dev.packages.esp-idf-xtensa
            gettext
            glibc
            nixpkgs-fmt
            python312
            python312Packages.breathe
            python312Packages.sphinx_rtd_theme
            sphinx
            stdenv
            valgrind
            xorg.libX11
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
            OKDCC_ESP_COMPONENTS = esp-components;
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
