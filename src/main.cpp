#include "BoundWeapons.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto vssink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#endif

		auto path = SKSE::log::log_directory();
		if (!path)
		{
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

#ifndef NDEBUG
		spdlog::logger multilog("multi_sink", { sink, vssink });

		auto log = std::make_shared<spdlog::logger>(std::move(multilog));

		const auto level = spdlog::level::trace;
		log->set_level(level);
		log->flush_on(level);
#else
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

		const auto level = spdlog::level::info;
		log->set_level(level);
		log->flush_on(level);
#endif

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName(Plugin::AUTHOR);
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	//v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

void MessagingCallback(SKSE::MessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
		{
			BoundWeapons::UpdateData();
		}
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * skse)
{
	InitializeLog();
	std::string version = Plugin::VERSION.string();
	SKSE::log::info("{} v{}"sv, Plugin::NAME, version);

	SKSE::Init(skse);

	const SKSE::MessagingInterface* messaging = SKSE::GetMessagingInterface();
	if (!messaging)
	{
		SKSE::log::error("Unable to get messaging interface. Aborting plugin load."sv);
		return false;
	}
	messaging->RegisterListener(MessagingCallback);

	SKSE::AllocTrampoline(128);

	BoundWeapons::Init(SKSE::GetTrampoline());

	return true;
}
