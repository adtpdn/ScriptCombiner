#include "MainFrame.h"
#include <wx/artprov.h> // For wxArtProvider
#include <wx/sstream.h> // For wxStringOutputStream
#include <wx/textfile.h> // For reading text files
#include <wx/txtstrm.h> 
#include <wx/datetime.h> 

// --- Ignore Pattern Presets ---

const char* DEFAULT_IGNORE_PATTERNS =
    ".git\n"
    ".vscode\n"
    "node_modules\n"
    "*.log\n"
    ".DS_Store\n";

const char* GODOT_IGNORE_PATTERNS =
    ".godot/\n"
    "*.import\n"
    ".mono/\n"
    "export_presets.cfg\n"
    "*.binary\n"
    "*.rcedit\n"
    "*.TMP\n"
    "*.png.import\n"
    "*.svg.import\n"
    "*.gltf.import\n"
    "*.glb.import\n"
    "*.obj.import\n"
    "*.wav.import\n"
    "*.mp3.import\n";

const char* UNITY_IGNORE_PATTERNS =
    "[Ll]ibrary/\n"
    "[Tt]emp/\n"
    "[Oo]bj/\n"
    "[Bb]uild/\n"
    "[Bb]uilds/\n"
    "[Ll]ogs/\n"
    "[Mm]emoryCaptures/\n"
    "*.pidb\n"
    "*.suo\n"
    "*.user\n"
    "*.userprefs\n"
    "*.sln\n"
    "*.csproj\n"
    "*.unityproj\n"
    "*.apk\n"
    "*.aab\n"
    "*.unitypackage\n"
    "*.app\n"
    "*.exe\n"
    "*.dll\n"
    "*.mdb\n"
    "*.pdb\n"
    ".vs/\n"
    ".vscode/\n";

const char* NODE_IGNORE_PATTERNS =
    "node_modules/\n"
    "npm-debug.log\n"
    "yarn-debug.log\n"
    "yarn-error.log\n"
    "package-lock.json\n"
    "yarn.lock\n"
    "pnpm-lock.yaml\n"
    ".env\n"
    "dist/\n"
    "build/\n"
    "*.log\n"
    ".DS_Store\n"
    ".cache/\n";


MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), m_imageList(nullptr), m_projectType(ProjectType::None)
{
    // ... (SetupMenuBar, CreateStatusBar are unchanged)
    SetupMenuBar();
    m_statusBar = CreateStatusBar(2);
    SetStatusText("Please load a project folder.", 0);
    
    m_imageList = new wxImageList(16, 16, true); 
    m_imageList->Add(wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
    m_imageList->Add(wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
    
    CreateLayout();
    SetupEditorStyles();
    Centre();

    // --- ADDED: Bind Context Menu Event ---
    Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::OnTreeRightClick, this, m_treeCtrl->GetId());
}

void MainFrame::SetupMenuBar()
{

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_LoadProject, "&Open Project Folder...\tCtrl-O", "Load a project directory");
    menuFile->Append(ID_SaveCombined, "&Save Combined File...\tCtrl-S", "Save all filtered files into one");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_Copy, "&Copy\tCtrl-C", "Copy selected text");
    menuEdit->Append(ID_SelectAll, "Select &All\tCtrl-A", "Select all text in the editor");
    
    wxMenu* menuView = new wxMenu;
    menuView->AppendCheckItem(ID_ToggleWordWrap, "&Toggle Word Wrap\tCtrl-W", "Toggle word wrap in the editor");
    menuView->Check(ID_ToggleWordWrap, true); 

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    m_menuBar = new wxMenuBar; 
    m_menuBar->Append(menuFile, "&File");
    m_menuBar->Append(menuEdit, "&Edit");
    m_menuBar->Append(menuView, "&View"); 
    m_menuBar->Append(menuHelp, "&Help");

    SetMenuBar(m_menuBar);

    // --- Bind Menu Events ---
    Bind(wxEVT_MENU, &MainFrame::OnLoadProject, this, ID_LoadProject);
    Bind(wxEVT_MENU, &MainFrame::OnSaveCombined, this, ID_SaveCombined);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnCopy, this, ID_Copy);
    Bind(wxEVT_MENU, &MainFrame::OnSelectAll, this, ID_SelectAll);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnToggleWordWrap, this, ID_ToggleWordWrap);
}

void MainFrame::CreateLayout()
{

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_leftPanel = CreateLeftPanel(this);
    m_rightPanel = CreateRightPanel(this);
    mainSizer->Add(m_leftPanel, 1, wxEXPAND | wxALL, 5); 
    mainSizer->Add(m_rightPanel, 3, wxEXPAND | wxALL, 5); 
    SetSizer(mainSizer);
}

wxPanel* MainFrame::CreateLeftPanel(wxWindow* parent)
{
    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // --- Load Project Button ---

    wxButton* loadBtn = new wxButton(panel, ID_LoadProject, "Load Project Folder");
    sizer->Add(loadBtn, 0, wxEXPAND | wxBOTTOM, 10); 
    Bind(wxEVT_BUTTON, &MainFrame::OnLoadProject, this, ID_LoadProject);

    // --- Settings Accordion ---

    m_settingsPane = new wxCollapsiblePane(panel, wxID_ANY, "Project Settings");
    sizer->Add(m_settingsPane, 0, wxEXPAND | wxBOTTOM, 5);
    wxWindow* settingsWin = m_settingsPane->GetPane();
    wxBoxSizer* settingsSizer = new wxBoxSizer(wxVERTICAL);
    
    m_showHiddenCheck = new wxCheckBox(settingsWin, ID_SettingsChanged, "Show Hidden Files/Folders");
    settingsSizer->Add(m_showHiddenCheck, 0, wxALL, 5);
    Bind(wxEVT_CHECKBOX, &MainFrame::OnSettingsChanged, this, ID_SettingsChanged);

    // --- Preset Buttons ---

    settingsSizer->Add(new wxStaticText(settingsWin, wxID_ANY, "Ignore Presets:"), 0, wxLEFT | wxRIGHT | wxTOP, 5);
    wxBoxSizer* presetSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* godotBtn = new wxButton(settingsWin, ID_PresetGodot, "Godot");
    wxButton* unityBtn = new wxButton(settingsWin, ID_PresetUnity, "Unity");
    wxButton* nodeBtn = new wxButton(settingsWin, ID_PresetNode, "Node.js");
    presetSizer->Add(godotBtn, 1, wxEXPAND | wxRIGHT, 2);
    presetSizer->Add(unityBtn, 1, wxEXPAND | wxLEFT | wxRIGHT, 2);
    presetSizer->Add(nodeBtn, 1, wxEXPAND | wxLEFT, 2);
    settingsSizer->Add(presetSizer, 0, wxEXPAND | wxALL, 5);
    Bind(wxEVT_BUTTON, &MainFrame::OnPresetGodot, this, ID_PresetGodot);
    Bind(wxEVT_BUTTON, &MainFrame::OnPresetUnity, this, ID_PresetUnity);
    Bind(wxEVT_BUTTON, &MainFrame::OnPresetNode, this, ID_PresetNode);

    // --- Ignore Patterns Text Area ---
    settingsSizer->Add(new wxStaticText(settingsWin, wxID_ANY, "Ignore Patterns (one per line):"), 0, wxLEFT | wxRIGHT | wxTOP, 5);
    m_ignorePatternsText = new wxTextCtrl(settingsWin, ID_SettingsChanged, DEFAULT_IGNORE_PATTERNS, wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE);
    settingsSizer->Add(m_ignorePatternsText, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    Bind(wxEVT_TEXT, &MainFrame::OnSettingsChanged, this, ID_SettingsChanged);

    // --- ADDED: Load/Save Ignore File Buttons ---
    wxBoxSizer* ignoreFileSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* loadIgnoreBtn = new wxButton(settingsWin, ID_LoadIgnoreFile, "Load...");
    wxButton* saveIgnoreBtn = new wxButton(settingsWin, ID_SaveIgnoreFile, "Save As...");
    ignoreFileSizer->Add(loadIgnoreBtn, 1, wxEXPAND | wxRIGHT, 2);
    ignoreFileSizer->Add(saveIgnoreBtn, 1, wxEXPAND | wxLEFT, 2);
    settingsSizer->Add(ignoreFileSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    Bind(wxEVT_BUTTON, &MainFrame::OnLoadIgnoreFile, this, ID_LoadIgnoreFile);
    Bind(wxEVT_BUTTON, &MainFrame::OnSaveIgnoreFile, this, ID_SaveIgnoreFile);
    // ---

    settingsWin->SetSizer(settingsSizer);

    // --- Filter Accordion ---

    m_filterPane = new wxCollapsiblePane(panel, wxID_ANY, "File Type Filters");
    sizer->Add(m_filterPane, 0, wxEXPAND | wxBOTTOM, 5);
    wxWindow* filterWin = m_filterPane->GetPane();
    wxFlexGridSizer* filterSizer = new wxFlexGridSizer(3, wxSize(5, 5)); 
    filterSizer->AddGrowableCol(0, 1);
    filterSizer->AddGrowableCol(1, 1);
    filterSizer->AddGrowableCol(2, 1);

    // --- Checkboxes (all unchanged) ---
    m_filterCpp = new wxCheckBox(filterWin, ID_SettingsChanged, ".cpp / .c"); m_filterCpp->SetValue(true);
    m_filterH = new wxCheckBox(filterWin, ID_SettingsChanged, ".h / .hpp"); m_filterH->SetValue(true);
    m_filterPy = new wxCheckBox(filterWin, ID_SettingsChanged, ".py"); m_filterPy->SetValue(true);
    m_filterJs = new wxCheckBox(filterWin, ID_SettingsChanged, ".js"); m_filterJs->SetValue(true);
    m_filterHtml = new wxCheckBox(filterWin, ID_SettingsChanged, ".html"); m_filterHtml->SetValue(true);
    m_filterCss = new wxCheckBox(filterWin, ID_SettingsChanged, ".css"); m_filterCss->SetValue(true);
    m_filterJson = new wxCheckBox(filterWin, ID_SettingsChanged, ".json"); m_filterJson->SetValue(true);
    m_filterMd = new wxCheckBox(filterWin, ID_SettingsChanged, ".md"); m_filterMd->SetValue(true);
    m_filterGd = new wxCheckBox(filterWin, ID_SettingsChanged, ".gd"); m_filterGd->SetValue(true);
    m_filterTscn = new wxCheckBox(filterWin, ID_SettingsChanged, ".tscn"); m_filterTscn->SetValue(false);
    m_filterTres = new wxCheckBox(filterWin, ID_SettingsChanged, ".tres"); m_filterTres->SetValue(false);
    m_filterRes = new wxCheckBox(filterWin, ID_SettingsChanged, ".res"); m_filterRes->SetValue(false);
    m_filterCs = new wxCheckBox(filterWin, ID_SettingsChanged, ".cs"); m_filterCs->SetValue(true);
    m_filterShader = new wxCheckBox(filterWin, ID_SettingsChanged, ".shader"); m_filterShader->SetValue(true);
    m_filterUnity = new wxCheckBox(filterWin, ID_SettingsChanged, ".unity"); m_filterUnity->SetValue(false);
    m_filterPrefab = new wxCheckBox(filterWin, ID_SettingsChanged, ".prefab"); m_filterPrefab->SetValue(false);
    m_filterTs = new wxCheckBox(filterWin, ID_SettingsChanged, ".ts"); m_filterTs->SetValue(true);
    m_filterMjs = new wxCheckBox(filterWin, ID_SettingsChanged, ".mjs"); m_filterMjs->SetValue(true);
    m_filterCjs = new wxCheckBox(filterWin, ID_SettingsChanged, ".cjs"); m_filterCjs->SetValue(true);
    filterSizer->Add(m_filterCpp, 1, wxEXPAND);
    filterSizer->Add(m_filterH, 1, wxEXPAND);
    filterSizer->Add(m_filterPy, 1, wxEXPAND);
    filterSizer->Add(m_filterJs, 1, wxEXPAND);
    filterSizer->Add(m_filterHtml, 1, wxEXPAND);
    filterSizer->Add(m_filterCss, 1, wxEXPAND);
    filterSizer->Add(m_filterJson, 1, wxEXPAND);
    filterSizer->Add(m_filterMd, 1, wxEXPAND);
    filterSizer->Add(m_filterGd, 1, wxEXPAND);
    filterSizer->Add(m_filterTscn, 1, wxEXPAND);
    filterSizer->Add(m_filterTres, 1, wxEXPAND);
    filterSizer->Add(m_filterRes, 1, wxEXPAND);
    filterSizer->Add(m_filterCs, 1, wxEXPAND);
    filterSizer->Add(m_filterShader, 1, wxEXPAND);
    filterSizer->Add(m_filterUnity, 1, wxEXPAND);
    filterSizer->Add(m_filterPrefab, 1, wxEXPAND);
    filterSizer->Add(m_filterTs, 1, wxEXPAND);
    filterSizer->Add(m_filterMjs, 1, wxEXPAND);
    filterSizer->Add(m_filterCjs, 1, wxEXPAND);
    Bind(wxEVT_CHECKBOX, &MainFrame::OnSettingsChanged, this, ID_SettingsChanged);
    filterWin->SetSizer(filterSizer);

    // --- File Tree ---

    m_treeCtrl = new wxTreeCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    m_treeCtrl->AssignImageList(m_imageList); 
    sizer->Add(m_treeCtrl, 1, wxEXPAND | wxALL, 5);
    Bind(wxEVT_TREE_SEL_CHANGED, &MainFrame::OnTreeSelectionChanged, this, m_treeCtrl->GetId()); 

    // --- Save Button ---

    wxButton* saveBtn = new wxButton(panel, ID_SaveCombined, "Save Combined File");
    sizer->Add(saveBtn, 0, wxEXPAND | wxTOP, 10); 
    Bind(wxEVT_BUTTON, &MainFrame::OnSaveCombined, this, ID_SaveCombined);

    panel->SetSizer(sizer);
    return panel;
}

wxPanel* MainFrame::CreateRightPanel(wxWindow* parent)
{

    wxPanel* panel = new wxPanel(parent, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* toolSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* copyBtn = new wxButton(panel, ID_Copy, "Copy");
    wxButton* selectAllBtn = new wxButton(panel, ID_SelectAll, "Select All");
    toolSizer->Add(copyBtn, 0, wxRIGHT, 5);
    toolSizer->Add(selectAllBtn, 0, wxRIGHT, 5);
    sizer->Add(toolSizer, 0, wxALL, 5);
    Bind(wxEVT_BUTTON, &MainFrame::OnCopy, this, ID_Copy);
    Bind(wxEVT_BUTTON, &MainFrame::OnSelectAll, this, ID_SelectAll);
    m_stc = new wxStyledTextCtrl(panel, wxID_ANY);
    sizer->Add(m_stc, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);
    return panel;
}

void MainFrame::SetupEditorStyles()
{

    m_stc->SetLexer(wxSTC_LEX_NULL);
    m_stc->StyleClearAll();
    m_stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_stc->SetMarginWidth(0, 50);
    m_stc->SetTabWidth(4);
    m_stc->SetIndent(4);
    m_stc->SetUseTabs(false);
    m_stc->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
    m_stc->SetWrapMode(wxSTC_WRAP_WORD);
    wxColour darkBg(30, 30, 30);
    wxColour lightText(220, 220, 220);
    wxColour darkText(128, 128, 128);
    wxColour keywordBlue(97, 175, 239);
    wxColour stringGreen(152, 195, 121);
    wxColour commentGray(117, 113, 94);
    wxColour numberPurple(198, 120, 221);
    m_stc->StyleSetBackground(wxSTC_STYLE_DEFAULT, darkBg);
    m_stc->StyleSetForeground(wxSTC_STYLE_DEFAULT, lightText);
    m_stc->StyleClearAll(); 
    m_stc->StyleSetBackground(wxSTC_STYLE_LINENUMBER, wxColour(20, 20, 20));
    m_stc->StyleSetForeground(wxSTC_STYLE_LINENUMBER, darkText);
    m_stc->SetCaretForeground(wxColour(255, 255, 255));
    m_stc->SetSelBackground(true, wxColour(50, 50, 90));
    m_stc->StyleSetForeground(wxSTC_C_COMMENT, commentGray);
    m_stc->StyleSetForeground(wxSTC_C_COMMENTLINE, commentGray);
    m_stc->StyleSetForeground(wxSTC_C_STRING, stringGreen);
    m_stc->StyleSetForeground(wxSTC_C_CHARACTER, stringGreen);
    m_stc->StyleSetForeground(wxSTC_C_WORD, keywordBlue);
    m_stc->StyleSetForeground(wxSTC_C_NUMBER, numberPurple);
    m_stc->StyleSetForeground(wxSTC_C_PREPROCESSOR, numberPurple);
    m_stc->StyleSetForeground(wxSTC_P_COMMENTLINE, commentGray);
    m_stc->StyleSetForeground(wxSTC_P_STRING, stringGreen);
    m_stc->StyleSetForeground(wxSTC_P_CHARACTER, stringGreen);
    m_stc->StyleSetForeground(wxSTC_P_WORD, keywordBlue); 
    m_stc->StyleSetForeground(wxSTC_P_NUMBER, numberPurple);
    m_stc->StyleSetForeground(wxSTC_P_DEFNAME, wxColour(229, 192, 123)); 
}

void MainFrame::SetEditorLexer(const wxString& filePath)
{

    wxFileName fn(filePath);
    wxString ext = fn.GetExt().Lower();

    if (ext == "cpp" || ext == "c" || ext == "h" || ext == "hpp") {
        m_stc->SetLexer(wxSTC_LEX_CPP);
    } else if (ext == "py" || ext == "gd") { 
        m_stc->SetLexer(wxSTC_LEX_PYTHON);
    } else if (ext == "js" || ext == "mjs" || ext == "cjs") { 
        m_stc->SetLexer(wxSTC_LEX_ESCRIPT);
    } else if (ext == "ts") {
        m_stc->SetLexer(wxSTC_LEX_ESCRIPT); 
    } else if (ext == "html" || ext == "wxml") {
        m_stc->SetLexer(wxSTC_LEX_HTML);
    } else if (ext == "css" || ext == "wxss") {
        m_stc->SetLexer(wxSTC_LEX_CSS);
    } else if (ext == "json") {
        m_stc->SetLexer(wxSTC_LEX_JSON);
    } else if (ext == "md") {
        m_stc->SetLexer(wxSTC_LEX_MARKDOWN);
    } else if (ext == "xml" || ext == "tscn" || ext == "tres" || ext == "res" || ext == "csproj" || ext == "unity" || ext == "prefab") { 
        m_stc->SetLexer(wxSTC_LEX_XML);
    } else if (ext == "cs") {
        m_stc->SetLexer(wxSTC_LEX_CPP); 
    } else if (ext == "shader") {
        m_stc->SetLexer(wxSTC_LEX_CPP); 
    } else {
        m_stc->SetLexer(wxSTC_LEX_NULL);
    }
}


// --- Main Logic ---

void MainFrame::OnLoadProject(wxCommandEvent& event)
{

    wxDirDialog dlg(this, "Choose a project directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    m_projectRoot = dlg.GetPath();
    m_projectType = ProjectType::None; 

    if (wxFileExists(m_projectRoot + "/project.godot")) {
        OnPresetGodot(event);
    } else if (wxDirExists(m_projectRoot + "/Assets") && wxDirExists(m_projectRoot + "/ProjectSettings")) {
        OnPresetUnity(event);
    } else if (wxFileExists(m_projectRoot + "/package.json")) {
        OnPresetNode(event);
    }

    PopulateFileTree();
}

void MainFrame::OnSettingsChanged(wxCommandEvent& event)
{

    if (!m_projectRoot.IsEmpty()) {
        PopulateFileTree();
    }
}

void MainFrame::PopulateFileTree()
{

    if (m_projectRoot.IsEmpty()) return;

    m_ignorePatterns.Clear();
    wxStringInputStream sstream(m_ignorePatternsText->GetValue());
    wxTextInputStream tstream(sstream); 
    while (!sstream.Eof()) { 
        wxString line = tstream.ReadLine();
        if (!line.IsEmpty()) {
            m_ignorePatterns.Add(line);
        }
    }

    m_treeCtrl->DeleteAllItems();
    wxFileName fn(m_projectRoot);
    wxTreeItemId rootId = m_treeCtrl->AddRoot(fn.GetFullName(), 0);
    m_treeCtrl->SetItemData(rootId, new FileTreeData(m_projectRoot));

    ScanDirectoryRecursive(m_projectRoot, rootId);
    m_treeCtrl->Expand(rootId);
    SetStatusText("Project loaded: " + m_projectRoot, 0);
}

void MainFrame::ScanDirectoryRecursive(const wxString& path, const wxTreeItemId& parentId)
{

    wxDir dir(path);
    if (!dir.IsOpened())
        return;

    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS | wxDIR_HIDDEN);
    while (cont)
    {
        wxString fullPath = path + wxFileName::GetPathSeparator() + filename;
        bool isDir = wxDir::Exists(fullPath);

        if (!ShouldIgnore(fullPath, isDir))
        {
            if (isDir)
            {
                wxTreeItemId newDirId = m_treeCtrl->AppendItem(parentId, filename, 0); 
                m_treeCtrl->SetItemData(newDirId, new FileTreeData(fullPath));
                ScanDirectoryRecursive(fullPath, newDirId);
            }
            else
            {
                if (IsFileTypeFiltered(fullPath))
                {
                    wxTreeItemId fileId = m_treeCtrl->AppendItem(parentId, filename, 1); 
                    m_treeCtrl->SetItemData(fileId, new FileTreeData(fullPath));
                }
            }
        }
        cont = dir.GetNext(&filename);
    }
}

bool MainFrame::ShouldIgnore(const wxString& path, bool isDir)
{

    wxFileName fn(path);
    wxString name = fn.GetFullName();

    if (!m_showHiddenCheck->IsChecked() && name.StartsWith("."))
    {
        return true;
    }

    for (const wxString& pattern : m_ignorePatterns)
    {
        wxString patternToCheck = pattern;
        if (pattern.EndsWith("/") || pattern.EndsWith(wxFileName::GetPathSeparator())) {
            if (!isDir) continue; 
            patternToCheck = pattern.Left(pattern.Length() - 1);
        }

        if (wxMatchWild(patternToCheck, name, false)) 
        {
            return true;
        }
        
        wxString relativePath = GetRelativePath(path);
        
        if (wxMatchWild(patternToCheck, relativePath, false))
        {
            return true;
        }
    }
    
    return false;
}

bool MainFrame::IsFileTypeFiltered(const wxString& path)
{

    wxString ext = wxFileName(path).GetExt().Lower();

    if (m_filterCpp->IsChecked() && (ext == "cpp" || ext == "c")) return true;
    if (m_filterH->IsChecked() && (ext == "h" || ext == "hpp")) return true;
    if (m_filterPy->IsChecked() && ext == "py") return true;
    if (m_filterJs->IsChecked() && ext == "js") return true;
    if (m_filterHtml->IsChecked() && (ext == "html" || ext == "wxml")) return true;
    if (m_filterCss->IsChecked() && (ext == "css" || ext == "wxss")) return true;
    if (m_filterJson->IsChecked() && ext == "json") return true;
    if (m_filterMd->IsChecked() && ext == "md") return true;
    if (m_filterGd->IsChecked() && ext == "gd") return true;
    if (m_filterTscn->IsChecked() && ext == "tscn") return true;
    if (m_filterTres->IsChecked() && ext == "tres") return true;
    if (m_filterRes->IsChecked() && ext == "res") return true;
    if (m_filterCs->IsChecked() && ext == "cs") return true;
    if (m_filterShader->IsChecked() && ext == "shader") return true;
    if (m_filterUnity->IsChecked() && ext == "unity") return true;
    if (m_filterPrefab->IsChecked() && ext == "prefab") return true;
    if (m_filterTs->IsChecked() && ext == "ts") return true;
    if (m_filterMjs->IsChecked() && ext == "mjs") return true;
    if (m_filterCjs->IsChecked() && ext == "cjs") return true;

    return false;
}

void MainFrame::OnTreeSelectionChanged(wxTreeEvent& event)
{

    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk()) return;

    FileTreeData* data = (FileTreeData*)m_treeCtrl->GetItemData(itemId);
    if (data && !wxDir::Exists(data->GetPath()))
    {
        LoadFileContent(data->GetPath());
    }
}

void MainFrame::LoadFileContent(const wxString& filePath)
{

    wxTextFile file;
    if (!file.Open(filePath))
    {
        m_stc->SetText("Error: Could not open file " + filePath);
        m_stc->SetReadOnly(true);
        return;
    }

    wxString content; 
    for (size_t i = 0; i < file.GetLineCount(); ++i)
    {
        content += file.GetLine(i) + "\n";
    }
    file.Close();

    m_stc->SetReadOnly(false);
    m_stc->SetText(content);
    m_stc->SetReadOnly(true); 
    m_stc->EmptyUndoBuffer();
    SetEditorLexer(filePath);
    SetStatusText(filePath, 0);
}

void MainFrame::OnSaveCombined(wxCommandEvent& event)
{

    if (m_projectRoot.IsEmpty())
    {
        wxMessageBox("Please load a project folder first.", "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    wxString projectName = wxFileName(m_projectRoot).GetFullName();
    wxString date = wxDateTime::Now().Format("%Y-%m-%d");
    wxString defaultFilename = "combined_files.txt";

    switch(m_projectType)
    {
        case ProjectType::Godot:
            defaultFilename = "godot_" + projectName + "_" + date + ".txt";
            break;
        case ProjectType::Unity:
            defaultFilename = "unity_" + projectName + "_" + date + ".txt";
            break;
        case ProjectType::Node:
            defaultFilename = "node_" + projectName + "_" + date + ".txt";
            break;
        case ProjectType::None:
        case ProjectType::Other:
            defaultFilename = projectName + "_" + date + ".txt";
            break;
    }

    wxFileDialog saveDlg(this, "Save Combined File", "", defaultFilename, 
                         "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDlg.ShowModal() == wxID_CANCEL)
        return;

    wxString combinedContent;
    
    combinedContent += "# -------------------------------------------------------------------------\n";
    combinedContent += "# Project Structure\n";
    combinedContent += "# -------------------------------------------------------------------------\n";
    combinedContent += GenerateProjectTree(); 
    combinedContent += "\n";

    GetCombinedContent(m_treeCtrl->GetRootItem(), combinedContent);

    wxTextFile file;
    if (!file.Create(saveDlg.GetPath()) && !file.Open(saveDlg.GetPath()))
    {
        wxMessageBox("Could not save file to " + saveDlg.GetPath(), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    file.Clear();
    wxStringInputStream sstream(combinedContent);
    wxTextInputStream tstream(sstream); 
    while(!sstream.Eof()) 
    {
        file.AddLine(tstream.ReadLine());
    }
    file.Write();
    file.Close();

    SetStatusText("Combined file saved.", 0);
}

wxString MainFrame::GenerateProjectTree()
{

    wxString treeString;
    wxTreeItemId rootId = m_treeCtrl->GetRootItem();
    if (!rootId.IsOk()) return "";

    treeString += m_treeCtrl->GetItemText(rootId) + "/\n";
    BuildTreeRecursive(rootId, treeString, "    ");
    return treeString;
}

void MainFrame::BuildTreeRecursive(const wxTreeItemId& parentId, wxString& treeString, const wxString& indent)
{

    wxTreeItemIdValue cookie;
    wxTreeItemId childId = m_treeCtrl->GetFirstChild(parentId, cookie);

    while (childId.IsOk())
    {
        wxString name = m_treeCtrl->GetItemText(childId);
        wxTreeItemId nextChildId = m_treeCtrl->GetNextChild(parentId, cookie); 
        
        treeString += indent;
        treeString += (nextChildId.IsOk() ? "├── " : "└── ");
        treeString += name;

        FileTreeData* data = (FileTreeData*)m_treeCtrl->GetItemData(childId);
        if (data && wxDir::Exists(data->GetPath()))
        {
            treeString += "/\n";
            wxString nextIndent = indent + (nextChildId.IsOk() ? "│   " : "    ");
            BuildTreeRecursive(childId, treeString, nextIndent);
        }
        else
        {
            treeString += "\n";
        }
        
        childId = nextChildId;
    }
}


void MainFrame::GetCombinedContent(const wxTreeItemId& parentId, wxString& combinedContent)
{

    wxTreeItemIdValue cookie;
    wxTreeItemId childId = m_treeCtrl->GetFirstChild(parentId, cookie);

    while (childId.IsOk())
    {
        FileTreeData* data = (FileTreeData*)m_treeCtrl->GetItemData(childId);
        if (data)
        {
            wxString path = data->GetPath();
            if (wxDir::Exists(path))
            {
                GetCombinedContent(childId, combinedContent);
            }
            else
            {
                wxTextFile file;
                if (file.Open(path))
                {
                    wxString relativePath = GetRelativePath(path); // <-- CHANGED

                    combinedContent += "\n# -------------------------------------------------------------------------\n";
                    combinedContent += "# " + relativePath + "\n";
                    combinedContent += "# -------------------------------------------------------------------------\n";
                    
                    for (size_t i = 0; i < file.GetLineCount(); ++i)
                    {
                        combinedContent += file.GetLine(i) + "\n";
                    }
                    file.Close();
                }
            }
        }
        childId = m_treeCtrl->GetNextChild(parentId, cookie);
    }
}


// --- Button/Menu Handlers ---
void MainFrame::OnCopy(wxCommandEvent& event) { m_stc->Copy(); }
void MainFrame::OnSelectAll(wxCommandEvent& event) { m_stc->SelectAll(); }
void MainFrame::OnExit(wxCommandEvent& event) { Close(true); }
void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxString aboutMessage;
    aboutMessage += "Script Combiner (Native)\n";
    aboutMessage += "\n";
    aboutMessage += "@2025\n"; 
    aboutMessage += "Build with love for AI Prompting\n";
    aboutMessage += "a native remake of the web-based tool.\n";
    aboutMessage += "https://github.com/adtpdn/script-combiner\n";
    aboutMessage += "---\n";
    aboutMessage += "donate paypal : overkaffeinated@gmail.com";

    wxMessageBox(aboutMessage,
                 "About Script Combiner", wxOK | wxICON_INFORMATION, this);
}

// --- (OnToggleWordWrap, OnPreset... are unchanged) ---
void MainFrame::OnToggleWordWrap(wxCommandEvent& event)
{
    bool isChecked = m_menuBar->IsChecked(ID_ToggleWordWrap);
    if (isChecked) {
        m_stc->SetWrapMode(wxSTC_WRAP_WORD);
    } else {
        m_stc->SetWrapMode(wxSTC_WRAP_NONE);
    }
}

void MainFrame::OnPresetGodot(wxCommandEvent& event)
{
    m_ignorePatternsText->SetValue(GODOT_IGNORE_PATTERNS);
    m_projectType = ProjectType::Godot;
    UpdateFilterCheckboxes(m_projectType);
    PopulateFileTree(); 
}

void MainFrame::OnPresetUnity(wxCommandEvent& event)
{
    m_ignorePatternsText->SetValue(UNITY_IGNORE_PATTERNS);
    m_projectType = ProjectType::Unity;
    UpdateFilterCheckboxes(m_projectType);
    PopulateFileTree(); 
}

void MainFrame::OnPresetNode(wxCommandEvent& event)
{
    m_ignorePatternsText->SetValue(NODE_IGNORE_PATTERNS);
    m_projectType = ProjectType::Node;
    UpdateFilterCheckboxes(m_projectType);
    PopulateFileTree(); 
}

void MainFrame::UpdateFilterCheckboxes(ProjectType type)
{

    m_filterCpp->SetValue(false);
    m_filterH->SetValue(false);
    m_filterPy->SetValue(false);
    m_filterJs->SetValue(false);
    m_filterHtml->SetValue(false);
    m_filterCss->SetValue(false);
    m_filterJson->SetValue(false);
    m_filterMd->SetValue(false);
    m_filterGd->SetValue(false);
    m_filterTscn->SetValue(false);
    m_filterTres->SetValue(false);
    m_filterRes->SetValue(false);
    m_filterCs->SetValue(false);
    m_filterShader->SetValue(false);
    m_filterUnity->SetValue(false);
    m_filterPrefab->SetValue(false);
    m_filterTs->SetValue(false);
    m_filterMjs->SetValue(false);
    m_filterCjs->SetValue(false);

    switch(type)
    {
        case ProjectType::Godot:
            m_filterGd->SetValue(true);
            m_filterTscn->SetValue(true); 
            m_filterTres->SetValue(true);
            m_filterRes->SetValue(true);
            m_filterShader->SetValue(true); 
            m_filterJson->SetValue(true); 
            break;
        case ProjectType::Unity:
            m_filterCs->SetValue(true);
            m_filterShader->SetValue(true);
            m_filterUnity->SetValue(true);
            m_filterPrefab->SetValue(true);
            m_filterJson->SetValue(true); 
            break;
        case ProjectType::Node:
            m_filterJs->SetValue(true);
            m_filterTs->SetValue(true);
            m_filterMjs->SetValue(true);
            m_filterCjs->SetValue(true);
            m_filterJson->SetValue(true); 
            m_filterMd->SetValue(true); 
            break;
        case ProjectType::None:
        case ProjectType::Other:
            m_filterCpp->SetValue(true);
            m_filterH->SetValue(true);
            m_filterPy->SetValue(true);
            m_filterJs->SetValue(true);
            m_filterHtml->SetValue(true);
            m_filterCss->SetValue(true);
            m_filterJson->SetValue(true);
            m_filterMd->SetValue(true);
            break;
    }
}

// --- ADDED: New Helper Function ---
wxString MainFrame::GetRelativePath(const wxString& fullPath)
{
    wxFileName fn(fullPath);
    wxString relativePath = fn.GetFullPath();
    relativePath.Replace(m_projectRoot, "");
    if (relativePath.StartsWith(wxFileName::GetPathSeparator())) {
        relativePath = relativePath.Mid(1);
    }
    relativePath.Replace(wxFileName::GetPathSeparator(), "/"); // Normalize
    return relativePath;
}

// --- ADDED: All New Event Handlers ---

void MainFrame::OnTreeRightClick(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk() || itemId == m_treeCtrl->GetRootItem())
    {
        return; // Don't show menu for root or invalid item
    }

    m_treeCtrl->SelectItem(itemId); // Select the item that was right-clicked

    FileTreeData* data = (FileTreeData*)m_treeCtrl->GetItemData(itemId);
    if (!data) return;

    m_rightClickedPath = data->GetPath(); // Store the path for the handlers
    bool isDir = wxDir::Exists(m_rightClickedPath);

    wxMenu contextMenu;
    contextMenu.Append(ID_IgnoreItem, "Ignore \"" + m_treeCtrl->GetItemText(itemId) + "\"");
    
    if (!isDir)
    {
        wxString ext = wxFileName(m_rightClickedPath).GetExt();
        if (!ext.IsEmpty())
        {
            contextMenu.Append(ID_IgnoreExtension, "Ignore *." + ext);
        }
    }

    // Bind the menu events just before showing
    Bind(wxEVT_MENU, &MainFrame::OnIgnoreItem, this, ID_IgnoreItem);
    Bind(wxEVT_MENU, &MainFrame::OnIgnoreExtension, this, ID_IgnoreExtension);

    PopupMenu(&contextMenu);

    // Unbind to avoid conflicts
    Unbind(wxEVT_MENU, &MainFrame::OnIgnoreItem, this, ID_IgnoreItem);
    Unbind(wxEVT_MENU, &MainFrame::OnIgnoreExtension, this, ID_IgnoreExtension);
}

void MainFrame::OnIgnoreItem(wxCommandEvent& event)
{
    if (m_rightClickedPath.IsEmpty()) return;

    wxString relativePath = GetRelativePath(m_rightClickedPath);
    
    if (wxDir::Exists(m_rightClickedPath))
    {
        relativePath += "/"; // Add trailing slash for directories
    }

    m_ignorePatternsText->AppendText("\n" + relativePath);
    PopulateFileTree(); // This will re-trigger settings changed
}

void MainFrame::OnIgnoreExtension(wxCommandEvent& event)
{
    if (m_rightClickedPath.IsEmpty()) return;

    wxString ext = wxFileName(m_rightClickedPath).GetExt();
    if (!ext.IsEmpty())
    {
        m_ignorePatternsText->AppendText("\n*." + ext);
        PopulateFileTree(); // This will re-trigger settings changed
    }
}

void MainFrame::OnLoadIgnoreFile(wxCommandEvent& event)
{
    wxFileDialog openDlg(this, "Load Ignore File", "", "",
                         "Ignore files (*.gitignore;*.txt)|*.gitignore;*.txt|All files (*.*)|*.*",
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openDlg.ShowModal() == wxID_CANCEL)
        return;

    wxTextFile file;
    if (!file.Open(openDlg.GetPath()))
    {
        wxMessageBox("Could not open file " + openDlg.GetPath(), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    wxString content;
    for (size_t i = 0; i < file.GetLineCount(); ++i)
    {
        content += file.GetLine(i) + "\n";
    }
    file.Close();

    m_ignorePatternsText->SetValue(content);
    PopulateFileTree(); // Will trigger settings changed
}

void MainFrame::OnSaveIgnoreFile(wxCommandEvent& event)
{
    wxFileDialog saveDlg(this, "Save Ignore File As...", "", ".gitignore",
                         "Ignore files (*.gitignore;*.txt)|*.gitignore;*.txt|All files (*.*)|*.*",
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveDlg.ShowModal() == wxID_CANCEL)
        return;

    wxTextFile file;
    if (!file.Create(saveDlg.GetPath()) && !file.Open(saveDlg.GetPath()))
    {
        wxMessageBox("Could not save file to " + saveDlg.GetPath(), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    file.Clear();
    wxStringInputStream sstream(m_ignorePatternsText->GetValue());
    wxTextInputStream tstream(sstream); 
    while(!sstream.Eof()) 
    {
        file.AddLine(tstream.ReadLine());
    }
    file.Write();
    file.Close();

    SetStatusText("Ignore file saved.", 0);
}