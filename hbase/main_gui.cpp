#include"main_gui.h"
#include"script.h"
#include"settings.h"
#include"fiber_pool.h"
#include"gta_util.hpp"
#include"features.h"
#include"mod_vehicle.h"
#include"script_global.h"
#include"data/player.h"
#include"data/vehicles.h"
#include"data/lists.h"
#include"enums.h"
#include"ui/ui_manager.h"
#include"ui/regular_submenu.h"
#include"ui/regular_option.h"
#include"ui/sub_option.h"
#include"ui/bool_option.h"
#include"ui/number_option.h"
#include"ui/choose_option.h"
#include"ui/break_option.h"
#include"ui/vehicle_option.h"
#include"ui/player_submenu.h"

namespace hbase
{
	enum Submenu : std::uint32_t
	{
		protection_option,
		script_protection,
		net_event_protection,
		other_protection,
		advertise_protection,
		crash_protection,
		onlie_option,
		rid_jion,
		remove_player,
		host_tools,
		info_spoof,
		self_option,
		change_player_model,
		player_list,
		task_option,
		pericoIsland,
		casinorobbery,
		doomsdayheist,
		apartment,
		contracttask,
		one_player_option,
		remove_one_player,
		teleport_player,
		friendly_option,
		attack_options,
		weapon_option,
		create_vehicle,
		create_xml_vehicle,
		create_ini_vehicle,
		vehicle_option,
		teleport_option,
		menu_seetings
	};

	void main_gui::initialize()
	{
		m_initialized = true;
		std::size_t SELF = g_ui_mgr->addSub("自我选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(SELF, "自我选项", self_option, [](RegularSubmenu* sub)
			{
				static int wanted_level = 0;
				sub->add_option<BoolOption<bool>>("无敌", "不会死亡", g_settings.options["god_mode"].get<bool*>());
				sub->add_option<BoolOption<bool>>("永不通缉", "不可见", g_settings.options["never_wanted"].get<bool*>());
				sub->add_option<NumberOption<int>>("通缉等级", "更改通缉等级", &wanted_level, 0, 5, 1, 2, true, [] {features::set_player_wanted_level(wanted_level); });
				sub->add_option<BreakOption>("外观");
				sub->add_option<SubOption>("更改玩家模型", "更改玩家模型", change_player_model);
				sub->add_option<BoolOption<bool>>("隐身", "不可见", g_settings.options["invisible"].get<bool*>());
				sub->add_option<BreakOption>("运动");
				sub->add_option<NumberOption<float>>("移动速度", "更改移动速度", &features::run_speed, 0, 5, 0.1);
				sub->add_option<NumberOption<float>>("游泳速度", "更改游泳速度", &features::swim_speed, 0, 5, 0.1);
				sub->add_option<BoolOption<bool>>("超级跳跃", "跳的很高", g_settings.options["super_jump"].get<bool*>());
				sub->add_option<RegularOption>("加满血量", "恢复全部血量", [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), ENTITY::GET_ENTITY_MAX_HEALTH(PLAYER::PLAYER_PED_ID()), 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("加满护甲", "恢复全部护甲", [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						PED::ADD_ARMOUR_TO_PED(PLAYER::PLAYER_PED_ID(), 400);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("清洁身体", "清洁身体", [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						PED::CLEAR_ALL_PED_PROPS(PLAYER::PLAYER_PED_ID());
						PED::CLEAR_PED_BLOOD_DAMAGE(PLAYER::PLAYER_PED_ID());
						PED::CLEAR_PED_WETNESS(PLAYER::PLAYER_PED_ID());
						PED::CLEAR_PED_DECORATIONS(PLAYER::PLAYER_PED_ID());
						PED::CLEAR_PED_DECORATIONS_LEAVE_SCARS(PLAYER::PLAYER_PED_ID());
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("自杀", "自杀", [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(SELF, "更改玩家模型", change_player_model, [](RegularSubmenu* sub)
			{
				static std::size_t ped_id{};
				sub->add_option<ChooseOption< const char*, std::size_t>>("模型列表", "选择后按下确认更换", &lists::player_model, &ped_id, false, [&] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::chage_player_model(MISC::GET_HASH_KEY(lists::player_model[ped_id]));
					}QUEUE_JOB_END_CLAUSE
					});
			});

		std::size_t PROTECTION = g_ui_mgr->addSub("防护选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "防护选项", protection_option, [](RegularSubmenu* sub)
			{
				sub->add_option<SubOption>("网络事件", nullptr, net_event_protection);
				sub->add_option<SubOption>("脚本防护", nullptr, script_protection);
				sub->add_option<SubOption>("其他防护", nullptr, other_protection);
				sub->add_option<SubOption>("崩溃防护", nullptr, crash_protection);
				sub->add_option<SubOption>("广告防护", nullptr, advertise_protection);
				sub->add_option<BoolOption<bool>>("脚本踢出&脚本崩溃", "通过freemodeStack恢复运行", g_settings.options["script_kick_and_crash"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "脚本防护", script_protection, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("赏金", nullptr, g_settings.options["bounty"].get<bool*>());
				sub->add_option<BoolOption<bool>>("禁止CEO", nullptr, g_settings.options["ceo_ban"].get<bool*>());
				sub->add_option<BoolOption<bool>>("踢CEO", nullptr, g_settings.options["ceo_kick"].get<bool*>());
				sub->add_option<BoolOption<bool>>("CEO钱", nullptr, g_settings.options["ceo_money"].get<bool*>());
				sub->add_option<BoolOption<bool>>("清除通缉等级", nullptr, g_settings.options["clear_wanted_level"].get<bool*>());
				sub->add_option<BoolOption<bool>>("假存款", nullptr, g_settings.options["fake_deposit"].get<bool*>());
				sub->add_option<BoolOption<bool>>("强制任务", nullptr, g_settings.options["force_mission"].get<bool*>());
				sub->add_option<BoolOption<bool>>("横幅", nullptr, g_settings.options["gta_banner"].get<bool*>());
				sub->add_option<BoolOption<bool>>("网络保释", nullptr, g_settings.options["network_bail"].get<bool*>());
				sub->add_option<BoolOption<bool>>("个人车辆被毁", nullptr, g_settings.options["personal_vehicle_destroyed"].get<bool*>());
				sub->add_option<BoolOption<bool>>("脱离雷达", nullptr, g_settings.options["remote_off_radar"].get<bool*>());
				sub->add_option<BoolOption<bool>>("旋转屏幕", nullptr, g_settings.options["rotate_cam"].get<bool*>());
				sub->add_option<BoolOption<bool>>("进入过场动画", nullptr, g_settings.options["send_to_cutscene"].get<bool*>());
				sub->add_option<BoolOption<bool>>("进入小岛", nullptr, g_settings.options["send_to_island"].get<bool*>());
				sub->add_option<BoolOption<bool>>("垃圾邮件", nullptr, g_settings.options["sound_spam"].get<bool*>());
				sub->add_option<BoolOption<bool>>("旋转屏幕", nullptr, g_settings.options["rotate_cam"].get<bool*>());
				sub->add_option<BoolOption<bool>>("观看", nullptr, g_settings.options["spectate"].get<bool*>());
				sub->add_option<BoolOption<bool>>("强制传送", nullptr, g_settings.options["force_teleport"].get<bool*>());
				sub->add_option<BoolOption<bool>>("交易错误", nullptr, g_settings.options["transaction_error"].get<bool*>());
				sub->add_option<BoolOption<bool>>("踢出载具", nullptr, g_settings.options["vehicle_kick"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "网络事件", net_event_protection, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("冻结", nullptr, g_settings.options["freeze"].get<bool*>());
				sub->add_option<BoolOption<bool>>("控制", nullptr, g_settings.options["control"].get<bool*>());
				sub->add_option<BoolOption<bool>>("举报", nullptr, g_settings.options["increment_stat_event"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "其他防护", other_protection, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("禁用错误屏幕", nullptr, g_settings.options["disable_error_screen"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "广告防护", advertise_protection, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("拦截广告信息", nullptr, g_settings.options["block_ad"].get<bool*>());
				sub->add_option<BoolOption<bool>>("崩溃广告发送者", nullptr, g_settings.options["crash_ad"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PROTECTION, "崩溃防护", crash_protection, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("无效同步类型", nullptr, g_settings.options["invalid_sync_type"].get<bool*>());
				sub->add_option<BoolOption<bool>>("同步模型更改", nullptr, g_settings.options["sync_model_mismatch"].get<bool*>());
				sub->add_option<BoolOption<bool>>("模型", nullptr, g_settings.options["model_crash"].get<bool*>());
				sub->add_option<BoolOption<bool>>("NetArray", nullptr, g_settings.options["net_array"].get<bool*>());
				sub->add_option<BoolOption<bool>>("PlayerModel", nullptr, g_settings.options["sync_player_model"].get<bool*>());
			});

		std::size_t ONLINE = g_ui_mgr->addSub("在线选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(ONLINE, "在线选项", onlie_option, [](RegularSubmenu* sub)
			{
				sub->add_option<SubOption>("RID加入", nullptr, rid_jion);
				sub->add_option<SubOption>("信息欺骗", nullptr, info_spoof);
				sub->add_option<SubOption>("移除玩家", nullptr, remove_player);
				sub->add_option<SubOption>("主机工具", nullptr, host_tools);
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(ONLINE, "RID加入", rid_jion, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>(std::move(RegularOption("RID").SetRightText(std::to_string(g_settings.options["join_rid"].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入", "输入RID内容", [] {
					g_ui_mgr->inputBox([] {g_settings.options["join_rid"] = atoi(g_ui_mgr->getInputText().c_str()); });
					});
				sub->add_option<RegularOption>("加入", "加入指定RID的战局", [] {
					g_fiber_pool->queue_job([]
						{
							if (g_pointers->m_menu_ptr)
							{
								do
								{
									HUD::ACTIVATE_FRONTEND_MENU(0xD528C7E2, 0, 2);
									script::get_current()->yield(1s);

									if (*g_pointers->m_menu_ptr)
									{
										int hash = -632792895;
										int64_t ptr = *g_pointers->m_menu_ptr;
										g_pointers->m_rid_joiner(&ptr, &hash);
									}
								} while (!*g_pointers->m_menu_ptr);
							}
						});
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(ONLINE, "移除玩家", remove_player, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("崩溃主机", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						Vector3 pos = { -16257.853, 10793.688, 893.610 };
						Ped myPed = PLAYER::PLAYER_PED_ID();
						Vector3 myPos = ENTITY::GET_ENTITY_COORDS(myPed, false);
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(myPed, pos.x, pos.y, pos.z, false, false, false);
						ENTITY::FREEZE_ENTITY_POSITION(myPed, true);
						script::get_current()->yield(3s);
						ENTITY::FREEZE_ENTITY_POSITION(myPed, false);
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(myPed, myPos.x, myPos.y, myPos.z, false, false, false);
					}
					QUEUE_JOB_END_CLAUSE
					});

				sub->add_option<RegularOption>(std::move(RegularOption("溢出崩溃", nullptr, [] {
					g_settings.buffer_crash.m_buffer_crash = true;
					}).SetRightText(std::to_string(g_settings.buffer_crash.m_buffer_crash_count).c_str())));
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(ONLINE, "主机工具", host_tools, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("强制成为脚本主机", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						CNetGamePlayer* player = g_pointers->m_get_net_game_player(PLAYER::PLAYER_ID());
						if (player)
						{
							g_pointers->m_forceScriptHost(g_settings.script_handler_net_component_mgr, player,++g_settings.lastToken,1);
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(ONLINE, "信息欺骗", info_spoof, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("开启RID欺骗", nullptr, g_settings.options["spoof_rockstar_id"].get<bool*>());
				sub->add_option<RegularOption>(std::move(RegularOption("RID").SetRightText(std::to_string(g_settings.options["rockstar_id"].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入RID", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["rockstar_id"] = atoi(g_ui_mgr->getInputText().c_str()); }); });
				sub->add_option<BreakOption>("名称");
				sub->add_option<BoolOption<bool>>("开启名称欺骗", nullptr, g_settings.options["spoof_username"].get<bool*>());
				sub->add_option<RegularOption>(std::move(RegularOption("名称").SetRightText(g_settings.options["username"].get<std::string>().c_str())));
				sub->add_option<RegularOption>("输入名称", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["username"] = g_ui_mgr->getInputText(); }); });
				sub->add_option<BreakOption>("主机序列");
				sub->add_option<BoolOption<bool>>("开启主机序列欺骗", nullptr, g_settings.options["spoof_host_token"].get<bool*>());
				sub->add_option<RegularOption>(std::move(RegularOption("主机序列").SetRightText(std::to_string(g_settings.options["host_token"].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入主机序列", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["host_token"] = atoi(g_ui_mgr->getInputText().c_str()); }); });
				sub->add_option<BreakOption>("IP");
				sub->add_option<BoolOption<bool>>("开启IP欺骗", nullptr, g_settings.options["spoof_ip"].get<bool*>());
				sub->add_option<RegularOption>(std::move(RegularOption("IP[1]").SetRightText(std::to_string(g_settings.options["ip"][1].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入IP[1]", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["ip"][1] = atoi(g_ui_mgr->getInputText().c_str()); }); });
				sub->add_option<RegularOption>(std::move(RegularOption("IP[2]").SetRightText(std::to_string(g_settings.options["ip"][2].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入IP[2]", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["ip"][2] = atoi(g_ui_mgr->getInputText().c_str()); }); });
				sub->add_option<RegularOption>(std::move(RegularOption("IP[3]").SetRightText(std::to_string(g_settings.options["ip"][3].get<int>()).c_str())));
				sub->add_option<RegularOption>("输入IP[3]", nullptr, [] {g_ui_mgr->inputBox([] {g_settings.options["ip"][3] = atoi(g_ui_mgr->getInputText().c_str()); }); });
			});

		std::size_t PLAYER_LIST = g_ui_mgr->addSub("玩家列表");
		g_ui_mgr->add_submenu<RegularSubmenu>(PLAYER_LIST, "玩家列表", onlie_option, [](RegularSubmenu* sub)
			{
				for (size_t i = 0; i < 32; i++)
				{
					player_class& pc = g_players.m_players[i];
					if (pc.is_onlie)
					{
						std::string name{};
						name += pc.player_name;
						if (pc.is_host) {
							name += " [主机]";
						}
						if (pc.is_me)
						{
							name += " [我]";
						}
						if (pc.is_script_host)
						{
							name += " [脚本主机]";
						}
						sub->add_option<SubOption>(name.c_str(), nullptr, one_player_option, [i] {g_players.player_id = i; });
					}
				}

				g_settings.spectating = false;
			});
		g_ui_mgr->add_submenu<PlayerSubmenu>(PLAYER_LIST, &g_players.player_id, one_player_option, [](PlayerSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("观看", nullptr, &g_settings.spectating);
				sub->add_option<SubOption>("移除玩家", nullptr, remove_one_player);
				sub->add_option<SubOption>("传送选项", nullptr, teleport_player);
				sub->add_option<SubOption>("友好选项", nullptr, friendly_option);
				sub->add_option<SubOption>("恶搞选项", nullptr, attack_options);
				sub->add_option<RegularOption>("复制服装", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::copy_outfit(g_players.player_id);
					}QUEUE_JOB_END_CLAUSE});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PLAYER_LIST, "恶搞选项", attack_options, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("CeoBan", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						int64_t data[] = { -764524031, g_players.player_id, 1 };

						g_pointers->m_trigger_script_event(1, data, 3, 1 << g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("CeoKick", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						int64_t  data[] = { 248967238, g_players.player_id, 1, 5 };

						g_pointers->m_trigger_script_event(1, data, 4, 1 << g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("传送到小岛", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						int64_t data[] = { -621279188, g_players.player_id, 1, 0, 1 };

						g_pointers->m_trigger_script_event(1, data, 5, 1 << g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("清除通缉等级", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						constexpr size_t arg_count = 3;
						int64_t args[arg_count] = {
							(int)eRemoteEvent::ClearWantedLevel,
							0,
							*script_global(1893548).at(g_players.player_id, 600).at(511).as<int*>()
						};

						g_pointers->m_trigger_script_event(1, args, arg_count, 1 << g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("克隆", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::clone_player(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_players.player_id));
					}QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("关进笼子", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::cage_player(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_players.player_id));
					}QUEUE_JOB_END_CLAUSE });
				sub->add_option<RegularOption>("撞击玩家", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::ram_player();
					}QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("空袭", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::air_strike_player(g_players.player_id);
					}QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("克隆", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::clone_player(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_players.player_id));
					}QUEUE_JOB_END_CLAUSE});
				sub->add_option<BoolOption<bool>>("喷水", nullptr, &g_settings.f_waterloop, false, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::water_loop(g_settings.f_waterloop, g_players.player_id);
					}QUEUE_JOB_END_CLAUSE });
				sub->add_option<BoolOption<bool>>("喷火", nullptr, &g_settings.f_fireloop, false, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::fire_loop(g_settings.f_fireloop, g_players.player_id);
					}QUEUE_JOB_END_CLAUSE});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PLAYER_LIST, "友好选项", friendly_option, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("给所有 MK1 武器", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::give_all_weapons();
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("给所有 MK2 武器", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE() {
						features::give_all_mk2();
					}QUEUE_JOB_END_CLAUSE });
				sub->add_option<RegularOption>("给电击枪", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::give_stun_gun();
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("给Digi Scanner", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::give_digiscanner();
					}
					QUEUE_JOB_END_CLAUSE});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PLAYER_LIST, "移除玩家", remove_one_player, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("主机踢", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						NETWORK::NETWORK_SESSION_KICK_PLAYER(g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("脚本崩溃", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						int64_t data[] = { -1386010354, g_players.player_id, 4212424, 442214, 0, 0, 0, 0, 0, 0 };
						g_pointers->m_trigger_script_event(1, data, 10, 1 << g_players.player_id);
					}
					QUEUE_JOB_END_CLAUSE});
				sub->add_option<BoolOption<bool>>("无效模型类型崩溃", "开启后去其他玩家旁边创建载具即可崩溃", &g_settings.m_spawn_crash_objects);
				sub->add_option<RegularOption>("取消同步踢", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						CNetGamePlayer* player = g_pointers->m_get_net_game_player(g_players.player_id);
						if (player)
						{
							rage::CNetworkPlayerMgr* player_mgr = *g_pointers->m_network_player_mgr;
							if (player_mgr)
							{
								g_pointers->m_remove_player(player_mgr, player);
							}
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>(std::move(RegularOption("取消删除崩溃(P)", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::de_remove_ped_crash();
					}
					QUEUE_JOB_END_CLAUSE
					}).SetRightText(g_settings.m_de_remove_crash ? "正在执行 请不要使用其它崩溃" : "")));
				sub->add_option<RegularOption>(std::move(RegularOption("取消删除崩溃(V)", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::de_remove_veh_crash();
					}
					QUEUE_JOB_END_CLAUSE
					}).SetRightText(g_settings.m_de_remove_crash ? "正在执行 请不要使用其它崩溃" : "")));
				sub->add_option<RegularOption>(std::move(RegularOption("取消删除崩溃(V2)", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::de_remove_veh_crash1();
					}
					QUEUE_JOB_END_CLAUSE
					}).SetRightText(g_settings.m_de_remove_crash ? "正在执行 请不要使用其它崩溃" : "")));
				sub->add_option<RegularOption>("无效handlingType崩溃", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						Ped ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_players.player_id);
						if (ENTITY::DOES_ENTITY_EXIST(ped)) {
							features::spawn_crash_vehicle(ped, "adder", 8, 0);
							features::spawn_crash_vehicle(ped, "blazer", 11, 3);
							features::spawn_crash_vehicle(ped, "dinghy", 1, 13);
							features::spawn_crash_vehicle(ped, "zentorno", 8, 0);
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(PLAYER_LIST, "传送选项", teleport_player, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("传送到他", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						Ped myPed = PLAYER::PLAYER_PED_ID();
						Ped ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_players.player_id);
						Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, false);
						ENTITY::SET_ENTITY_COORDS_NO_OFFSET(myPed, pos.x, pos.y, pos.z, false, false, false);
					}
					QUEUE_JOB_END_CLAUSE
					});
			});

		std::size_t WEAPON = g_ui_mgr->addSub("武器选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(WEAPON, "武器选项", weapon_option, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("获取全部武器", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						for (Hash w : lists::all_weapon) {
							WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), w, 9999, true);
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("移除全部武器", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						WEAPON::REMOVE_ALL_PED_WEAPONS(PLAYER::PLAYER_PED_ID(), false);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("加满所有子弹", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						for (Hash w : lists::all_weapon) {
							WEAPON::SET_PED_AMMO(PLAYER::PLAYER_PED_ID(), w, 9999, true);
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<BreakOption>("特殊");
				sub->add_option<BoolOption<bool>>("无限弹药", nullptr, g_settings.options["unlimited_ammo"].get<bool*>());
				sub->add_option<BoolOption<bool>>("爆炸弹药", nullptr, g_settings.options["explosive_ammo"].get<bool*>());
				sub->add_option<BoolOption<bool>>("快速射击", "无视换弹CD 按下鼠标右键即可持续射击", g_settings.options["rapid_fire"].get<bool*>());
			});

		std::size_t VEHICLE = g_ui_mgr->addSub("载具选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(VEHICLE, "载具选项", vehicle_option, [](RegularSubmenu* sub)
			{
				sub->add_option<SubOption>("创建载具", nullptr, create_vehicle);
				sub->add_option<BoolOption<bool>>("载具无敌", nullptr, g_settings.options["vehicle_invincible"].get<bool*>());
				sub->add_option<BoolOption<bool>>("安全带", nullptr, g_settings.options["safety_belt"].get<bool*>());
				sub->add_option<BoolOption<bool>>("无限氮气", "具有喷射功能的载具可一直喷射 空格停喷", g_settings.options["infinite_nitrogen"].get<bool*>());
				sub->add_option<BoolOption<bool>>("按9提速", nullptr, g_settings.options["9_key_speed_up"].get<bool*>());
				sub->add_option<BoolOption<bool>>("彩虹载具", nullptr, g_settings.options["rainbow_vehicle"].get<bool*>());
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(VEHICLE, "创建载具", create_vehicle, [](RegularSubmenu* sub)
			{
				sub->add_option<BoolOption<bool>>("删除上一创建载具", "删除不用的载具", g_settings.options["delete_last_vehicle"].get<bool*>());
				sub->add_option<BoolOption<bool>>("创建满改载具", nullptr, g_settings.options["spwan_vehicle_max"].get<bool*>());
				sub->add_option<BoolOption<bool>>("玩家进入", nullptr, g_settings.options["spwan_vehicle_in"].get<bool*>());
				sub->add_option<SubOption>("创建XML载具", "创建XML模组载具", create_xml_vehicle);
				sub->add_option<SubOption>("创建INI载具", "创建INI模组载具", create_ini_vehicle);
				sub->add_option<BreakOption>("载具列表");
				for (VehicleClass& vc : g_vehicles.classes)
				{
					sub->add_option<VehicleOption>(vc.className.c_str(), nullptr, vc.vehicles, &vc.p, false, [vc] {
						QUEUE_JOB_BEGIN_CLAUSE(vc)
						{
							features::spawn_vehicle(vc.vehicles[vc.p].hash);
						}
						QUEUE_JOB_END_CLAUSE
						});
				}
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(VEHICLE, "创建XML载具", create_xml_vehicle, [&](RegularSubmenu* sub)
			{
				for (std::string& str : m_xmlVehicleList)
				{
					sub->add_option<RegularOption>(str.c_str(), nullptr, [&] {
						std::string file = std::string(MENU_PATH) + "\\vehicle\\xml\\" + str + ".xml";

						QUEUE_JOB_BEGIN_CLAUSE(file)
						{
							modvehicle::create_xml_vehicle(file.c_str());
						}
						QUEUE_JOB_END_CLAUSE
						});
				}
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(VEHICLE, "创建INI载具", create_ini_vehicle, [&](RegularSubmenu* sub)
			{
				for (std::string& str : m_iniVehicleList)
				{
					sub->add_option<RegularOption>(str.c_str(), nullptr, [&] {
						std::string file = std::string(MENU_PATH) + "\\vehicle\\ini\\" + str + ".ini";
						QUEUE_JOB_BEGIN_CLAUSE(file)
						{
							modvehicle::create_ini_vehicle(file.c_str());
						}
						QUEUE_JOB_END_CLAUSE
						});
				}
			});

		std::size_t TELEPORT = g_ui_mgr->addSub("传送选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(TELEPORT, "传送选项", teleport_option, [](RegularSubmenu* sub)
			{
				sub->add_option<RegularOption>("传送到导航点", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::tp_to_waypoint();
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("传送到任务点", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						features::teleport_to_objective();
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<BreakOption>("常用地点");
				sub->add_option<RegularOption>("C山", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ 336.158, 4373.934, 64.46 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("水下船", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ 3156.38355,-355.16663, -20.09742 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("海洞", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ 3067.58, 2212.25, 3.00 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("伽利略天文台", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -430.905, 1135.2722, 325.904 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("伽利略天公园", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ 719.30,1204.76,325.88 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("海滩", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -256.43,6569.93, 2.66 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("矿井", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -604.92, 2113.95, 127.11 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("迷宫银行", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -74.94243,-818.63446, 326.174347 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("军事基地", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -2012.8470f, 2956.5270f, 32.8101f }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("赞库多塔", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -2356.0940, 3248.645, 101.4505 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("面具店", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -1338.16, -1278.11, 4.87 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("LSC", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -373.01, -124.91, 38.31 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("武器店", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ 247.3652, -45.8777, 69.9411 }); }QUEUE_JOB_END_CLAUSE});
				sub->add_option<RegularOption>("飞机场", nullptr, [] {	QUEUE_JOB_BEGIN_CLAUSE() { features::tp_to_vector3({ -1102.2910f, -2894.5160f, 13.9467f }); }QUEUE_JOB_END_CLAUSE});
			});

		std::size_t TASK = g_ui_mgr->addSub("任务选项");
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "任务选项", task_option, [](RegularSubmenu* sub)
			{
				sub->add_option<SubOption>("佩里科岛", nullptr, pericoIsland);
				sub->add_option<SubOption>("赌场豪劫", nullptr, casinorobbery);
				sub->add_option<SubOption>("末日抢劫", nullptr, doomsdayheist);
				sub->add_option<SubOption>("公寓抢劫", nullptr, apartment);
				sub->add_option<SubOption>("合约任务", nullptr, contracttask);
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "佩里科岛", pericoIsland, [](RegularSubmenu* sub)
			{
				static int player1 = 100, player2 = 100, player3 = 100, player4 = 100;
				sub->add_option<RegularOption>("重置面板", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_MISSIONS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_PROGRESS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_PLAYTHROUGH_STATUS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_APPROACH"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_BS_ENTR"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_BS_GEN"), 0, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_MISSIONS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_PROGRESS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_PLAYTHROUGH_STATUS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_APPROACH"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_BS_ENTR"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_BS_GEN"), 0, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("跳过前置", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_BS_GEN"), 131071, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_BS_ENTR"), 63, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_BS_ABIL"), 63, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_APPROACH"), -1, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_PROGRESS"), 131055, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_TARGET"), 5, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_MISSIONS"), 65345, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_WEAPONS"), 2, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4CNF_TROJAN"), 1, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H4_PLAYTHROUGH_STATUS"), 10, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_BS_GEN"), 131071, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_BS_ENTR"), 63, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_BS_ABIL"), 63, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_APPROACH"), -1, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_PROGRESS"), 131055, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_TARGET"), 5, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_MISSIONS"), 65345, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_WEAPONS"), 2, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4CNF_TROJAN"), 1, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H4_PLAYTHROUGH_STATUS"), 10, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<NumberOption<int>>("玩家1分红", nullptr, &player1, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1973496).at(823).at(56).at(1).as<int*>() = player1;
					});
				sub->add_option<NumberOption<int>>("玩家2分红", nullptr, &player2, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1973496).at(823).at(56).at(2).as<int*>() = player2;
					});
				sub->add_option<NumberOption<int>>("玩家3分红", nullptr, &player3, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1973496).at(823).at(56).at(3).as<int*>() = player3;
					});
				sub->add_option<NumberOption<int>>("玩家4分红", nullptr, &player4, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1973496).at(823).at(56).at(4).as<int*>() = player4;
					});
				sub->add_option<RegularOption>("杀死队友", nullptr, [] {
					for (int i = 0; i < 4; i++) {
						if (ENTITY::DOES_ENTITY_EXIST(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i)) && PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i) != PLAYER::PLAYER_ID()) {
							Vector3 Coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i), 1);
							FIRE::ADD_EXPLOSION(Coords.x, Coords.y, Coords.z, 0, 1, 1, 0, 1, 0);
						}
					}
					});
				sub->add_option<RegularOption>("传送雕像", nullptr, [] {
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 5006.70, -5755.89, 15.48, 1, 0, 0, 0);
					script::get_current()->yield();
					});
				sub->add_option<RegularOption>("传送大门", nullptr, [] {
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 5006.70, -5755.89, 15.48, 1, 0, 0, 0);
					script::get_current()->yield();
					});
				sub->add_option<RegularOption>("逃跑", nullptr, [] {
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 4580, -6012, 3, 1, 0, 0, 0);
					script::get_current()->yield();
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "赌场豪劫", casinorobbery, [](RegularSubmenu* sub)
			{
				static int player1 = 100, player2 = 100, player3 = 100, player4 = 100;
				sub->add_option<RegularOption>("跳过前置(兵不厌炸)", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_TARGET"), 3, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_TARGET"), 3, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_APPROACH"), 2, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_APPROACH"), 2, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_ACCESSPOINTS"), -1, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_ACCESSPOINTS"), -1, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_POI"), -1, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_POI"), -1, 0);
						script::get_current()->yield(50ms);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET1"), -1, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET1"), -1, 0);
						script::get_current()->yield(2000ms);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_DISRUPTSHIP"), 3, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_DISRUPTSHIP"), 3, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_KEYLEVELS"), 2, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_KEYLEVELS"), 2, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWDRIVER"), 5, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWDRIVER"), 5, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWWEAP"), 4, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWWEAP"), 4, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWHACKER"), 4, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWHACKER"), 4, 0);
						script::get_current()->yield(50ms);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET0"), -1, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET0"), -1, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("重置第一块板", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET1"), 0, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET1"), 0, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("重置第二块板", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET0"), 0, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET0"), 0, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<NumberOption<int>>("玩家1分红", nullptr, &player1, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1966718).at(1497).at(736).at(92).at(1).as<int*>() = player1;
					});
				sub->add_option<NumberOption<int>>("玩家2分红", nullptr, &player2, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1966718).at(1497).at(736).at(92).at(2).as<int*>() = player2;
					});
				sub->add_option<NumberOption<int>>("玩家3分红", nullptr, &player3, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1966718).at(1497).at(736).at(92).at(3).as<int*>() = player3;
					});
				sub->add_option<NumberOption<int>>("玩家4分红", nullptr, &player4, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1966718).at(1497).at(736).at(92).at(4).as<int*>() = player4;
					});
				sub->add_option<RegularOption>("删除NPC分红", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWWEAP"), 6, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWDRIVER"), 6, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_CREWHACKER"), 6, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWWEAP"), 6, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWDRIVER"), 6, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_CREWHACKER"), 6, 0);
						*script_global(262145).at(28439).as<float*>() = 0;
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<RegularOption>("修复拍照", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET1"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_BITSET0"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_POI"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_H3OPT_ACCESSPOINTS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_CAS_HEIST_FLOW"), 0, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET1"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_BITSET0"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_POI"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_H3OPT_ACCESSPOINTS"), 0, 0);
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_CAS_HEIST_FLOW"), 0, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "末日抢劫", doomsdayheist, [](RegularSubmenu* sub)
			{
				static int player1 = 100, player2 = 100, player3 = 100, player4 = 100;
				static int endDay_combo_pos = 0;
				static std::vector<char*> endDay_combo = {
					"数据泄露",
					"波格丹危机",
					"末日将至"
				};
				sub->add_option<ChooseOption<char*, int>>("选择前置", nullptr, &endDay_combo, &endDay_combo_pos);
				sub->add_option<RegularOption>("跳过前置", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						switch (endDay_combo_pos)
						{
						case 0:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_MISSION_PROG"), 503, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_HEIST_STATUS"), 229383, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_MISSION_PROG"), 503, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_HEIST_STATUS"), 229383, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//	features::notifyfmt("已跳过前置:~b~数据泄露");
							break;
						}
						case 1:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_MISSION_PROG"), 240, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_HEIST_STATUS"), 229378, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_MISSION_PROG"), 240, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_HEIST_STATUS"), 229378, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//	features::notifyfmt("已跳过前置:~b~波格丹危机");
							break;
						}
						case 2:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_MISSION_PROG"), 16368, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_HEIST_STATUS"), 229380, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_MISSION_PROG"), 16368, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_HEIST_STATUS"), 229380, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_GANGOPS_FLOW_NOTIFICATIONS"), 1557, 0);
							//	features::notifyfmt("已跳过前置:~b~末日将至");
							break;
						}
						}
					}
					QUEUE_JOB_END_CLAUSE
					});

				sub->add_option<NumberOption<int>>("玩家1分红", nullptr, &player1, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1962755).at(812).at(50).at(1).as<int*>() = player1;
					});
				sub->add_option<NumberOption<int>>("玩家2分红", nullptr, &player2, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1962755).at(812).at(50).at(2).as<int*>() = player2;
					});
				sub->add_option<NumberOption<int>>("玩家3分红", nullptr, &player3, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1962755).at(812).at(50).at(3).as<int*>() = player3;
					});
				sub->add_option<NumberOption<int>>("玩家4分红", nullptr, &player4, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1962755).at(812).at(50).at(4).as<int*>() = player4;
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "公寓抢劫", apartment, [](RegularSubmenu* sub)
			{
				static int player1 = 100, player2 = 100, player3 = 100, player4 = 100;
				sub->add_option<RegularOption>("跳过前置", "动画中使用", [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_HEIST_PLANNING_STAGE"), -1, 0);
						//
						STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_HEIST_PLANNING_STAGE"), -1, 0);
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<NumberOption<int>>("玩家1分红", nullptr, &player1, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1934631).at(3008).at(1).as<int*>() = player1;
					});
				sub->add_option<NumberOption<int>>("玩家2分红", nullptr, &player2, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1934631).at(3008).at(2).as<int*>() = player2;
					});
				sub->add_option<NumberOption<int>>("玩家3分红", nullptr, &player3, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1934631).at(3008).at(3).as<int*>() = player3;
					});
				sub->add_option<NumberOption<int>>("玩家4分红", nullptr, &player4, 0, 100);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(1934631).at(3008).at(4).as<int*>() = player4;
					});
			});
		g_ui_mgr->add_submenu<RegularSubmenu>(TASK, "合约任务", contracttask, [](RegularSubmenu* sub)
			{
				static std::vector<const char*> Contract_combo = {
					"夜生活泄密",
					"上流社会泄密",
					"南中心泄密",
					"分红关(档案任务)(xx博士)",
					"通关"
				};
				static int Contract_combo_pos = 0;
				static int finalMoney = 1000000;
				sub->add_option<ChooseOption<const char*, int>>("选择前置", nullptr, &Contract_combo, &Contract_combo_pos);
				sub->add_option<RegularOption>("跳过前置", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						switch (Contract_combo_pos)
						{
						case 0:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_BS"), 28, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_COOLDOWN"), -1, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_BS"), 28, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_COOLDOWN"), -1, 0);
							//	features::notifyfmt("已跳过任务:~p~夜生活泄密");
							break;
						}
						case 1:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_BS"), 124, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_COOLDOWN"), -1, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_BS"), 124, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_COOLDOWN"), -1, 0);
							//	features::notifyfmt("已跳过任务:~p~上流社会泄密");
							break;
						}
						case 2:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_BS"), 2044, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_COOLDOWN"), -1, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_BS"), 2044, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_COOLDOWN"), -1, 0);
							//features::notifyfmt("已跳过任务:~p~南中心泄密");
							break;
						}
						case 3:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_BS"), 4092, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_COOLDOWN"), -1, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_BS"), 4092, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_COOLDOWN"), -1, 0);
							//	features::notifyfmt("已跳过任务:~p~分红关(档案任务)(xx博士)");
							break;
						}
						case 4:
						{
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP0_FIXER_STORY_COOLDOWN"), -1, 0);
							//
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_GENERAL_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_COMPLETED_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_BS"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_STRAND"), -1, 0);
							STATS::STAT_SET_INT(MISC::GET_HASH_KEY("MP1_FIXER_STORY_COOLDOWN"), -1, 0);
							//	features::notifyfmt("已一键通关");
							break;
						}
						}
					}
					QUEUE_JOB_END_CLAUSE
					});
				sub->add_option<NumberOption<int>>("最终收入", "任务开始后更改 失败需重新设置", &finalMoney, 0, 1000000);
				sub->add_option<RegularOption>("修改", nullptr, [] {
					*script_global(262145).at(31373).as<int*>() = finalMoney;
					});
				sub->add_option<RegularOption>("传送到工厂约翰尼贡斯位置", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 510.23, -667.92, 24.86, 1, 0, 0, 0);
						script::get_current()->yield();
					}
					QUEUE_JOB_END_CLAUSE					});
				sub->add_option<RegularOption>("传送到机库约翰尼贡斯", nullptr, [] {
					QUEUE_JOB_BEGIN_CLAUSE()
					{
						ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), -935, -3018, 19, 1, 0, 0, 0);
						script::get_current()->yield();
					}
					QUEUE_JOB_END_CLAUSE
					});
			});

		std::size_t SETTINGS = g_ui_mgr->addSub("菜单设置");
		g_ui_mgr->add_submenu<RegularSubmenu>(SETTINGS, "菜单设置", menu_seetings, [](RegularSubmenu* sub)
			{
			});
	}

	void main_gui::tick()
	{
		if (m_initialized)
		{
			g_ui_mgr->onTick();
		}
		else
		{
			initialize();

			std::string path = MENU_PATH"\\vehicle\\ini\\";
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				std::string str = entry.path().generic_u8string();
				if (str.find(".ini"))
				{
					std::size_t found = str.find_last_of(".ini");
					str = str.substr(0, found - 3);

					found = str.find_last_of("/\\");
					str = str.substr(found + 1);
					m_iniVehicleList.push_back(str);
				}
			}

			path = MENU_PATH"\\vehicle\\xml\\";
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				std::string str = entry.path().generic_u8string();
				if (str.find(".xml"))
				{
					std::size_t found = str.find_last_of(".xml");
					str = str.substr(0, found - 3);

					found = str.find_last_of("/\\");
					str = str.substr(found + 1);
					m_xmlVehicleList.push_back(str);
				}
			}
		}
	}
}