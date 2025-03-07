#include "CommandHistory.h"

void CommandHistory::executeCommand(std::shared_ptr<Command> command)
{
    if ( !command->Execute() ) { return; }
    if (this->undoStack.size() >= MAX_COMMAND_HISTORY)
    {
        this->undoStack.erase(this->undoStack.begin());
    }
    this->undoStack.emplace_back(std::move(command));
    this->redoStack = std::vector<std::shared_ptr<Command>>();
}

void CommandHistory::undo()
{
    if (undoStack.empty())
    {
        return;
    }

    auto command = this->undoStack.back();
    this->undoStack.pop_back();
    command->Undo();

    if (this->redoStack.size() >= MAX_COMMAND_HISTORY)
    {
        this->redoStack.erase(this->redoStack.begin());
    }
    this->redoStack.emplace_back(command);
}

void CommandHistory::redo()
{
    if (this->redoStack.empty())
    {
        return;
    }

    auto command = this->redoStack.back();
    this->redoStack.pop_back();
    command->Execute();

    if (this->undoStack.size() >= MAX_COMMAND_HISTORY)
    {
        this->undoStack.erase(this->undoStack.begin());
    }
    this->undoStack.push_back(command);
}
