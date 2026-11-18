{ inputs, ... }:
let
  inherit (inputs) flake-parts;
in
{
  _class = "flake";

  options.perSystem = flake-parts.lib.mkPerSystemOption (
    { pkgs, ... }:
    {
      devShells.web = pkgs.mkShell {
        packages = with pkgs; [
          nodejs_24
        ];
      };
    }
  );
}
