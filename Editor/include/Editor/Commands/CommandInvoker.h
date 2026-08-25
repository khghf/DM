#pragma once
#include"ICommand.h"
#include<type_traits>
#include<utility>
#include"CommandHistory.h"
namespace DM
{
	struct CommandInvoker
	{
		template<typename Cmd, typename... Args>requires std::is_base_of_v<ICommand, Cmd>
		static void Invoke(Args&&... args)
		{
			Cmd* cmd = new Cmd{ std::forward<Args>(args)... };
			if (cmd->Execute())
			{
				CommandHistory::Get()->Record(cmd);
			}
			else
			{
				delete cmd;
			}
		}
	};
}
