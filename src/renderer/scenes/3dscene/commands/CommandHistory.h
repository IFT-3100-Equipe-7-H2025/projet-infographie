#pragma once

#include "3dscene/commands/Command.h"
#include <memory>
#include <vector>

class CommandHistory
{
public:
    CommandHistory() : undoStack(), redoStack() {}

    void executeCommand(std::shared_ptr<Command> command);
    void undo();
    void redo();

    [[nodiscard]] std::vector<std::shared_ptr<Command>>& GetUndoStack() { return undoStack; }
    [[nodiscard]] std::vector<std::shared_ptr<Command>>& GetRedoStack() { return redoStack; }

private:
    std::vector<std::shared_ptr<Command>> undoStack;
    std::vector<std::shared_ptr<Command>> redoStack;
};
