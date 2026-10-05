# Setup - Smart Exam Invigilation System (Windows + MSVC + SFML)

This project is 100% C++. `app.py` is just a launcher: it compiles the
C++ with your Visual Studio compiler and runs the result. Do this setup
once.

## 1. Visual Studio C++ tools

You said you already have Visual Studio installed. Confirm the C++
workload is in it:

1. Open **Visual Studio Installer** (search it in the Start menu).
2. Click **Modify** on your VS install.
3. Make sure **"Desktop development with C++"** is checked. If not, check
   it and click Modify/Install.

You do **not** need to open Visual Studio itself to build this project -
`app.py` drives the compiler directly.

## 2. Install SFML

1. Go to https://www.sfml-dev.org/download/sfml/2.6.1/
2. Under "Visual C++ 17 (2022) - 64-bit", download the zip
   (adjust the VS version number if you have VS 2019 - use the vc16 build).
3. Extract it somewhere simple, e.g. `C:\SFML-2.6.1`.
4. Confirm it has `include`, `lib`, and `bin` folders somewhere inside
   (either directly in that folder, or one level down - `app.py` checks
   both).

## 3. Point the launcher at SFML

1. Run `python app.py` once from the project folder. It will fail on
   purpose and create a file called `sfml_config.txt`.
2. Open `sfml_config.txt` and add one line with your SFML folder path,
   for example:
   ```
   C:\SFML-2.6.1
   ```
3. Save the file.

## 4. Run it

```
python app.py
```

This compiles everything under `src/` (except `core_test.cpp`, a
separate console test file not part of the GUI) with `cl.exe`, copies
the SFML DLLs next to the compiled exe, and launches it. Every run after
the first is just `python app.py` again - it rebuilds only if you've
changed the C++ source (recompiles every time currently; that's fine at
this project size).

## If the build fails

Copy the exact error text `cl.exe` prints and send it back - most Windows
SFML build issues are one of:
- Wrong SFML build for your VS version (vc16 vs vc17) - re-download the
  matching one.
- `sfml_config.txt` pointing at the wrong folder.
- Visual Studio missing the C++ workload (step 1).

## Login

- Username: `Admin`
- Password: `Admin123`

(Stored in `data/admin.csv`, created automatically on first run - you can
change it there later without recompiling.)

## Notes on what's admin-configurable

Nothing is hardcoded except the dropdown option lists (departments,
blocks, years). Halls, subjects, exam time slots, and all records are
entered and saved by the admin through the app itself, exactly as
discussed - the app just operates on whatever's currently saved in the
`data/*.csv` files.
