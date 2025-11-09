#pragma once

#include <wx/wx.h>
#include <wx/stc/stc.h> // For wxStyledTextCtrl
#include <wx/treectrl.h>  // For wxTreeCtrl
#include <wx/collpane.h> // For wxCollapsiblePane (accordion)
#include <wx/textctrl.h> // For wxTextCtrl (ignore patterns)
#include <wx/checkbox.h> // For wxCheckBox
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <wx/imaglist.h> // <-- ADDED: Header for wxImageList

// This class will hold file data in our tree
class FileTreeData : public wxTreeItemData
{
public:
    FileTreeData(const wxString& path) : m_path(path) {}
    const wxString& GetPath() const { return m_path; }
private:
    wxString m_path;
};

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    // --- Core UI Components ---
    wxPanel* m_leftPanel;
    wxPanel* m_rightPanel;
    wxTreeCtrl* m_treeCtrl;
    wxStyledTextCtrl* m_stc;
    wxStatusBar* m_statusBar;
    wxMenuBar* m_menuBar; // <-- ADDED: Make menu bar a member

    // --- Sidebar Settings Components ---
    wxCollapsiblePane* m_settingsPane;
    wxCheckBox* m_showHiddenCheck;
    wxTextCtrl* m_ignorePatternsText;

    wxCollapsiblePane* m_filterPane;
    // --- Filter Checkboxes (Expanded) ---
    wxCheckBox* m_filterCpp;
    wxCheckBox* m_filterH;
    wxCheckBox* m_filterPy;
    wxCheckBox* m_filterJs;
    wxCheckBox* m_filterHtml;
    wxCheckBox* m_filterCss;
    wxCheckBox* m_filterJson;
    wxCheckBox* m_filterMd;
    // Godot
    wxCheckBox* m_filterGd;
    wxCheckBox* m_filterTscn;
    wxCheckBox* m_filterTres;
    wxCheckBox* m_filterRes;
    // Unity
    wxCheckBox* m_filterCs;
    wxCheckBox* m_filterShader;
    wxCheckBox* m_filterUnity;
    wxCheckBox* m_filterPrefab;
    // Node
    wxCheckBox* m_filterTs;
    wxCheckBox* m_filterMjs;
    wxCheckBox* m_filterCjs;
    
    // --- Internal State ---
    wxString m_projectRoot;
    wxArrayString m_ignorePatterns;
    wxImageList* m_imageList; 
    wxString m_rightClickedPath; // <-- ADDED: For context menu
    
    enum class ProjectType { None, Godot, Unity, Node, Other };
    ProjectType m_projectType;

    // --- Event Handlers ---
    void OnLoadProject(wxCommandEvent& event);
    void OnSaveCombined(wxCommandEvent& event);
    void OnTreeSelectionChanged(wxTreeEvent& event);
    void OnTreeRightClick(wxTreeEvent& event); // <-- ADDED
    void OnCopy(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSettingsChanged(wxCommandEvent& event); // For filters/hidden
    void OnToggleWordWrap(wxCommandEvent& event);
    // Preset Handlers
    void OnPresetGodot(wxCommandEvent& event);
    void OnPresetUnity(wxCommandEvent& event);
    void OnPresetNode(wxCommandEvent& event);
    // Context Menu Handlers
    void OnIgnoreItem(wxCommandEvent& event); // <-- ADDED
    void OnIgnoreExtension(wxCommandEvent& event); // <-- ADDED
    // Ignore File Handlers
    void OnLoadIgnoreFile(wxCommandEvent& event); // <-- ADDED
    void OnSaveIgnoreFile(wxCommandEvent& event); // <-- ADDED

    // --- Helper Functions ---
    void CreateLayout();
    wxPanel* CreateLeftPanel(wxWindow* parent);
    wxPanel* CreateRightPanel(wxWindow* parent);
    void SetupMenuBar();
    void SetupEditorStyles();
    void SetEditorLexer(const wxString& filePath);
    
    void PopulateFileTree();
    void ScanDirectoryRecursive(const wxString& path, const wxTreeItemId& parentId);
    bool ShouldIgnore(const wxString& path, bool isDir);
    bool IsFileTypeFiltered(const wxString& path);

    wxString GenerateProjectTree(); 
    void BuildTreeRecursive(const wxTreeItemId& parentId, wxString& treeString, const wxString& indent); 

    void GetCombinedContent(const wxTreeItemId& parentId, wxString& combinedContent);
    void LoadFileContent(const wxString& filePath);

    void UpdateFilterCheckboxes(ProjectType type); 

    wxString GetRelativePath(const wxString& fullPath); // <-- ADDED

    // Enum for event IDs
    enum {
        ID_LoadProject = wxID_HIGHEST + 1,
        ID_SaveCombined,
        ID_Copy,
        ID_SelectAll,
        ID_SettingsChanged,
        ID_ToggleWordWrap,
        ID_PresetGodot,
        ID_PresetUnity,
        ID_PresetNode,
        ID_IgnoreItem,      // <-- ADDED
        ID_IgnoreExtension, // <-- ADDED
        ID_LoadIgnoreFile,  // <-- ADDED
        ID_SaveIgnoreFile   // <-- ADDED
    };
};