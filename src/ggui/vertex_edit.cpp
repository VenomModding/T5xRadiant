#include "std_include.hpp"
#include <random>

namespace ggui
{
	void vertex_edit_dialog::feature_noise()
	{
		ImGui::title_with_seperator_helpmark("Noise", true, 0, 2.0f, 8.0f,
			"[1] Select a single patch\n[2] Select vertices you want to apply noise to");

		static game::vec3_t noise_scale = { 0.0f, 0.0f, 10.0f };

		const auto patch = game::g_selected_brushes_next()->patch;
		const bool enable_noise =  game::is_single_brush_selected(false)
								&& patch && patch->def
								&& game::g_qeglobals->d_num_move_points > 0;

		ImGui::BeginDisabled(!enable_noise);
		{
			static int noise_mode = 0;
			const char* noise_modes_str[4] = { "Height", "Alpha" };
			ImGui::SliderInt("Mode", &noise_mode, 0, 1, noise_modes_str[noise_mode]);

			ImGui::BeginDisabled(noise_mode);
			{
				ImGui::DragFloat3("XYZ Height Scale", noise_scale, 0.1f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::EndDisabled();
			}

			if (ImGui::Button("Add Noise", ImVec2(ImGui::GetItemRectSize().x, ImGui::GetFrameHeight())))
			{
				// generate a random number within a certain range
				std::random_device	rd;
				std::mt19937		gen(rd());

				std::uniform_real_distribution<float> distr(-100, 100);

				for (auto pt = 0; pt < game::g_qeglobals->d_num_move_points; pt++)
				{
					const auto vert = game::g_qeglobals->d_move_points[pt];

					const float rand1 = distr(gen) * 0.01f;
					const float rand2 = distr(gen) * 0.01f;

					if (noise_mode == 0)
					{
						vert->xyz[0] += (rand1 * noise_scale[0]);
						vert->xyz[1] += (rand2 * noise_scale[1]);
						vert->xyz[2] += (rand1 * noise_scale[2]);
					}
					else if (noise_mode == 1)
					{
						vert->vert_color.a = utils::pack_float(fabs(rand1));
					}
				}

				if (patch && patch->def)
				{
					game::Patch_UpdateSelected(patch->def, true);
				}
			}

			ImGui::EndDisabled();
		}
	}

	bool vertex_edit_dialog::gui()
	{
		const auto MIN_WINDOW_SIZE = ImVec2(400.0f, 200.0f);
		const auto INITIAL_WINDOW_SIZE = ImVec2(400.0f, 800.0f);

		ImGui::SetNextWindowSizeConstraints(MIN_WINDOW_SIZE, ImVec2(FLT_MAX, FLT_MAX));
		ImGui::SetNextWindowSize(INITIAL_WINDOW_SIZE, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ggui::get_initial_window_pos(), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Vertex Edit Dialog##window", this->get_p_open(), ImGuiWindowFlags_NoCollapse))
		{
			ImGui::End();
			return false ;
		}

		ImGui::Indent(4.0f);
		SPACING(0.0f, 2.0f);

		ImGui::title_with_seperator_helpmark("Vertex Color", false, 0, 2.0f, 8.0f,
			"Select vertices to enable feature");

		static float vertex_edit_color[4] = {};
		const bool enable_vert_color_edit = game::g_qeglobals->d_num_move_points > 0;

		bool		 manual_edit = false;
		float		 manual_edit_steps[4] = {};
		int			 manual_edit_dir[4] = { 1, 1, 1, 1 };
		static float manual_edit_amount[4] = { 0.1f, 0.1f, 0.1f, 0.1f };

		if (ImGui::TreeNodeEx("Manual Offsets", ImGuiTreeNodeFlags_SpanFullWidth))
		{
			ImGui::BeginDisabled(!enable_vert_color_edit);
			{
				imgui::Spacing();
				imgui::Indent(-28.0f);
				if (ImGui::InputScalarDir("Red", ImGuiDataType_Float, &manual_edit_steps[0], &manual_edit_dir[0], &manual_edit_amount[0], nullptr, true, "", ImGuiInputTextFlags_None, true))
				{
					manual_edit = true;
				}

				if (ImGui::InputScalarDir("Green", ImGuiDataType_Float, &manual_edit_steps[1], &manual_edit_dir[1], &manual_edit_amount[1], nullptr, true, "", ImGuiInputTextFlags_None, true))
				{
					manual_edit = true;
				}

				const auto pre_text_cursor_pos = imgui::GetCursorPos();

				if (ImGui::InputScalarDir("Blue", ImGuiDataType_Float, &manual_edit_steps[2], &manual_edit_dir[2], &manual_edit_amount[2], nullptr, true, "", ImGuiInputTextFlags_None, true))
				{
					manual_edit = true;
				}

				if (ImGui::InputScalarDir("Alpha", ImGuiDataType_Float, &manual_edit_steps[3], &manual_edit_dir[3], &manual_edit_amount[3], nullptr, true, "", ImGuiInputTextFlags_None, true))
				{
					manual_edit = true;
				}

				ImGui::EndDisabled();

				const auto post_widget_cursor_pos = imgui::GetCursorPos();

				imgui::SetCursorPos(ImVec2(pre_text_cursor_pos.x + 200.0f, pre_text_cursor_pos.y - 60.0f));
				imgui::TextUnformatted("  Used to Add / Subtract\n      vertex color-values\ninstead of replacing them\n   with the widget below\n\n        (You can hold +/-)");

				imgui::SetCursorPos(post_widget_cursor_pos);

				imgui::Spacing();
				imgui::Separator();
				imgui::Spacing();

				imgui::Indent(28.0f);
			}

			ImGui::TreePop();
		}

		imgui::Spacing();

		ImGui::BeginDisabled(!enable_vert_color_edit);
		{
			if (ImGui::ColorPicker4("Vertex Color", vertex_edit_color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB) || manual_edit)
			{
				for (auto pt = 0; pt < game::g_qeglobals->d_num_move_points; pt++)
				{
					const auto vert = game::g_qeglobals->d_move_points[pt];

					if (manual_edit)
					{
						vert->vert_color.r = utils::pack_float(utils::unpack_float(vert->vert_color.r) + manual_edit_steps[0]);
						vert->vert_color.g = utils::pack_float(utils::unpack_float(vert->vert_color.g) + manual_edit_steps[1]);
						vert->vert_color.b = utils::pack_float(utils::unpack_float(vert->vert_color.b) + manual_edit_steps[2]);
						vert->vert_color.a = utils::pack_float(utils::unpack_float(vert->vert_color.a) + manual_edit_steps[3]);
					}
					else
					{
						vert->vert_color.r = utils::pack_float(vertex_edit_color[0]);
						vert->vert_color.g = utils::pack_float(vertex_edit_color[1]);
						vert->vert_color.b = utils::pack_float(vertex_edit_color[2]);
						vert->vert_color.a = utils::pack_float(vertex_edit_color[3]);
					}
				}

				FOR_ALL_SELECTED_BRUSHES(sb)
				{
					if (sb->patch && sb->patch->def)
					{
						game::Patch_UpdateSelected(sb->patch->def, true);
					}
				}
			}

			ImGui::EndDisabled();
		}

		// add random noise to selected vertices
		vertex_edit_dialog::feature_noise();

		ImGui::End();
		return true;
	}

	void vertex_edit_dialog::on_vertex_edit_dialog_command()
	{
		if (dvars::gui_use_new_vertedit_dialog->current.enabled)
		{
			GET_GUI(vertex_edit_dialog)->toggle();
			return;
		}

		// original dialog
		auto* v_edit = reinterpret_cast<CWnd*>(0x25D65B0);
		utils::hook::call<void(__fastcall)(CWnd*, int, int)>(0x58EA4F)(v_edit, 0, IsWindowVisible(v_edit->GetWindow()) ? SW_HIDE : SW_SHOW);
	}

	void vertex_edit_dialog::hooks()
	{
		utils::hook::detour(0x42BCD0, vertex_edit_dialog::on_vertex_edit_dialog_command, HK_JUMP);
	}

	void vertex_edit_dialog::register_dvars()
	{
		dvars::gui_use_new_vertedit_dialog = dvars::register_bool(
			/* name		*/ "gui_use_new_vertedit_dialog",
			/* default	*/ true,
			/* flags	*/ game::dvar_flags::saved,
			/* desc		*/ "Overwrites default hotkey");
	}

	REGISTER_GUI(vertex_edit_dialog);
}
