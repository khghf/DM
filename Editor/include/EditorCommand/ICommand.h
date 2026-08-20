#pragma once
#include<string>
namespace DM
{
	struct ICommand
	{
		virtual void Execute() = 0;
		virtual void Undo() = 0;
	protected:
		std::string m_CmdName;
	};

	struct CmdNewWorld:ICommand
	{
		CmdNewWorld() { m_CmdName="NewWorld"; }
		virtual void Execute()override;
		virtual void Undo()override;
	private:
		std::string m_Path;
	};


}