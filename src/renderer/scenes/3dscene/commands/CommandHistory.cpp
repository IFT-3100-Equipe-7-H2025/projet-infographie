#include "CommandHistory.h"

void CommandHistory::executeCommand(std::shared_ptr<Command> command)
{
    command->Execute();
    this->undoStack.emplace_back(command);
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
    this->undoStack.push_back(command);
}
