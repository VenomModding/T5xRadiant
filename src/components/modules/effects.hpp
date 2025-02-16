#pragma once

namespace components
{
	class effects : public component
	{
	public:
		effects();
		~effects();

		const char* get_name() override { return "effects"; };

		static fx_system::FxEffect* Editor_SpawnEffect(int localClientNum, fx_system::FxEffectDef* remoteDef, int msecBegin, const float* origin, const float(*axis)[3], int markEntnum);
		static bool load_effect(const char* effect_name);

		static void editor_on_effect_play_repeat();
		static void editor_trigger_effect(int msecBegin);

		static bool effect_is_paused();
		static bool effect_is_playing();
		static bool effect_is_repeating();
		static bool effect_can_play();

		static bool is_fx_origin_selected();
		static void fx_origin_frame();

		static void tick_playback();
		static void tick_repeat();

		static void on_effect_stop();
		static void reset_editor_effect();
		static void radiant_init_fx();

		static void generate_createfx(bool copy_to_raw = false);
		static void set_initial_state();

		static void play();
		static void play_retrigger();
		static void apply_changes();
		static void repeat();
		static void pause();
		static void stop();
		static void stop_all();
		static void edit();


		static inline bool			is_fx_origin_selected_;
		static inline std::string	last_fx_name_;

	private:
	};
}
