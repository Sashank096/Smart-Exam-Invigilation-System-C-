"""
app.py - launcher ONLY.

This script contains zero application logic. All of the Smart Exam
Invigilation System's behaviour (classes, file handling, GUI) lives in
the C++ source under src/ and include/. Running this script:

  1. Finds your Visual Studio (MSVC) compiler.
  2. Finds your SFML installation (path you set below, or in
     sfml_config.txt).
  3. Compiles every .cpp under src/ (except core_test.cpp, which is a
     separate console test harness, not part of the GUI app) with cl.exe.
  4. Copies the SFML DLLs next to the compiled exe so Windows can find them.
  5. Runs the exe.

Usage:
    python app.py

First run will fail with clear instructions if SFML isn't set up yet -
that's expected, follow SETUP.md once and then just run this each time.
"""

import os
import sys
import subprocess
import shutil
import glob

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
SRC_DIR = os.path.join(PROJECT_ROOT, "src")
INCLUDE_DIR = os.path.join(PROJECT_ROOT, "include")
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
DATA_DIR = os.path.join(PROJECT_ROOT, "data")
CONFIG_FILE = os.path.join(PROJECT_ROOT, "sfml_config.txt")
EXE_NAME = "SmartExamInvigilation.exe"

# Files that make up the actual GUI app (core_test.cpp is excluded - it's a
# separate console test harness used during development, not the GUI).
EXCLUDE_FROM_BUILD = {"core_test.cpp"}


def fail(msg):
    print("\n[app.py] ERROR: " + msg + "\n")
    sys.exit(1)


def read_sfml_path():
    if not os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, "w") as f:
            f.write("# Put the full path to your SFML folder below, e.g.\n"
                    "# C:\\SFML-2.6.1\n")
        fail(
            "sfml_config.txt was just created (first run).\n"
            "  1. Install SFML for Visual Studio (see SETUP.md).\n"
            "  2. Open sfml_config.txt and put the SFML folder path on the "
            "first non-comment line.\n"
            "  3. Run 'python app.py' again."
        )
    path = None
    with open(CONFIG_FILE) as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#"):
                path = line
                break
    if not path or not os.path.isdir(path):
        fail(f"SFML path in sfml_config.txt does not exist: {path!r}. "
             f"Fix the path and run again.")
    return path


def find_vcvarsall():
    """Locate vcvarsall.bat via vswhere, which ships with every VS install."""
    vswhere = os.path.join(
        os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
        "Microsoft Visual Studio", "Installer", "vswhere.exe")
    if not os.path.exists(vswhere):
        fail("Could not find vswhere.exe - is Visual Studio installed with "
             "the 'Desktop development with C++' workload?")
    try:
        install_path = subprocess.check_output(
            [vswhere, "-latest", "-products", "*",
             "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
             "-property", "installationPath"],
            text=True).strip()
    except subprocess.CalledProcessError:
        install_path = ""
    if not install_path:
        fail("vswhere found no Visual Studio install with the C++ "
             "(VC.Tools.x86.x64) component. Install 'Desktop development "
             "with C++' from the Visual Studio Installer.")
    vcvarsall = os.path.join(install_path, "VC", "Auxiliary", "Build", "vcvarsall.bat")
    if not os.path.exists(vcvarsall):
        fail(f"vcvarsall.bat not found at expected path: {vcvarsall}")
    return vcvarsall


def find_sfml_layout(sfml_root):
    """
    SFML's official MSVC download sometimes puts include/lib/bin directly
    under the root, and sometimes one level down. Handle both.
    """
    direct = (os.path.join(sfml_root, "include"),
              os.path.join(sfml_root, "lib"),
              os.path.join(sfml_root, "bin"))
    if all(os.path.isdir(p) for p in direct):
        return direct

    for entry in os.listdir(sfml_root):
        sub = os.path.join(sfml_root, entry)
        if os.path.isdir(sub):
            nested = (os.path.join(sub, "include"),
                      os.path.join(sub, "lib"),
                      os.path.join(sub, "bin"))
            if all(os.path.isdir(p) for p in nested):
                return nested

    fail(f"Could not find include/, lib/ and bin/ folders under {sfml_root}. "
         f"Check that this is the SFML folder from the official download "
         f"(see SETUP.md).")


def build():
    sfml_root = read_sfml_path()
    sfml_include, sfml_lib, sfml_bin = find_sfml_layout(sfml_root)
    vcvarsall = find_vcvarsall()

    os.makedirs(BUILD_DIR, exist_ok=True)
    os.makedirs(DATA_DIR, exist_ok=True)

    sources = [f for f in glob.glob(os.path.join(SRC_DIR, "*.cpp"))
               if os.path.basename(f) not in EXCLUDE_FROM_BUILD]
    if not sources:
        fail("No .cpp files found to build under src/.")

    exe_path = os.path.join(BUILD_DIR, EXE_NAME)

    # SFML 2.x dynamic (non-static) link libraries, release build.
    # main is required first so its main() satisfies the linker before the
    # module libs that provide symbols it calls.
    libs = ["sfml-graphics.lib", "sfml-window.lib", "sfml-system.lib"]

    cl_command = (
        f'cl.exe /nologo /EHsc /std:c++17 /O2 '
        f'/I "{INCLUDE_DIR}" /I "{sfml_include}" '
        + " ".join(f'"{s}"' for s in sources) +
        f' /Fe:"{exe_path}" /Fo:"{BUILD_DIR}\\\\" '
        f'/link /LIBPATH:"{sfml_lib}" ' + " ".join(libs)
    )

    full_command = f'call "{vcvarsall}" x64 && {cl_command}'

    print("[app.py] Compiling with MSVC...")
    result = subprocess.run(full_command, shell=True, cwd=PROJECT_ROOT)
    if result.returncode != 0:
        fail("Compilation failed - see the errors above. Send them back and "
             "they'll get fixed.")

    # Copy SFML runtime DLLs next to the exe so Windows can find them.
    for dll in glob.glob(os.path.join(sfml_bin, "*.dll")):
        shutil.copy2(dll, BUILD_DIR)

    print(f"[app.py] Build succeeded: {exe_path}")
    return exe_path


def run(exe_path):
    # Run with cwd = project root (not build/) so the exe's relative paths
    # like "data/students.csv" resolve correctly. Windows still finds the
    # SFML DLLs because it always searches the exe's own folder regardless
    # of the working directory.
    print("[app.py] Launching Smart Exam Invigilation System...\n")
    subprocess.run([exe_path], cwd=PROJECT_ROOT)


if __name__ == "__main__":
    exe = build()
    run(exe)
