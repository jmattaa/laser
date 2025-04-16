{
  description = "laser - another ls alternative but configurable with lua!";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        pname = "laser";
        version = "1.5.0";

        src = pkgs.fetchFromGitHub {
          owner = "jmattaa";
          repo = pname;
          rev = "v${version}";
          sha256 = "c1825cfbefcc2d4122259db5eb91284bd784da7cc2d8c3d40ea03855131bbf65";
        };

      in
      {
        packages.${pname} = pkgs.stdenv.mkDerivation {
          inherit pname version src;

          nativeBuildInputs = [ pkgs.cmake ];
          buildInputs = [ pkgs.lua pkgs.libgit2 ];

          cmakeFlags = [
            "-DLUA_SCRIPTS_INSTALL_PATH=${placeholder "out"}/lsr"
          ];

          installPhase = ''
            runHook preInstall
            install -Dm755 build/lsr $out/bin/lsr
            cp -r $src/lsr $out/lsr
            runHook postInstall
          '';

          postInstall = ''
            mkdir -p $out/share/bash-completion/completions
            $out/bin/lsr --completions > $out/share/bash-completion/completions/lsr
          '';
        };

        defaultPackage = self.packages.${system}.${pname};

        apps.default = flake-utils.lib.mkApp {
          drv = self.packages.${system}.${pname};
          name = "lsr";
        };

        devShells.default = pkgs.mkShell {
          nativeBuildInputs = [ pkgs.cmake ];
          buildInputs = [ pkgs.lua pkgs.libgit2 ];
        };
      });
}
