#include<Editor/Commands/CommandHistory.h>
#include<Editor/Commands/ICommand.h>

#include<algorithm>
namespace DM
{
	CommandHistory::CommandHistory()
	{
	}

	CommandHistory::~CommandHistory()
	{
		std::for_each(m_RedoList.begin(), m_RedoList.end(), [](ICommand* ele) {delete ele; });
		std::for_each(m_UndoList.begin(), m_UndoList.end(), [](ICommand* ele) {delete ele; });
	}

	void CommandHistory::Record(ICommand* cmd)
	{
		if (!cmd) return;

		while (!m_RedoList.empty())
		{
			delete m_RedoList.front();
			m_RedoList.pop_front();
		}

		//所有被记录的操作都会放入undo列表
		m_UndoList.push_back(cmd);

		// undo 列表同样限容量，避免无限增长
		while (m_UndoList.size() > static_cast<size_t>(m_Capacity))
		{
			delete m_UndoList.front();
			m_UndoList.pop_front();
		}
	}

	void CommandHistory::Undo()
	{
		if (m_UndoList.empty())return;

		ICommand* cmd = m_UndoList.back();
		m_UndoList.pop_back();

		cmd->Undo();
		m_RedoList.push_back(cmd);

		if (m_RedoList.size() > static_cast<size_t>(m_Capacity))
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

		// 调用命令级 Redo 重放副作用，而非重新 Execute(Execute 可能弹对话框/依赖选中状态)
		cmd->Redo();
		m_UndoList.push_back(cmd);

		if (m_UndoList.size() > static_cast<size_t>(m_Capacity))
		{
			delete m_UndoList.front();
			m_UndoList.pop_front();
		}
	}
}
