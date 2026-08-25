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

		void Record(ICommand* cmd);

		void Undo();
		void Redo();

		bool CanUndo() const { return !m_UndoList.empty(); }
		bool CanRedo() const { return !m_RedoList.empty(); }
		int GetUndoCount() const { return static_cast<int>(m_UndoList.size()); }
		int GetRedoCount() const { return static_cast<int>(m_RedoList.size()); }

	private:
		std::deque<ICommand*>m_UndoList;
		std::deque<ICommand*>m_RedoList;

		int m_Capacity = 100;
	};
}
