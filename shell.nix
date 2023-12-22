{ pkgs ? import <nixpkgs> }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    arduino-cli
    clang
    gnumake
    nixpkgs-fmt
    python3
    python310Packages.pyserial
  ];
}
