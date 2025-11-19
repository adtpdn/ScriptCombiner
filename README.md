# ScriptCombiner (Native)

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)
![Language](https://img.shields.io/badge/language-C%2B%2B17-00599C)

**ScriptCombiner** is a lightweight, native desktop application designed to prepare your codebases for AI prompting (ChatGPT, Claude, Gemini). 

It scans a project directory, filters files based on your preferences, and generates a single text file containing the project structure and the content of all selected source files.

## ✨ Features

* **🚀 Native Performance:** Built with C++ and wxWidgets for a small footprint and fast execution.
* **🌲 Context-Aware Tree:** Generates a visual ASCII directory tree at the top of the output.
* **⚙️ Presets:** One-click configuration for **Godot**, **Unity**, and **Node.js** projects.
* **👀 Smart Filtering:** * Toggle specific file extensions (.cpp, .py, .js, .cs, etc.).
    * Respects `.gitignore` patterns.
    * Right-click context menu to quickly ignore files or extensions.
* **📝 Syntax Highlighting:** Preview files with syntax coloring before combining.
* **🔒 Privacy:** Runs entirely offline on your machine.

## 📦 Compilation Guide

### Prerequisites
* **C++ Compiler** (GCC, Clang, or MSVC)
* **CMake** (3.10 or higher)
* **wxWidgets** (3.2 or higher)

### 🍎 macOS

1.  **Install Dependencies** (using Homebrew):
    ```bash
    brew install cmake wxwidgets
    ```

2.  **Build:**
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

3.  **Run:**
    ```bash
    ./ScriptCombiner
    ```

### 🖥 Windows

1.  **Install wxWidgets:**
    * *Option A (vcpkg - Recommended):*
        ```powershell
        vcpkg install wxwidgets:x64-windows
        ```
    * *Option B (Installer):* Download the binaries from [wxWidgets.org](https://www.wxwidgets.org/downloads/) and set your `wxWidgets_ROOT` environment variable.

2.  **Build (using PowerShell or Command Prompt):**
    ```powershell
    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake
    cmake --build . --config Release
    ```

3.  **Run:**
    Find the executable in `build/Release/ScriptCombiner.exe`.

---

## 📖 How to Use

1.  **Load Project:** Click **"Load Project Folder"** (or `Ctrl+O`) and select the root directory of your code.
2.  **Select Preset (Optional):** Open "Project Settings" on the left and click a preset (e.g., Godot) to automatically check relevant file types and ignore standard junk folders.
3.  **Refine Selection:**
    * Use the **"File Type Filters"** accordion to toggle specific extensions.
    * **Right-click** any file in the tree view to "Ignore this file" or "Ignore all *.ext".
    * Edit ignore patterns manually in the text area.
4.  **Review:** Click on files in the tree to preview their content in the right panel.
5.  **Export:** Click **"Save Combined File"**.
6.  **Prompt:** Upload the generated text file to your favorite AI model with the prompt: *"Here is my project structure and code. [Insert your question]..."*

---

## 🛠️ Extending the Features

Want to add support for Rust, Go, or Ruby?

1.  **Add UI Checkbox:**
    In `src/MainFrame.h`, add a `wxCheckBox* m_filterRust;` member.
    In `src/MainFrame.cpp` (`CreateLeftPanel`), initialize the checkbox and add it to the sizer.

2.  **Update Filter Logic:**
    In `src/MainFrame.cpp` (`IsFileTypeFiltered`), add:
    ```cpp
    if (m_filterRust->IsChecked() && ext == "rs") return true;
    ```

3.  **Add Syntax Highlighting:**
    In `src/MainFrame.cpp` (`SetEditorLexer`), add a case for the extension to set the correct `wxSTC_LEX_*` constant.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.