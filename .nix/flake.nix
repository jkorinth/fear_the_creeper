{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;
        };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            arduino-cli
            audacity
            avrdude
            cmake
            cppcheck
            ffmpeg-full
            freecad
            gcc
            jre
            kicad
            meshlab
            minicom
            openscad
            platformio
            python3Full
            python3Packages.numpy
            python3Packages.pydub
            saleae-logic-2
            super-slicer
            vscode
            zip
          ];
        };
      }
    );
}
