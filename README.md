# ScriptCombiner

**ScriptCombiner** is a high-performance, native desktop application designed to prepare your coding projects for AI Large Language Models (LLMs) like Claude, ChatGPT, and Gemini.

It scans your project directory, filters files based on your specifications, and generates a single text file containing your project's directory structure and source code.

## ✨ Features

* **⚡ Native Performance:** Built with C++17 and wxWidgets for a minimal footprint.
* **🌲 Context-Aware Tree:** Generates a visual ASCII directory tree at the top of the output for AI context.
* **⚙️ Smart Presets:** One-click configuration for **Godot**, **Unity**, and **Node.js** projects.
* **👀 Syntax Highlighting:** integrated code editor to preview files before combining.
* **🛡️ Smart Filtering:**
    * Toggle specific file extensions (e.g., `.cpp`, `.py`, `.gd`, `.cs`, `.ts`).
    * Respects `.gitignore` patterns and allows custom ignore rules.
    * **Right-click** context menu to ignore files or extensions instantly.
* **💻 Cross-Platform:** Compile natively for Windows (x64/ARM), macOS (Intel/Apple Silicon), and Linux.

## 🚀 How to Use

1.  **Load Project:** Click **"Load Project Folder"** or use `Ctrl+O`.
2.  **Select Preset:** Choose a preset (Godot, Unity, Node) in the settings pane to automatically configure filters and ignore patterns.
3.  **Refine:**
    * Use the checkboxes to toggle file types.
    * Add custom ignore patterns (one per line) in the settings text area.
    * Right-click items in the tree to ignore them.
4.  **Preview:** Click any file in the tree to inspect its content.
5.  **Export:** Click **"Save Combined File"** (`Ctrl+S`) to generate the context file.

## 🛠️ Build Instructions

### Prerequisites
* **CMake** (3.10+)
* **C++ Compiler** (GCC, Clang, or MSVC)
* **wxWidgets** (3.2+)

### 🍎 macOS (Universal Binary - M1/M2 & Intel)

This build command creates a single app that runs natively on both Apple Silicon and Intel Macs.

```bash
# 1. Install dependencies
brew install cmake wxwidgets

# 2. Build
mkdir build && cd build
cmake ..
make

# 3. Run
open ScriptCombiner.app

### 🖥 Windows (x64 & ARM64)

**x64 (Standard):**

```powershell
mkdir build; cd build
cmake .. -A x64
cmake --build . --config Release
```

**ARM64 (Surface Pro X / Snapdragon):**

```powershell
mkdir build; cd build
cmake .. -A ARM64
cmake --build . --config Release
```

### 🐧 Linux (x64 & ARM)

This works for standard desktops (x64) and devices like Raspberry Pi (ARM).

```bash
# 1. Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake libwxgtk3.0-gtk3-dev

# 2. Build
mkdir build && cd build
cmake ..
make

# 3. Run
./ScriptCombiner
```

## 🔌 Extending the Project

To add support for a new language (e.g., **Rust**):

1.  **Add UI:** In `src/MainFrame.cpp` (`CreateLeftPanel`), add a new `wxCheckBox` for `.rs` files.
2.  **Add Logic:** In `src/MainFrame.cpp` (`IsFileTypeFiltered`), add:
    ```cpp
    if (m_filterRust->IsChecked() && ext == "rs") return true;
    ```
3.  **Add Syntax Highlighting:** In `src/MainFrame.cpp` (`SetEditorLexer`), map the extension:
    ```cpp
    } else if (ext == "rs") {
        m_stc->SetLexer(wxSTC_LEX_CPP); // Rust tokens are similar to C++
    }
    ```

## 📄 License

MIT License