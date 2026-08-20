#include<EditorCommand/CommandHistory.h>
#include<EditorCommand/ICommand.h>

#include<algorithm>
namespace DM
{
	CommandHistory::CommandHistory()
	{
		/*int capcity = 100;
		m_RedoList.resize(capcity);
		m_UndoList.resize(capcity);*/
	}

	CommandHistory::~CommandHistory()
	{
		std::for_each(m_RedoList.begin(), m_RedoList.end(), [](ICommand* ele) {delete ele; });
		std::for_each(m_UndoList.begin(), m_UndoList.end(), [](ICommand* ele) {delete ele; });
	}
	void CommandHistory::Undo()
	{
		if (m_UndoList.empty())return;

		ICommand* cmd = m_UndoList.back();
		m_UndoList.pop_back();

		cmd->Undo();
		m_RedoList.push_back(cmd);

		if (m_RedoList.size() > m_Capacity)
		{
			delete m_RedoList.front();
			m_RedoList.pop_front();
		}
	}

	void CommandHistory::Redo()
	{
		if (m_RedoList.empty())return;

		ICommand* cmd = m_RedoList.back();
		m_RedoList.pop_back();

		cmd->Execute();
		m_UndoList.push_back(cmd);

		if (m_UndoList.size() > m_Capacity)
		{
			delete m_UndoList.front();
			m_UndoList.pop_front();
		}
	}
}