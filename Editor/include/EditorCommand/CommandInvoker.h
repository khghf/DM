#pragma once
#include"ICommand.h"
#include<type_traits>
#include"CommandHistory.h"
namespace DM
{
	struct CommandInvoker
	{
		template<typename Cmd>requires std::is_base_of_v<ICommand, Cmd>
		static void Invoke()
		{
			Cmd* cmd = new Cmd{};
			cmd->Execute();
			CommandHistory::Get()->Record(cmd);
		}
	};
}
