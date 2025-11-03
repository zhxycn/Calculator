THIRD-PARTY NOTICES
===================

This repository publishes only the project's source code. The project source is licensed under the MIT License (see `LICENSE`). The MIT license applies only to files authored as part of this repository and does not relicense third‑party libraries used by the project.

Important: This repository does NOT provide prebuilt binaries or runtime DLLs as part of the official source distribution or releases. If you have local build artifacts (for example a `build/` folder created by your IDE or CI that contains DLLs or executables), those artifacts are not part of the project's source license and should not be relied on as distributed third‑party binaries in official releases.

Third‑party components used by this project
-----------------------------------------

1) Qt 6
- Typical license: GNU Lesser General Public License (LGPL) for many Qt editions. Certain Qt modules or distributions may use different licenses or include exceptions. Verify the exact license version and any exceptions for the Qt distribution you use.
- Official info: https://doc.qt.io/qt-6/licensing.html
- Note for users building from source: You must obtain Qt yourself (for example, from the official Qt installer or your system package manager). This repository does not bundle Qt binaries.

2) GNU MP (GMP)
- Typical license: GNU Lesser General Public License (LGPL). Verify the exact license version for the GMP package you use.
- Official info: https://gmplib.org/
- Note for users building from source: You must install GMP (gmp/gmpxx) on your system or via your package manager yourself. This repository does not bundle GMP binaries.

What you must do if you distribute binaries that include third‑party libraries
-----------------------------------------------------------------------------
- If you create and distribute a binary release that bundles third‑party LGPL libraries (for example, shipping Qt DLLs or GMP DLLs inside a ZIP/installer), you must comply with the applicable LGPL obligations for those libraries.
  - Include a prominent notice that your application includes third‑party libraries, list each third‑party library and its license, and provide any required license texts with the distribution.
  - Include copies of the third‑party license texts in the distribution (for example, place them in a `licenses/` directory in the release artifact).
  - If you dynamically link to an LGPL library (i.e., ship DLL/.so files), ensure recipients can replace those libraries with modified versions (placing a different DLL in the same directory is usually sufficient). Document how to replace or relink the libraries.
  - If you statically link an LGPL library into your executable, the LGPL requires that you provide the object files or another mechanism that allows recipients to relink the executable with a modified version of the LGPL library. Typically this means providing build scripts and object files, or distributing the corresponding library source, as required by the specific LGPL version.
- If you modify an LGPL library itself, you must make those modifications available under the terms of the LGPL.
