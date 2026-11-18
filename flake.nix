{
  description = "An Implementation of Digital Command Control for Model Railroads";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    cspell-nix = {
      url = "github:kakkun61/cspell-nix";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-parts.follows = "flake-parts";
        treefmt-nix.follows = "treefmt-nix";
      };
    };
    treefmt-nix.url = "github:numtide/treefmt-nix";
  };

  outputs =
    inputs@{
      nixpkgs,
      flake-parts,
      cspell-nix,
      treefmt-nix,
      ...
    }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      imports = [
        cspell-nix.flakeModule
        treefmt-nix.flakeModule
        ./board/flake-module.nix
        ./web/flake-module.nix
      ];
      systems = nixpkgs.lib.systems.flakeExposed;
      perSystem =
        {
          self',
          pkgs,
          ...
        }:
        {
          devShells.default = pkgs.mkShell {
            inputsFrom = with self'.devShells; [
              board
              web
            ];
          };
          cspell.configFile = ./.cspell.yaml;
          treefmt.programs = {
            biome.enable = false;
            clang-format.enable = true;
            cmake-format.enable = true;
            nixfmt.enable = true;
            yamlfmt.enable = true;
          };
        };
      flake = { };
    };
}
