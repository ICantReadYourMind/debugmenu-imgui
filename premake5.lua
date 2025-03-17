workspace "debugmenu-imgui"
	configurations { "ReleaseSA", "DebugSA", "DebugIII", "ReleaseVC", "DebugVC", "ReleaseIII", "DebugIII" }
	location "project_files"
   
project "debugmenu-imgui"
	kind "SharedLib"
	language "C++"
	targetdir "output/dll/"
	objdir ("output/obj")
	targetextension ".dll"
	characterset ("MBCS")
	linkoptions "/SAFESEH:NO"
	buildoptions { "-std:c++latest", "/permissive" }
	defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NON_CONFORMING_SWPRINTFS", "_USE_MATH_DEFINES", "RW" }
	disablewarnings { "4244", "4800", "4305", "4073", "4838", "4996", "4221", "4430", "26812", "26495", "6031" }
    cppdialect "C++latest"

	files {
		"source/**.*",
	}
	
	includedirs { 
		"source/**",
	}
	
	includedirs {
		"$(PLUGIN_SDK_DIR)/shared/rwd3d9/",
		"$(PLUGIN_SDK_DIR)/shared/",
		"$(PLUGIN_SDK_DIR)/shared/game/",
	}
	
	filter { "configurations:*III" }
		defines { "GTA3", "PLUGIN_SGV_10EN" }
		includedirs {
			"$(PLUGIN_SDK_DIR)/plugin_III/",
			"$(PLUGIN_SDK_DIR)/plugin_III/game_III/",
			"$(PLUGIN_SDK_DIR)/plugin_III/game_III/rw",
		}
		targetname "debugmenu-imgui-III"
		debugdir "$(GTA_III_DIR)"
		debugcommand "$(GTA_III_DIR)/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_III_DIR)\\debugmenu.dll\""
		
	filter { "configurations:*VC" }	
		defines { "GTAVC", "PLUGIN_SGV_10EN" }	
		includedirs {
			"$(PLUGIN_SDK_DIR)/plugin_vc/",
			"$(PLUGIN_SDK_DIR)/plugin_vc/game_vc/",
			"$(PLUGIN_SDK_DIR)/plugin_vc/game_vc/rw",
		}
		targetname "debugmenu-imgui-VC"
		debugdir "$(GTA_VC_DIR)"
		debugcommand "$(GTA_VC_DIR)/gta-vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_VC_DIR)\\debugmenu.dll\""
		
	filter { "configurations:*SA" }	
		defines { "GTASA", "PLUGIN_SGV_10EN" }	
		includedirs {
			"$(PLUGIN_SDK_DIR)/plugin_sa/",
			"$(PLUGIN_SDK_DIR)/plugin_sa/game_sa/",
			"$(PLUGIN_SDK_DIR)/plugin_sa/game_sa/rw",
		}
		targetname "debugmenu-imgui-SA"
		debugdir "$(GTA_SA_DIR)"
		debugcommand "$(GTA_SA_DIR)/gta_sa.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_SA_DIR)\\debugmenu.dll\""
	
	filter { }
	
	libdirs { 
		"$(PLUGIN_SDK_DIR)/output/lib/",
		"$(PLUGIN_SDK_DIR)/shared/rwd3d9/",
	}
	
	filter "configurations:Debug*"		
		defines { "DEBUG" }
		symbols "on"
		staticruntime "on"

	filter "configurations:Release*"
		defines { "NDEBUG" }
		symbols "off"
		optimize "On"
		staticruntime "on"
		
	filter "configurations:ReleaseSA"
		links { "plugin" }
	filter "configurations:ReleaseVC"
		links { "plugin_vc", "rwd3d9" }
	filter "configurations:ReleaseIII"
		links { "plugin_iii", "rwd3d9" }
		
	filter "configurations:DebugSA"
		links { "plugin_d" }	
	filter "configurations:DebugVC"
		links { "plugin_vc_d", "rwd3d9" }
	filter "configurations:DebugIII"
		links { "plugin_iii_d", "rwd3d9" }
			
	filter { }
