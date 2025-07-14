{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }: flake-utils.lib.eachDefaultSystem(system: 
    let
      pkgs = import nixpkgs { inherit system; };
    in {
      devShell = pkgs.mkShell {
        buildInputs = with pkgs; [
	  openscad
	  python3Full
	  freecad
	  kicad
	  arduino-cli
	  cppcheck
	  cmake
	  gcc
	  avrdude
	];
      };
    });
}
