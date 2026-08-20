#pragma once
#include<queue>
namespace DM
{
	class ICommand;
	class CommandHistory
	{
		CommandHistory();
	public:
		~CommandHistory();

		static CommandHistory* Get()
		{
			static CommandHistory inst;
			return &inst;
		}

		void Record(ICommand* cmd)
		{
			m_UndoList.push_back(cmd);
		}

		void Undo();
		void Redo();
	private:
		std::deque<ICommand*>m_UndoList;
		std::deque<ICommand*>m_RedoList;

		int m_Capacity = 100;
	};
}