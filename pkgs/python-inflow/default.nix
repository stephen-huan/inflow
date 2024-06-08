{ lib
, fetchPypi
, buildPythonPackage
, setuptools
}:

buildPythonPackage rec {
  pname = "python-inflow";
  version = "1.0.1";

  src = fetchPypi {
    pname = "python_inflow";
    inherit version;
    hash = "sha256-CZrrMHiGOwKQpXZOc5CuklqwxOuv7PCZzkPZH7zKNuU=";
  };

  format = "pyproject";

  nativeBuildInputs = [
    setuptools
  ];

  pythonImportsCheck = [
    "inflow"
  ];

  meta = with lib; {
    description = "Variance-optimal paragraph formatter";
    homepage = "https://cgdct.moe/blog/far/";
    license = licenses.unlicense;
    maintainers = with maintainers; [ stephen-huan ];
  };
}
