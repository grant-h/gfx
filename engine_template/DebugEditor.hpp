#ifndef _DEBUG_EDITOR_H
#define _DEBUG_EDITOR_H

#include <vector>
#include <memory>

#include <Shader.hpp>
#include <ResourceManager.hpp>

#include <Renderer.hpp>
#include <imgui.h>
#include <TextEditor.h>

extern const TextEditor::Palette custom_palette;

class DebugEditor {
  private:
    TextEditor text_editor_;
    std::shared_ptr<Shader> current_shader_;
    std::string file_name_;
  public:
    DebugEditor() {
      auto lang = TextEditor::LanguageDefinition::GLSL();
      current_shader_ = ResourceManager::instance()->get_shader("Phong.frag");
      text_editor_.SetText(current_shader_->get_code());
      text_editor_.SetLanguageDefinition(lang);
      text_editor_.SetPalette(custom_palette);
    }
    ~DebugEditor() = default;

    void draw()
    {
      auto cpos = text_editor_.GetCursorPosition();
      ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
      ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
      if (ImGui::BeginMenuBar())
      {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Save", "Ctrl-S", nullptr, true))
          {
            auto textToSave = text_editor_.GetText();
            current_shader_->set_code(textToSave);
            ResourceManager::instance()->recompile_shader(current_shader_);
          }
          if (ImGui::MenuItem("Quit", "Alt-F4"))
            ;
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
          bool ro = text_editor_.IsReadOnly();
          if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
            text_editor_.SetReadOnly(ro);
          ImGui::Separator();

          if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && text_editor_.CanUndo()))
            text_editor_.Undo();
          if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && text_editor_.CanRedo()))
            text_editor_.Redo();

          ImGui::Separator();

          if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, text_editor_.HasSelection()))
            text_editor_.Copy();
          if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && text_editor_.HasSelection()))
            text_editor_.Cut();
          if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && text_editor_.HasSelection()))
            text_editor_.Delete();
          if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
            text_editor_.Paste();

          ImGui::Separator();

          if (ImGui::MenuItem("Select all", nullptr, nullptr))
            text_editor_.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(text_editor_.GetTotalLines(), 0));

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
          if (ImGui::MenuItem("Dark palette"))
            text_editor_.SetPalette(TextEditor::GetDarkPalette());
          if (ImGui::MenuItem("Light palette"))
            text_editor_.SetPalette(TextEditor::GetLightPalette());
          if (ImGui::MenuItem("Retro blue palette"))
            text_editor_.SetPalette(TextEditor::GetRetroBluePalette());
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, text_editor_.GetTotalLines(),
          text_editor_.IsOverwrite() ? "Ovr" : "Ins",
          text_editor_.CanUndo() ? "*" : " ",
          text_editor_.GetLanguageDefinition().mName.c_str(), file_name_.c_str());

      float alpha_before = ImGui::GetStyle().Alpha;
      ImVec4* colors = ImGui::GetStyle().Colors;
      colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.70f);
      //ImGui::GetStyle().Alpha = 0.55f;
      //ImGui::GetStyle().WindowBG = 0.00f;
      text_editor_.Render("TextEditor");
      ImGui::GetStyle().Alpha = alpha_before;

      ImGui::End();
    }
};

#endif // _DEBUG_EDITOR_H
