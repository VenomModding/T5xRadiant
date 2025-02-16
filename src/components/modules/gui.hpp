#pragma once

namespace components
{
	class gui : public component
	{
	public:
		gui();
		~gui();

		const char* get_name() override { return "gui"; };

		//static void imgui_init_ccamerawnd();
		static void shutdown();
		
		static void begin_frame();
		static void end_frame(); 

		static void saved_windowstates();
		static void render_loop();

		static void register_dvars();
		//static bool any_open_menus();

	private:
	};
}
