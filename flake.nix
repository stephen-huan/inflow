{
  description = "inflow";

  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      inherit (nixpkgs) lib;
      systems = lib.systems.flakeExposed;
      eachDefaultSystem = f: builtins.foldl' lib.attrsets.recursiveUpdate { }
        (map f systems);
    in
    eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        python' = pkgs.python3.withPackages (ps: with ps; [
          build
          keyring-pass
        ]);
        formatters = with pkgs; [ black isort clang-tools nixpkgs-fmt ];
        linters = with pkgs; [ pyright clang-tools ruff statix ];
      in
      {
        packages.${system} = {
          default = pkgs.inflow.overrideAttrs {
            src = ./.;
            env.NIX_CFLAGS_COMPILE = "-Werror";
          };
          python-inflow = (
            pkgs.python3Packages.callPackage ./pkgs/python-inflow { }
          ).overrideAttrs {
            src = ./.;
          };
        };

        formatter.${system} = pkgs.writeShellApplication {
          name = "formatter";
          runtimeInputs = formatters;
          text = ''
            isort "$@"
            black "$@"
            nixpkgs-fmt "$@"
            clang-format -i inflow.cpp
          '';
        };

        checks.${system}.lint = pkgs.stdenvNoCC.mkDerivation {
          name = "lint";
          src = ./.;
          doCheck = true;
          nativeCheckInputs = formatters ++ linters;
          checkPhase = ''
            isort --check --diff .
            black --check --diff .
            nixpkgs-fmt --check .
            ruff check .
            pyright .
            clang-tidy inflow.cpp --
            statix check
          '';
          installPhase = "touch $out";
        };

        devShells.${system}.default = (pkgs.mkShellNoCC.override {
          stdenv = pkgs.stdenvNoCC.override {
            initialPath = [ pkgs.coreutils ];
          };
        }) {
          packages = [
            pkgs.twine
            python'
          ]
          ++ formatters
          ++ linters;
        };
      }
    );
}
