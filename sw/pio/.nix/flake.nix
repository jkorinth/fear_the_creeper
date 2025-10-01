{
  description = "PlatformIO development environment with VSCode for ATtiny85 programming";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; config.allowUnfree = true; };
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Core development tools
            vscode
            python3
            python3Packages.pip
            platformio
            
            # AVR toolchain (useful for debugging/manual compilation if needed)
            pkgsCross.avr.buildPackages.gcc
            avrdude
            
            # USB/Serial tools
            picocom
            minicom
            screen
            
            # Additional useful tools
            git
            gnumake
            cmake
            patchelf  # For patching PlatformIO's toolchain
            
            # Libraries that PlatformIO might need
            libusb1
            pkg-config
	    clang-analyzer
	    clang
          ];

          # Environment variables
          shellHook = ''
            echo "🚀 PlatformIO + VSCode development environment activated!"
            echo "📋 Available tools:"
            echo "  - VSCode: code"
            echo "  - PlatformIO CLI: pio"
            echo "  - AVR tools: avr-gcc, avrdude"
            echo "  - Serial tools: picocom, minicom, screen"
            echo ""
            echo "💡 Getting started:"
            echo "  1. Run 'code .' to open VSCode in current directory"
            echo "  2. Install PlatformIO IDE extension in VSCode"
            echo "  3. Create new PlatformIO project or open existing one"
            echo ""
            echo "🔧 For ATtiny85 with Black Pill ISP:"
            echo "  - Platform: atmelavr"
            echo "  - Board: attiny85"
            echo "  - Upload protocol: custom (Black Pill as ISP)"
            echo ""
            
            # Add current directory to Python path for PlatformIO
            export PYTHONPATH="$PWD:$PYTHONPATH"
            
            # Ensure USB device access
            export PLATFORMIO_UDEV_RULES_ENABLE=1
            
            # Set PlatformIO home directory (optional, keeps it local)
            export PLATFORMIO_HOME_DIR="$PWD/.platformio"
            
            # Add any custom PlatformIO settings
            export PLATFORMIO_BUILD_DIR="$PWD/.pio/build"
            
            # PlatformIO setup and toolchain patching
            echo "🔧 Setting up PlatformIO toolchain patching..."
            
            # Function to patch PlatformIO's downloaded toolchain
            patch_pio_toolchain() {
              local pio_home="''${PLATFORMIO_HOME_DIR:-''$HOME/.platformio}"
              local toolchain_path="''$pio_home/packages/toolchain-atmelavr"
              
              if [ -d "''$toolchain_path" ]; then
                echo "📦 Patching PlatformIO AVR toolchain..."
                find "''$toolchain_path" -type f -executable -exec file {} \; | \
                  grep ELF | cut -d: -f1 | \
                  xargs -r ${pkgs.patchelf}/bin/patchelf --set-interpreter ${pkgs.glibc}/lib/ld-linux-x86-64.so.2 2>/dev/null || true
                  
                find "''$toolchain_path" -type f -executable -exec file {} \; | \
                  grep ELF | cut -d: -f1 | \
                  xargs -r ${pkgs.patchelf}/bin/patchelf --set-rpath ${pkgs.lib.makeLibraryPath [ pkgs.glibc pkgs.gcc-unwrapped.lib pkgs.zlib ]} 2>/dev/null || true
                  
                echo "✅ Toolchain patched successfully"
              else
                echo "⚠️  Toolchain not found. Run 'pio platform install atmelavr' first"
              fi
            }
            
            # Auto-patch after platform install
            alias pio-install-and-patch='pio platform install atmelavr && patch_pio_toolchain'
            
            echo "💡 Use 'pio-install-and-patch' to install and patch the AVR platform"
            
            # Make sure we can access USB devices
            if [ -w /dev/ttyACM0 ] || [ -w /dev/ttyUSB0 ] 2>/dev/null; then
              echo "✅ USB device access appears to be working"
            else
              echo "⚠️  You may need to add your user to dialout group:"
              echo "   sudo usermod -a -G dialout $USER"
              echo "   Then log out and back in"
            fi
            
            echo "✅ Using Nix-provided AVR toolchain instead of PlatformIO's"
            echo "Happy coding! 🎯"
	    patch_pio_toolchain
          '';

          # Fix for PlatformIO on NixOS
          NIX_LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
            pkgs.stdenv.cc.cc
            pkgs.libusb1
          ];
        };
      });
}
