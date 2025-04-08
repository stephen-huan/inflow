# inflow

A variance-optimal paragraph formatter. It has only three rules:

1. Minimize the variance of the lengths of each line...
2. ...subject to the constraint that the number of lines is smallest.
3. Ignore the last line, while making sure it's shorter than average.

The program `inflow` reads its input from stdin and writes to stdout.
It takes a single positional argument, the maximum number of characters
per line, e.g. `inflow 72`. The default width if unspecified is 79.

A previous version of this program was called `far`.

See [far: a (f)ast re-write of the Unix utility
p(ar)](https://cgdct.moe/blog/far/) for more information.

## Installation

Inflow is packaged in
[Nixpkgs](https://github.com/NixOS/nixpkgs) as `pkgs.inflow` and
[PyPi](https://pypi.org/project/python-inflow/) as `python-inflow`.

## Editor integration

- (neo)vim: set `formatprg` to `inflow` or `inflow 72`
- emacs: [inflow.el](https://github.com/eshrh/inflow.el)
