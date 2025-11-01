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
        pio_install_and_patch = pkgs.writeShellScriptBin "pio-install-and-patch" ''
                    	#!/usr/bin/env bash
          		TOOLCHAIN="''${1:-atmelavr}"
                    	PIO_HOME="''${PLATFORMIO_HOME_DIR:-''$HOME/.platformio}"
          		TOOLCHAIN_PATH="''$PIO_HOME/packages/toolchain-$TOOLCHAIN"

          		set -e

          		${pkgs.platformio}/bin/pio pkg install -p ''$TOOLCHAIN
          		if [ -d "$TOOLCHAIN_PATH" ]; then
          		  echo "patching ''$TOOLCHAIN toolchain ..."
          		  find "$TOOLCHAIN_PATH" -type f -executable -exec file {} \; | \
          		    grep ELF | cut -d: -f1 | \
          		    xargs -r ${pkgs.patchelf}/bin/patchelf --set-rpath ${
                  pkgs.lib.makeLibraryPath [
                    pkgs.glibc
                    pkgs.gcc-unwrapped.lib
                    pkgs.zlib
                  ]
                } 2>/dev/null || true
          		  echo "toolchain $TOOLCHAIN was patched."
          		else
          		  echo "toolchain path: $TOOLCHAIN_PATH does not exist!"
          		fi
                    	'';
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            arduino-cli
            audacity
            avrdude
            clang
            clang-analyzer
            cmake
            cppcheck
            ffmpeg-full
            freecad
            gcc
            gtkwave
            jre
            kicad
            libelf
            libusb1
            meshlab
            minicom
            openscad
            pio_install_and_patch
            pkg-config
            platformio
            pulseview
            python3Full
            python3Packages.numpy
            python3Packages.pip
            python3Packages.pydub
            saleae-logic-2
            sigrok-cli
            simavr
            super-slicer
            vscode
            zip
          ];
          shellHook = ''
            	export PLATFORMIO_HOME_DIR="$PWD/sw/.platformio"
            	export PLATFORMIO_BUILD_DIR="$PWD/sw/.pio/build"
            	'';
        };

      }
    );
}
