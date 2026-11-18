{ inputs, ... }:
let
  inherit (inputs) flake-parts;
in
{
  _class = "flake";

  options.perSystem = flake-parts.lib.mkPerSystemOption (
    { pkgs, ... }:
    {
      devShells.board = pkgs.mkShell {
        packages =
          with pkgs;
          let
            build = [
              platformio-core
              stdenv
            ];
            doc = [
              gettext
              doxygen
              python312Packages.breathe
              python312Packages.sphinx_rtd_theme
              sphinx
            ];
          in
          build ++ doc;
      };
    }
  );
}
